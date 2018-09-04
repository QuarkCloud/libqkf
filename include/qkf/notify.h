
#ifndef __QKF_NOTIFY_H
#define __QKF_NOTIFY_H 1

#include "qkf/compile.h"
#include "qkf/rlist.h"

#include <stdint.h>
#include <pthread.h>

__BEGIN_DECLS

/**
    ��epoll�У���Ҫ������¼��Ƚ��٣����¼��б�����У����ʹ�ü򵥵�����͹��ˡ�
    ���ڶ��¼�Դ������ߵĳ����У����޷����ǣ��������ֳ���Ҳ����������
*/
typedef struct __st_qkf_notify_node qkf_notify_node_t ;
typedef struct __st_qkf_notifier qkf_notifier_t ;
typedef struct __st_qkf_watcher qkf_watcher_t ;
typedef struct __st_qkf_notify_manager qkf_notify_manager_t ;

struct __st_qkf_notify_node{
    rlist_t             nlink ;          //�¼�Դ����
    rlist_t             wlink ;          //�۲�������
    rlist_t             ready ;          //���ýڵ�ؽڵ�watcher��ready����

    qkf_notifier_t*     notifier ;
    qkf_watcher_t*      watcher ; 
} ;

struct __st_qkf_notifier{
    rlist_t             link ;
    
    pthread_spinlock_t  locker ;
    rlist_t             nodes ;

    qkf_notify_manager_t * owner ;
} ;

struct __st_qkf_watcher{
    rlist_t             link ;

    pthread_spinlock_t  locker ;
    rlist_t             ready ;
    rlist_t             nodes ;

    qkf_notify_manager_t * owner ;
} ;

struct __st_qkf_notify_manager{
    rlist_t             notifiers ;

    pthread_spinlock_t  locker ;
    rlist_t             watchers ;
    rlist_t             readies ;       //�����ҽ�׼���õ�watcher
} ;

QKFAPI bool qkf_notify_node_init(qkf_notify_node_t * node) ;
QKFAPI void qkf_notify_node_final(qkf_notify_node_t * node) ;


QKFAPI bool qkf_notifier_init(qkf_notifier_t * notifier) ;
QKFAPI void qkf_notifier_final(qkf_notifier_t * notifier) ;

QKFAPI bool qkf_watcher_init(qkf_watcher_t * watcher) ;
QKFAPI void qkf_watcher_final(qkf_watcher_t * watcher) ;

QKFAPI bool qkf_notify_manager_init(qkf_notify_manager_t * manager) ;
QKFAPI void qkf_notify_manager_final(qkf_notify_manager_t * manager) ;

QKFAPI bool qkf_notify_add_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier) ;
QKFAPI void qkf_notify_del_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier) ;

QKFAPI bool qkf_notify_add_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;
QKFAPI void qkf_notify_del_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;


__END_DECLS

#endif  /** __QKF_NOTIFY_H */
