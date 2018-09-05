
#include "qkf/notify.h"
#include <stddef.h>

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

bool qkf_watcher_init(qkf_watcher_t * watcher)
{
    if(watcher == NULL)
        return false ;
    rlist_init(&watcher->link) ;
    rlist_init(&watcher->readies) ;
    rlist_init(&watcher->nodes) ;

    pthread_spin_init(&watcher->locker , 0) ;
    watcher->owner = NULL ;
    return true ;
}

void qkf_watcher_final(qkf_watcher_t * watcher)
{
    if(watcher == NULL)
        return ;

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

bool qkf_notify_manager_init(qkf_notify_manager_t * manager)
{
    if(manager == NULL)
        return false ;
    pthread_spin_init(&manager->nwlocker , 0) ;
    rlist_init(&manager->notifiers) ;
    rlist_init(&manager->watchers) ;

    pthread_spin_init(&manager->rlocker , 0) ;
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

    pthread_spin_lock(&manager->rlocker) ;
    rlist_init(&manager->readies) ;
    pthread_spin_unlock(&manager->rlocker) ;

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

    pthread_spin_lock(&manager->rlocker) ;

    if(rlist_empty(&watcher->ready) == true)
        rlist_add_tail(&manager->readies , &watcher->ready) ;

    pthread_spin_unlock(&manager->rlocker) ;    
}

void qkf_notify_unready_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    if(manager == NULL || watcher == NULL || watcher->owner != manager)
        return ;

    pthread_spin_lock(&manager->rlocker) ;

    if(rlist_empty(&watcher->ready) == false)
        rlist_del(&manager->readies , &watcher->ready) ;

    pthread_spin_unlock(&manager->rlocker) ;    
}

