
#include "qkf/notify.h"
#include <stddef.h>
#include <sys/time.h>
#include <string.h>

void * qkf_notify_alloc(size_t size)
{
    return ::malloc(size) ;
}

void qkf_notify_free(void * addr)
{
    ::free(addr) ;
}

bool qkf_notify_node_init(qkf_notify_node_t * node)
{
    if(node == NULL)
        return false ;
    rlist_init(&node->nlink) ;
    rlist_init(&node->wlink) ;
    rlist_init(&node->ready) ;

    node->notifier = NULL ;
    node->watcher = NULL ;
    return true ;
}

void qkf_notify_node_final(qkf_notify_node_t * node)
{
    qkf_notify_node_init(node) ;
}

bool qkf_notifier_init(qkf_notifier_t * notifier)
{
    if(notifier == NULL)
        return false ;

    rlist_init(&notifier->link) ;
    pthread_spin_init(&notifier->locker , 0) ;
    rlist_init(&notifier->nodes) ;
    notifier->owner = NULL ;
    return true ;    
}

void qkf_notifier_final(qkf_notifier_t * notifier)
{
    if(notifier == NULL)
        return ;

    while(true)
    {
        qkf_notify_node_t * node = NULL ;        
        pthread_spin_lock(&notifier->locker) ;
        rlist_t * next = NULL;
        if(rlist_empty(&notifier->nodes) == false)
        {
            next = notifier->nodes.next ;
            rlist_del(&notifier->nodes , next) ;
            node = container_of(next , qkf_notify_node_t , nlink) ;
            node->notifier = NULL ;
        }
        pthread_spin_unlock(&notifier->locker) ;

        if(node == NULL)
            break ;
        
        qkf_watcher_unlink(node->watcher , node) ;
        qkf_notify_node_final(node) ;

        ::qkf_notify_free(node) ;
    }
}

bool qkf_notifier_link(qkf_notifier_t * notifier , qkf_notify_node_t * node)
{
    if(notifier == NULL || node == NULL || (node->notifier != NULL && node->notifier != notifier))
        return false ;

    pthread_spin_lock(&notifier->locker) ;
    if(rlist_empty(&node->nlink) == true)
    {
        node->notifier = notifier ;
        rlist_add_tail(&notifier->nodes , &node->nlink) ;
    }
    pthread_spin_unlock(&notifier->locker) ;

    return true ;
}

bool qkf_notifier_unlink(qkf_notifier_t * notifier , qkf_notify_node_t * node)
{
    if(notifier == NULL || node == NULL || (node->notifier != NULL && node->notifier != notifier))
        return false ;

    pthread_spin_lock(&notifier->locker) ;
    if(rlist_empty(&node->nlink) == false)
        rlist_del(&notifier->nodes , &node->nlink) ;

    node->notifier = NULL ;
    pthread_spin_unlock(&notifier->locker) ;

    return true ;
}

typedef struct __st_qkf_watcher_map_node{
    rb_node_t link ;
    qkf_notifier_t * key ;
    qkf_notify_node_t * value ;
} qkf_watcher_map_node_t ;

int qkf_watcher_map_compare(const rb_node_t * src , const rb_node_t * dst)
{
    if(src == NULL)
        return (dst == NULL)?0:-1 ;
    else if(dst == NULL)
        return 1 ;

    const qkf_watcher_map_node_t * snode = (const qkf_watcher_map_node_t *)src ;
    const qkf_watcher_map_node_t * dnode = (const qkf_watcher_map_node_t *)dst ;
    return ((intptr_t)snode->key - (intptr_t)dnode->key) ;
}

bool qkf_watcher_init(qkf_watcher_t * watcher)
{
    if(watcher == NULL)
        return false ;
    rlist_init(&watcher->link) ;
    rlist_init(&watcher->readies) ;
    rlist_init(&watcher->nodes) ;

    pthread_spin_init(&watcher->locker , 0) ;
    pthread_mutex_init(&watcher->map_guard , NULL) ;
    watcher->nodes_map.root = NULL ;
    watcher->nodes_map.key_compare = qkf_watcher_map_compare ;

    watcher->owner = NULL ;
    return true ;
}

void qkf_watcher_final(qkf_watcher_t * watcher)
{
    if(watcher == NULL)
        return ;

    //1、先删除映射列表
    pthread_mutex_lock(&watcher->map_guard) ;
    rb_node_t * cur = NULL;
    cur = rb_first(&watcher->nodes_map) ;
    while(cur != NULL)
    {
        rb_node_t * next = rb_next(cur) ;
        qkf_notify_free(cur) ;
        cur = next ;        
    }
    pthread_mutex_unlock(&watcher->map_guard) ;

    //2、在删除关联节点列表
    while(true)
    {
        qkf_notify_node_t * node = NULL ;        
        pthread_spin_lock(&watcher->locker) ;
        rlist_t * next = NULL;
        if(rlist_empty(&watcher->nodes) == false)
        {
            next = watcher->nodes.next ;
            rlist_del(&watcher->nodes , next) ;
            node = container_of(next , qkf_notify_node_t , wlink) ;
            node->watcher = NULL ;

            //watcher中的ready列表无足轻重，其本身同时也指向link节点
        }
        pthread_spin_unlock(&watcher->locker) ;

        if(node == NULL)
            break ;
        
        qkf_notifier_unlink(node->notifier , node) ;
        //从manager的ready列表中删除
        qkf_notify_manager_t * manager = watcher->owner ;
        if(manager != NULL)
            qkf_notify_unready_watcher(manager , watcher) ;

        qkf_notify_node_final(node) ;
        ::qkf_notify_free(node) ;
    }
}

bool qkf_watcher_link(qkf_watcher_t * watcher , qkf_notify_node_t * node)
{
    if(watcher == NULL || node == NULL || (node->watcher != NULL && node->watcher != watcher))
        return false ;

    pthread_spin_lock(&watcher->locker) ;
    if(rlist_empty(&node->wlink) == true)
    {
        node->watcher = watcher ;
        rlist_add_tail(&watcher->nodes , &node->wlink) ;
    }
    pthread_spin_unlock(&watcher->locker) ;

    return true ; 
}

bool qkf_watcher_unlink(qkf_watcher_t * watcher , qkf_notify_node_t * node)
{
    if(watcher == NULL || node == NULL || (node->watcher != NULL && node->watcher != watcher))
        return false ;

    pthread_spin_lock(&watcher->locker) ;
    if(rlist_empty(&node->wlink) == false)
        rlist_del(&watcher->nodes , &node->wlink) ;

    node->watcher = NULL ;
    pthread_spin_unlock(&watcher->locker) ;

    return true ;
}

bool qkf_watcher_add(qkf_watcher_t * watcher , qkf_notifier_t * notifier)
{
    if(watcher == NULL || notifier == NULL)
        return false ;

    qkf_notify_node_t * node = (qkf_notify_node_t *)::qkf_notify_alloc(sizeof(qkf_notify_node_t)) ;
    if(node == NULL)
        return false ;

    qkf_notify_node_init(node) ;
    node->notifier = notifier ;
    node->watcher = watcher ;

    qkf_watcher_map_node_t * map = (qkf_watcher_map_node_t *)::qkf_notify_alloc(sizeof(qkf_watcher_map_node_t)) ;
    ::memset(map , 0 , sizeof(qkf_watcher_map_node_t)) ;
    map->key = notifier ;
    map->value = node ;

    pthread_mutex_lock(&watcher->map_guard) ;
    if(rb_insert(&watcher->nodes_map , &map->link) == false)
    {
        qkf_notify_free(map) ;
        qkf_notify_free(node) ;
        pthread_mutex_unlock(&watcher->map_guard) ;
        return false ;
    }

    pthread_mutex_unlock(&watcher->map_guard) ;

    qkf_notifier_link(notifier , node) ;
    qkf_watcher_link(watcher , node) ;

    return true ;
}

bool qkf_watcher_del(qkf_watcher_t * watcher , qkf_notifier_t * notifier)
{
    if(watcher == NULL || notifier == NULL)
        return false ;

    qkf_watcher_map_node_t tmp , * found = NULL;
    tmp.key = notifier ;
    
    pthread_mutex_lock(&watcher->map_guard) ;

    found = (qkf_watcher_map_node_t *)rb_find(&watcher->nodes_map , &tmp.link) ;
    if(found != NULL)
        rb_erase(&watcher->nodes_map , &found->link) ;    

    pthread_mutex_unlock(&watcher->map_guard) ;

    if(found == NULL)
        return false ;

    qkf_notify_node_t * node = found->value ;
    ::qkf_notify_free(found) ;


    if(node->notifier != notifier || node->watcher != watcher)
        return false ;

    qkf_notifier_unlink(notifier , node) ;
    qkf_watcher_unlink(watcher , node) ;

    ::qkf_notify_free(node) ;
    return true ;
}

int qkf_watcher_readies(qkf_watcher_t * watcher , qkf_notifier_t ** notifiers , int max_notifiers)
{
    if(watcher == NULL || notifiers == NULL || max_notifiers <= 0)
        return -1 ;

    int count = 0 ;
    pthread_spin_lock(&watcher->locker) ;
    while(rlist_empty(&watcher->readies) == false && count < max_notifiers)
    {
        qkf_notify_node_t * node = container_of(watcher->readies.next , qkf_notify_node_t , ready) ;
        rlist_del(&watcher->readies , &node->ready) ;
        notifiers[count] = node->notifier ;
        ++count ;
    }
    pthread_spin_unlock(&watcher->locker) ;
    return count ;
}

bool qkf_notify_manager_init(qkf_notify_manager_t * manager)
{
    if(manager == NULL)
        return false ;
    pthread_spin_init(&manager->nwlocker , 0) ;
    rlist_init(&manager->notifiers) ;
    rlist_init(&manager->watchers) ;

    pthread_mutex_init(&manager->rmutex , NULL) ;
    pthread_cond_init(&manager->rcond , NULL) ;
    rlist_init(&manager->readies) ;
    return true ;
}

void qkf_notify_manager_final(qkf_notify_manager_t * manager)
{
    if(manager == NULL)
        return ;

    rlist_t link ;
    rlist_init(&link) ;
    pthread_spin_lock(&manager->nwlocker) ;
    if(rlist_empty(&manager->notifiers) == false)
    {
        rlist_t * notifiers = &manager->notifiers ;
        link.next = notifiers->next ;
        notifiers->next->prev = &link ;
        
        link.prev = notifiers->prev ;
        notifiers->prev->next = &link ;
    }
    pthread_spin_unlock(&manager->nwlocker) ;

    while(rlist_empty(&link) == false)
    {
        qkf_notifier_t * notifier = (qkf_notifier_t *)link.next ;
        notifier->owner = NULL ;
        qkf_notifier_final(notifier) ;
        rlist_del(&link , &notifier->link) ;
    }

    pthread_spin_lock(&manager->nwlocker) ;
    if(rlist_empty(&manager->notifiers) == false)
    {
        rlist_t * watchers = &manager->watchers ;
        link.next = watchers->next ;
        watchers->next->prev = &link ;
        
        link.prev = watchers->prev ;
        watchers->prev->next = &link ;
    }
    pthread_spin_unlock(&manager->nwlocker) ;

    while(rlist_empty(&link) == false)
    {
        qkf_watcher_t * watcher = (qkf_watcher_t *)link.next ;
        watcher->owner = NULL ;
        qkf_watcher_final(watcher) ;
        rlist_del(&link , &watcher->link) ;
    }

    pthread_mutex_lock(&manager->rmutex) ;
    rlist_init(&manager->readies) ;
    pthread_mutex_unlock(&manager->rmutex) ;

}

bool qkf_notify_add_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier)
{
    if(manager == NULL || notifier == NULL || (notifier->owner != NULL && notifier->owner != manager))
        return false ;

    pthread_spin_lock(&manager->nwlocker) ;
    if(notifier->owner == NULL)
        notifier->owner = manager ;
    if(rlist_empty(&notifier->link) == true)
        rlist_add_tail(&manager->notifiers , &notifier->link) ;
    pthread_spin_unlock(&manager->nwlocker) ;
    return true ;
}

void qkf_notify_del_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier)
{
    if(manager == NULL || notifier == NULL || (notifier->owner != NULL && notifier->owner != manager))
        return ;

    pthread_spin_lock(&manager->nwlocker) ;
    if(notifier->owner != NULL)
        notifier->owner = NULL ;
    if(rlist_empty(&notifier->link) == false)
        rlist_del(&manager->notifiers , &notifier->link) ;
    pthread_spin_unlock(&manager->nwlocker) ;
}

bool qkf_notify_add_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    if(manager == NULL || watcher == NULL || (watcher->owner != NULL && watcher->owner != manager))
        return false ;

    pthread_spin_lock(&manager->nwlocker) ;
    if(watcher->owner == NULL)
        watcher->owner = manager ;
    if(rlist_empty(&watcher->link) == true)
        rlist_add_tail(&manager->watchers , &watcher->link) ;
    pthread_spin_unlock(&manager->nwlocker) ;

    return true ;
}

void qkf_notify_del_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    if(manager == NULL || watcher == NULL || (watcher->owner != NULL && watcher->owner != manager))
        return ;

    pthread_spin_lock(&manager->nwlocker) ;
    if(watcher->owner != NULL)
        watcher->owner = NULL ;
    if(rlist_empty(&watcher->link) == false)
        rlist_del(&manager->watchers , &watcher->link) ;
    pthread_spin_unlock(&manager->nwlocker) ;
}

void qkf_notify_ready_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    if(manager == NULL || watcher == NULL || watcher->owner != manager)
        return ;

    pthread_mutex_lock(&manager->rmutex) ;

    if(rlist_empty(&watcher->ready) == true)
    {
        rlist_add_tail(&manager->readies , &watcher->ready) ;
        pthread_cond_signal(&manager->rcond) ;
    }

    pthread_mutex_unlock(&manager->rmutex) ;    
}

void qkf_notify_unready_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    if(manager == NULL || watcher == NULL || watcher->owner != manager)
        return ;

    pthread_mutex_lock(&manager->rmutex) ;

    if(rlist_empty(&watcher->ready) == false)
        rlist_del(&manager->readies , &watcher->ready) ;

    pthread_mutex_unlock(&manager->rmutex) ;    
}

static const int64_t kNotifyWaitMinTime = 15000 ;       //最小等待时间

int qkf_notify_wait(qkf_notify_manager_t * manager , qkf_watcher_t ** watchers , int max_watchers , int timeout)
{
    if(manager == NULL || watchers == NULL || max_watchers <= 0)
        return -1 ;
    if(timeout < 0)
        timeout = 3600000 ;

    int wcount  = 0 ;
    struct timeval start ;
    ::gettimeofday(&start , NULL) ;
    while(true)
    {
        pthread_mutex_lock(&manager->rmutex) ;
        while(rlist_empty(&manager->readies) == false && wcount < max_watchers)
        {
            rlist_t * next = manager->readies.next ;
            rlist_del(&manager->readies , next) ;

            qkf_watcher_t * watcher = container_of(next , qkf_watcher_t , ready) ;
            watchers[wcount] = watcher ;
            ++wcount ;
        }
        if(wcount > 0 || timeout == 0)
        {
            pthread_mutex_unlock(&manager->rmutex) ;
            break ;
        }
        struct timeval now ;
        ::gettimeofday(&now , NULL) ;
        int64_t usec = (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec) ;
        int64_t gap = (timeout * 1000) - usec ;
        if(gap > kNotifyWaitMinTime)
        {
            struct timespec ts ;
            ts.tv_sec = 0 ;
            ts.tv_nsec = kNotifyWaitMinTime * 1000 ;
            pthread_cond_timedwait(&manager->rcond , &manager->rmutex , &ts) ;
        }
        pthread_mutex_unlock(&manager->rmutex) ;
    }

    return wcount ;
}

