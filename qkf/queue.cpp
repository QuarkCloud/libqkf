
#include "qkf/queue.h"


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
    //
}
void qkf_queue_clear(qkf_queue_t * queue) 
{
    //
}

bool qkf_queue_push(qkf_queue_t * queue , qkf_queue_node_t * node , int timeout) 
{
    return false ;
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

