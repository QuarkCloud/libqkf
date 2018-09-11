
#ifndef __QKF_QUEUE_H
#define __QKF_QUEUE_H 1

#include "qkf/compile.h"
#include "qkf/rlist.h"

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

__BEGIN_DECLS

typedef struct __st_qkf_queue_node{
    int64_t         timestamp ;
    size_t          size ;
    char            data[4] ;
} qkf_queue_node_t;

typedef struct __st_qkf_queue{
    qkf_queue_node_t**  nodes ;
    size_t              capacity ;
    size_t              head ;
    size_t              tail ;

    size_t              encounter ;
    size_t              decounter ;

    size_t              ewtimes ;
    size_t              dwtimes ;

    pthread_spinlock_t  elocker ;
    pthread_spinlock_t  dlocker ;

    bool                finaled ;
    bool                ewaiting ;
    bool                dwaiting ;

    pthread_mutex_t     guard ;
    sem_t               esema ;
    sem_t               dsema ;
} qkf_queue_t;


QKFAPI bool qkf_queue_init(qkf_queue_t * queue , size_t capacity) ;
QKFAPI void qkf_queue_final(qkf_queue_t * queue) ;
QKFAPI void qkf_queue_clear(qkf_queue_t * queue) ;

//timeout的延迟以毫秒为精度，更高的精度基本没有什么意义。
QKFAPI bool qkf_queue_try_push(qkf_queue_t * queue , qkf_queue_node_t * node) ;
QKFAPI bool qkf_queue_push(qkf_queue_t * queue , qkf_queue_node_t * node , int timeout) ;
QKFAPI bool qkf_queue_try_pop(qkf_queue_t * queue , qkf_queue_node_t ** node) ;
QKFAPI int qkf_queue_pop(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes , int timeout) ;
QKFAPI int qkf_queue_browse(qkf_queue_t * queue , qkf_queue_node_t ** nodes , int max_nodes) ;

QKFAPI size_t qkf_queue_size(qkf_queue_t * queue) ;
QKFAPI size_t qkf_queue_capacity(qkf_queue_t * queue) ;
QKFAPI bool qkf_queue_full(qkf_queue_t * queue) ;
QKFAPI bool qkf_queue_empty(qkf_queue_t * queue) ;

QKFAPI bool qkf_queue_extend(qkf_queue_t * queue , size_t capacity) ;

__END_DECLS

#endif  /** __QKF_QUEUE_H */
