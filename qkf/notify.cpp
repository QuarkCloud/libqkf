
#include "qkf/notify.h"

bool qkf_notify_node_init(qkf_notify_node_t * node)
{
    return false ;
}

void qkf_notify_node_final(qkf_notify_node_t * node)
{
    //
}

bool qkf_notifier_init(qkf_notifier_t * notifier)
{
    return false ;
}

void qkf_notifier_final(qkf_notifier_t * notifier)
{
    //
}

bool qkf_watcher_init(qkf_watcher_t * watcher)
{
    return false ;
}

void qkf_watcher_final(qkf_watcher_t * watcher)
{
    //
}

bool qkf_notify_manager_init(qkf_notify_manager_t * manager)
{
    return false ;
}

void qkf_notify_manager_final(qkf_notify_manager_t * manager)
{
    //
}

bool qkf_notify_add_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier)
{
    return false ;
}

void qkf_notify_del_notifier(qkf_notify_manager_t * manager , qkf_notifier_t * notifier)
{
    //
}

bool qkf_notify_add_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    return false ;
}

void qkf_notify_del_watcher(qkf_notify_manager_t * manager , qkf_watcher_t * watcher)
{
    //
}

