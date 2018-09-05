
#ifndef __QKF_NOTIFY_H
#define __QKF_NOTIFY_H 1

#include "qkf/compile.h"
#include "qkf/rlist.h"
#include "qkf/rbtree.h"

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
    rlist_t             ready ;

    pthread_spinlock_t  locker ;
    rlist_t             readies ;
    rlist_t             nodes ;

    qkf_notify_manager_t * owner ;
} ;

struct __st_qkf_notify_manager{
    pthread_spinlock_t  nwlocker ;
    rlist_t             notifiers ;
    rlist_t             watchers ;

    pthread_spinlock_t  rlocker ;
    rlist_t             readies ;       //�����ҽ�׼���õ�watcher
} ;

QKFAPI void * qkf_notify_alloc(size_t size) ;
QKFAPI void qkf_notify_free(void * addr) ;

QKFAPI bool qkf_notify_node_init(qkf_notify_node_t * node) ;
QKFAPI void qkf_notify_node_final(qkf_notify_node_t * node) ;

QKFAPI bool qkf_notifier_init(qkf_notifier_t * notifier) ;
QKFAPI void qkf_notifier_final(qkf_notifier_t * notifier) ;
QKFAPI bool qkf_notifier_link(qkf_notifier_t * notifier , qkf_notify_node_t * node) ;
QKFAPI bool qkf_notifier_unlink(qkf_notifier_t * notifier , qkf_notify_node_t * node) ;

QKFAPI bool qkf_watcher_init(qkf_watcher_t * watcher) ;
QKFAPI void qkf_watcher_final(qkf_watcher_t * watcher) ;
QKFAPI bool qkf_watcher_link(qkf_watcher_t * watcher , qkf_notify_node_t * node) ;
QKFAPI bool qkf_watcher_unlink(qkf_watcher_t * watcher , qkf_notify_node_t * node) ;

QKFAPI bool qkf_watcher_add(qkf_watcher_t * watcher , qkf_notifier_t * notifier) ;
QKFAPI bool qkf_watcher_del(qkf_watcher_t * watcher , qkf_notifier_t * notifier) ;

QKFAPI bool qkf_notify_manager_init(qkf_notify_manager_t * manager) ;
QKFAPI void qkf_notify_manager_final(qkf_notify_manager_t * manager) ;

QKFAPI bool qkf_notify_add_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier) ;
QKFAPI void qkf_notify_del_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier) ;

QKFAPI bool qkf_notify_add_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;
QKFAPI void qkf_notify_del_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;

QKFAPI void qkf_notify_ready_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;
QKFAPI void qkf_notify_unready_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher) ;

QKFAPI int qkf_notify_wait(qkf_notify_manager_t * manager , qkf_watcher_t ** watchers , int count , int timeout) ;


__END_DECLS

#endif  /** __QKF_NOTIFY_H */
