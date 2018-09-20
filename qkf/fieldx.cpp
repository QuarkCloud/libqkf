
#include "qkf/fieldx.h"
#include "qkf/malloc.h"
#include <string.h>

qkf_field_list_t * qkf_field_list_new(uint8_t type , int capacity)
{
    size_t lsize = sizeof(qkf_field_list_t) ;
    qkf_field_list_t * list = (qkf_field_list_t *)::qkf_malloc(kDefaultMMgr , lsize) ;
    if(list == NULL)
        return false ;

    if(qkf_field_list_init(list , type , capacity) == true)
        return list ;

    ::qkf_free(kDefaultMMgr , list) ;
    return false ;
}

bool qkf_field_list_init(qkf_field_list_t * list , uint8_t type , int capacity)
{
    if(list == NULL || capacity <= 0)
        return false;

    ::memset(list , 0 , sizeof(qkf_field_list_t)) ;

    size_t data_size = sizeof(qkf_field_data_t) * capacity ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , data_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , data_size) ;

    qkf_field_data_t * old_datas = list->datas ;
    list->datas = datas ;
    list->capacity = capacity ;
    list->type = type ;
    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

void qkf_field_list_final(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    qkf_field_data_t * datas = list->datas ;
    ::memset(list , 0 , sizeof(qkf_field_list_t)) ;
    if(datas != NULL)
        ::qkf_free(kDefaultMMgr , datas) ;
}

void qkf_field_list_free(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    qkf_field_list_final(list) ;
    ::qkf_free(kDefaultMMgr , list) ;
}

void qkf_field_list_clear(qkf_field_list_t * list)
{
    if(list == NULL)
        return ;

    list->size = 0 ;
    list->head = 0 ;
    list->tail = 0 ;
}

bool qkf_field_list_push(qkf_field_list_t * list , qkf_field_data_t * data)
{
    if(list == NULL || data == NULL)
        return false ;
    if(list->size >= list->capacity)
        return false ;

    uint32_t tail = list->tail + 1;
    if(tail >= list->capacity)
        tail = 0 ;

    qkf_field_data_t * node = list->datas + tail ;
    node->val = data->val ;
    list->tail = tail ;
    return true ;
}

bool qkf_field_list_pop(qkf_field_list_t * list , qkf_field_data_t & data)
{
    if(list == NULL)
        return false ;

    if(list->size <= 0)
        return false ;

    uint32_t head = list->head + 1;
    if(head >= list->capacity)
        head = 0 ;

    qkf_field_data_t * node = list->datas + head ;
    data.val = node->val ;
    list->head = head ;
    return true ;
}

bool qkf_field_list_extend(qkf_field_list_t * list)
{
    if(list == NULL)
        return false ;

    return false ;
}

bool qkf_field_list_shrink(qkf_field_list_t * list)
{
    return false ;
}
