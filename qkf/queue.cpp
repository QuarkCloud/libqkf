
#include "qkf/queue.h"
#include "qkf/time.h"

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
        ::qkf_free(nodes) ;
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

    int64_t ts = qk_now_usec() ;
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
    //memory_barrier() ;
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

        pthread_sin_lick(&queue->elocker) ;
        int64_t now = qkf_now_msec() ;
        if((int)(now - begin) >  timeout)
            break ;       
    } while(queue->finaled == false) ;

    queue->ewaiting = false ;
    if(result == true)
    {
        size_t idx = queue->tail % queue->capacity ;

    }
    


    return false ;
}

bool qkf_queue_try_pop(qkf_queue_t * queue , qkf_queue_node_t ** node) 
{

}

int qkf_queue_pop(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes , int timeout) 
{
    return 0 ;
}

int qkf_queue_browse(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes) 
{
    return 0 ;
}

size_t qkf_queue_size(qkf_queue_t * queue) 
{
    return 0 ;
}
size_t qkf_queue_capacity(qkf_queue_t * queue) 
{
    return 0 ;
}
bool qkf_queue_full(qkf_queue_t * queue) 
{
    return false ;
}
bool qkf_queue_empty(qkf_queue_t * queue) 
{
    return false ;
}

bool qkf_queue_extend(qkf_queue_t * queue , size_t capacity) 
{
    return false ;
}

