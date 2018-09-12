
#include "qkf/queue.h"
#include "qkf/time.h"
#include "qkf/malloc.h"
#include "qkf/sync.h"
#include <stdlib.h>
#include <string.h>

bool qkf_queue_init(qkf_queue_t * queue , size_t capacity) 
{
    if(queue == NULL || capacity == 0)
        return false ;

    queue->nodes = NULL ;
    queue->capacity = queue->head = queue->tail = 0 ;
    queue->encounter = queue->decounter = 0 ;
    queue->ewtimes = queue->dwtimes = 0 ;

    queue->finaled = queue->ewaiting = queue->dwaiting = false ;
    pthread_spin_init(&queue->elocker , 0) ;
    pthread_spin_init(&queue->dlocker , 0) ;

    pthread_mutex_init(&queue->guard , NULL) ;
    sem_init(&queue->esema , 0 , 1) ;
    sem_init(&queue->dsema , 0 , 1) ;

    return true ;
}

void qkf_queue_final(qkf_queue_t * queue) 
{
    if(queue == NULL || queue->finaled == true)
        return ;

    qkf_queue_node_t ** nodes = NULL ;

    queue->finaled = true ;   

    pthread_spin_lock(&queue->elocker) ;
    pthread_spin_lock(&queue->dlocker) ;

    nodes = queue->nodes ;
    queue->nodes = NULL ;

    queue->head = queue->tail = queue->capacity = 0 ;

    pthread_spin_unlock(&queue->dlocker) ;
    pthread_spin_unlock(&queue->elocker) ;

    if(nodes != NULL)
        ::qkf_free(kDefaultMMgr , nodes) ;
}

void qkf_queue_clear(qkf_queue_t * queue) 
{
    if(queue == NULL)
        return ;

    pthread_spin_lock(&queue->elocker) ;
    pthread_spin_lock(&queue->dlocker) ;

    queue->head = queue->tail = 0 ;

    pthread_spin_unlock(&queue->dlocker) ;
    pthread_spin_unlock(&queue->elocker) ;
}

bool qkf_queue_try_push(qkf_queue_t * queue , qkf_queue_node_t * node)
{
    if(queue == NULL || node == NULL)
        return false ;

    int64_t ts = qkf_now_usec() ;
    pthread_spin_lock(&queue->elocker) ;

    if(qkf_queue_full(queue) == true || queue->nodes == NULL || queue->capacity == 0)
    {
        pthread_spin_unlock(&queue->elocker) ;
        return false ;    
    }

    /*
        在多核机器中，发现++tail_先完成，但读线程仍然读到旧的数据。因此必须加上内存屏障。
        现在，我在pthread_spin_unlock中加入了内存屏障，这里是否还需要内存屏障，留待测试。
    */
    size_t idx = queue->tail % queue->capacity ;
    node->timestamp = ts ;
    queue->nodes[idx] = node ;
    qkf_memory_barrier() ;
    ++queue->tail ;

    pthread_spin_unlock(&queue->elocker) ;
    return true ;
}

bool qkf_queue_push(qkf_queue_t * queue , qkf_queue_node_t * node , int timeout) 
{
    if(timeout == 0)
        return qkf_queue_try_push(queue , node) ;

    if(queue == NULL || node == NULL || queue->nodes == NULL)
        return false ;

    bool result = false ;
    if(timeout < 0)
        timeout = 3600000 ;

    int64_t begin = qkf_now_msec() ;
    bool need_notify = false ;
    
    pthread_spin_lock(&queue->elocker) ;
    do{
        if(qkf_queue_full(queue) == false)
        {
            result = true ;
            break ;
        }

        queue->ewaiting = true ;
        ++queue->ewtimes ;
        pthread_spin_unlock(&queue->elocker) ;

        struct timespec ts ;
        qkf_next_nsec(&ts , 50000000) ;
        sem_timedwait(&queue->esema , &ts) ;

        pthread_spin_lock(&queue->elocker) ;
        int64_t now = qkf_now_msec() ;
        if((int)(now - begin) >  timeout)
            break ;       
    } while(queue->finaled == false) ;

    queue->ewaiting = false ;
    if(result == true)
    {
        size_t idx = queue->tail % queue->capacity ;
        node->timestamp = qkf_now_usec() ;
        queue->nodes[idx] = node ;
        qkf_memory_barrier() ;
        ++queue->tail ;


        if(queue->dwaiting == true)
        {
            ++queue->decounter ;
            need_notify = true ;
        }
    }
    pthread_spin_unlock(&queue->elocker) ;

    if(need_notify == true)
        sem_post(&queue->dsema) ;  

    return true ;
}

bool qkf_queue_try_pop(qkf_queue_t * queue , qkf_queue_node_t ** node) 
{
    if(queue == NULL || queue->nodes == NULL || node == NULL)
        return false ;

    pthread_spin_lock(&queue->dlocker) ;

    if(qkf_queue_empty(queue) == true)
    {
        pthread_spin_unlock(&queue->dlocker) ;
        *node = NULL ;
        return true ;
    }

    size_t idx = queue->head % queue->capacity ;
    *node = queue->nodes[idx] ;
    if(qkf_queue_full(queue) == true)
        ++queue->decounter ;
    ++queue->head ;
    pthread_spin_unlock(&queue->dlocker) ;

    return true ;
}

int qkf_queue_pop(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes , int timeout) 
{
    if(timeout == 0)
    {
        if(max_nodes <= 0 || nodes == NULL)
            return -1 ;

        qkf_queue_node_t * node = NULL ;
        int count = qkf_queue_try_pop(queue , &node) ;
        if(count <= 0)
            return count ;
        nodes[0] = node ;
        return 1 ;
    }

    if(queue == NULL || nodes == NULL || queue->nodes == NULL || max_nodes <= 0)
        return -1 ;

    if(timeout < 0)
        timeout = 360000 ;
    int64_t begin = qkf_now_msec() ;
    bool result = false , need_notify = false;

    pthread_spin_lock(&queue->dlocker) ;
    do{
        if(qkf_queue_empty(queue) == false)
        {
            result = true ;
            break ;
        }

        queue->dwaiting = true ;
        ++queue->dwtimes ;
        pthread_spin_unlock(&queue->dlocker) ;

        struct timespec ts ;
        qkf_next_nsec(&ts , 50000000) ;
        sem_timedwait(&queue->dsema , &ts) ;

        int64_t now = qkf_now_msec() ;
        pthread_spin_lock(&queue->dlocker) ;
        if((int)(now - begin) > timeout)
            break ;    
    }while(queue->finaled == false) ;

    queue->dwaiting = false ;
    int counter = 0 ;
    if(result == true)
    {
        size_t idx = queue->head % queue->capacity ;
        size_t count = queue->tail - queue->head ;

        while((counter < max_nodes) && (counter < (int)count))
        {
            if(idx > queue->capacity)
                idx = 0 ;

            nodes[counter] = queue->nodes[idx] ;
            ++counter ;
            ++idx ;        
        }

        queue->head += counter ;

        if(queue->ewaiting == true)
        {
            ++queue->encounter ;
            need_notify = true ;
        }
    }
    pthread_spin_unlock(&queue->dlocker) ;


    if(need_notify == true)
        sem_post(&queue->esema) ;

    return counter ;
}

int qkf_queue_browse(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes) 
{
    if(queue == NULL || queue->nodes == NULL || nodes == NULL || max_nodes <= 0)
        return -1 ;

    int counter = 0 ;
    pthread_spin_lock(&queue->dlocker) ;

    size_t count = queue->tail - queue->head ;
    size_t idx = queue->head % queue->capacity ;
    while((counter < max_nodes) && (counter < (int)count))
    {
        if(idx > queue->capacity)
            idx = 0 ;

        nodes[counter] = queue->nodes[idx] ;
        ++counter ;
        ++idx ;
    }

    pthread_spin_unlock(&queue->dlocker) ;

    return counter ;
}

size_t qkf_queue_size(qkf_queue_t * queue) 
{
    if(queue == NULL)
        return 0 ;
    else
        return (queue->tail - queue->head) ;
}

size_t qkf_queue_capacity(qkf_queue_t * queue) 
{
    if(queue == NULL)
        return 0 ;
    else
        return queue->capacity ;
}

bool qkf_queue_full(qkf_queue_t * queue) 
{
    if(queue == NULL)
        return true ;
    else
        return ((queue->tail - queue->head) >= queue->capacity) ;
}

bool qkf_queue_empty(qkf_queue_t * queue) 
{
    if(queue == NULL)
        return true ;
    else
        return (queue->head == queue->tail) ;
}

bool qkf_queue_extend(qkf_queue_t * queue , size_t capacity) 
{
    if(queue == NULL)
        return false ;

    pthread_mutex_lock(&queue->guard) ;
    if(queue->capacity >= capacity)
    {
        pthread_mutex_unlock(&queue->guard) ;
        return true ;    
    }

    size_t nsize = sizeof(qkf_queue_node_t *) * capacity ;
    qkf_queue_node_t ** nodes = (qkf_queue_node_t **)::qkf_malloc(kDefaultMMgr , nsize) ;
    if(nodes == NULL)
    {
        pthread_mutex_unlock(&queue->guard) ;
        return false ;    
    }
    ::memset(nodes , 0 , nsize) ;

    if(queue->nodes != NULL)
    {
        size_t osize = sizeof(qkf_queue_node_t *) * queue->capacity ;
        if(osize > 0)
            ::memcpy(nodes , queue->nodes , osize) ;
    }

    qkf_queue_node_t ** tmp = queue->nodes ;

    pthread_spin_lock(&queue->elocker) ;
    pthread_spin_lock(&queue->dlocker) ;

    queue->nodes = nodes ;
    queue->capacity = capacity ;

    pthread_spin_unlock(&queue->dlocker) ;
    pthread_spin_unlock(&queue->elocker) ;

    pthread_mutex_unlock(&queue->guard) ;

    if(tmp != NULL)
        ::qkf_free(kDefaultMMgr , tmp) ;

    return true ;
}


