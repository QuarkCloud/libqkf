
#ifndef __QKF_NOTIFY_H
#define __QKF_NOTIFY_H 1

#include "qkf/compile.h"
#include "qkf/rlist.h"
#include "qkf/rbtree.h"

#include <stdint.h>

__BEGIN_DECLS

typedef struct __st_qkf_notify_node qkf_notify_node_t ;
typedef struct __st_qkf_notifier qkf_notifier_t ;
typedef struct __st_qkf_watcher qkf_watcher_t ;


struct __st_qkf_notify_node{
    rlist_t notifier ;          //事件源链表
    rlist_t watcher ;           //观察者链表
} ;

struct __st_qkf_notifier{
    rlist_t link ;
    uint32_t id ;
    rlist_t watchers ;
} ;



__END_DECLS

#endif  /** __QKF_NOTIFY_H */
