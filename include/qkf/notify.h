
#ifndef __QKF_NOTIFY_H
#define __QKF_NOTIFY_H 1

#include "qkf/compile.h"
#include "qkf/rlist.h"
#include "qkf/rbtree.h"

#include <stdint.h>
#include <pthread.h>

__BEGIN_DECLS

/**
    在epoll中，需要管理的事件比较少，在事件列表管理中，因此使用简单的掩码就够了。
    但在多事件源多监听者的场景中，就无法覆盖，而且这种场景也不在少数。
*/
typedef struct __st_qkf_notify_node qkf_notify_node_t ;
typedef struct __st_qkf_notifier qkf_notifier_t ;
typedef struct __st_qkf_watcher qkf_watcher_t ;
typedef struct __st_qkf_notify_manager qkf_notify_manager_t ;
typedef struct __st_qkf_notify_allocator qkf_notify_allocator_t ;

struct __st_qkf_notify_node{
    rlist_t         notifier_link ;          //事件源链表
    qkf_notifier_t *notifier ;
    rlist_t         watcher_link ;           //观察者链表
    qkf_watcher_t * watcher ;
} ;

struct __st_qkf_notify_allocator{
    void * root ;
    qkf_notify_node_t * alloc() ;
    void free(qkf_notify_node_t * node) ;
} ;

struct __st_qkf_notifier{
    rlist_t     link ;
    uint32_t    id ;

    pthread_spinlock_t watcher_locker ;
    rlist_t     watchers ;
    rb_tree_t   watcher_map ;

    qkf_notify_allocator_t * allocator ;        //节点分配
} ;

struct __st_qkf_watcher{
    rlist_t     link ;
    uint32_t    id ;
    rlist_t     notifiers ;
} ;

struct __st_qkf_notify_manager{
    rlist_t     notifier_list ;
    rb_tree_t   notifier_map ;

    rlist_t     watcher_list ;
    rb_tree_t   watcher_map ;
} ;

QKFAPI void qkf_notify_node_init(qkf_notify_node_t * node) ;
QKFAPI void qkf_notify_node_final(qkf_notify_node_t * node) ;

QKFAPI void qkf_notify_allocator_init(qkf_notify_allocator_t * allocator) ;
QKFAPI void qkf_notify_allocator_final(qkf_notify_allocator_t * allocator) ;
QKFAPI qkf_notify_node_t * qkf_notify_alloc(qkf_notify_allocator_t * allocator) ;
QKFAPI void qkf_notify_free(qkf_notify_allocator_t * allocator , qkf_notify_node_t * node) ;

QKFAPI void qkf_notifier_init(qkf_notifier_t * notifier) ;
QKFAPI void qkf_notifier_final(qkf_notifier_t * notifier) ;
QKFAPI bool qkf_notifier_add_watcher(qkf_notifier_t * notifier , qkf_watcher_t * watcher) ;
QKFAPI void qkf_notifier_del_watcher(qkf_notifier_t * notifier , qkf_watcher_t * watcher) ;

__END_DECLS

#endif  /** __QKF_NOTIFY_H */
