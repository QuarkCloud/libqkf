
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
    {
        if(qkf_field_list_extend(list) == false)
            return false ;
    }

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

    qkf_field_list_shrink(list) ;
    return true ;
}

bool qkf_field_list_copy(qkf_field_list_t * list , qkf_field_data_t * datas) 
{
    if(list == NULL || datas == NULL)
        return false ;
    if(list->size == 0)
        return true ;

    //由于使用环形队列，拷贝数据同一般的不一样
    uint32_t head = list->head ;
    uint32_t tail = list->tail ;

    if(head < tail)
    {
        //head和tail之间是数据
        qkf_field_data_t * from = list->datas + head ;
        size_t size  = sizeof(qkf_field_data_t) * list->size ;
        if(size > 0)
            ::memcpy(datas , from , size) ;
    }
    else if(head > tail)
    {
        //0-->tail 和 head-->capacity之间是数据，分为两段
        qkf_field_data_t * from = list->datas + head ;
        uint32_t copy_count = list->capacity - head + 1 ;
        size_t size = sizeof(qkf_field_data_t) * (copy_count) ;
        ::memcpy(datas , from , size) ;

        from = datas + copy_count ;
        if(tail > 0)
        {
            size = sizeof(qkf_field_data_t) * (tail - 1) ;
            ::memcpy(from , list->datas , size) ;
        }
    }

    return true ;
}

bool qkf_field_list_extend(qkf_field_list_t * list)
{
    if(list == NULL)
        return false ;

    uint32_t capacity = qkf_vector_extend_size(list->capacity) ;
    size_t data_size = sizeof(qkf_field_data_t) * capacity ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , data_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , data_size) ;
    qkf_field_data_t * old_datas = list->datas ;

    if(qkf_field_list_copy(list , datas) == false)
    {
        ::qkf_free(kDefaultMMgr , datas) ;
        return false ;
    }


    list->datas = datas ;
    list->head = 0 ;
    list->tail = list->size ;
    list->capacity = capacity ;
    
    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

bool qkf_field_list_shrink(qkf_field_list_t * list)
{
    if(list == NULL)
        return false ;
    if(list->datas == NULL || list->capacity == 0)
        return true ;

    uint32_t newcap = qkf_vector_shrink_size(list->capacity , list->size) ;
    if(newcap == 0)
        return false ;

    size_t shr_size = sizeof(qkf_field_data_t) * newcap ;
    qkf_field_data_t * datas = (qkf_field_data_t *)::qkf_malloc(kDefaultMMgr , shr_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , shr_size) ;

    qkf_field_data_t * old_datas = list->datas ;
    if(qkf_field_list_copy(list , datas) == false)
    {
        ::qkf_free(kDefaultMMgr , datas) ;
        return false ;
    }

    list->datas = datas ;
    list->head = 0 ;
    list->tail = list->size ;
    list->capacity = newcap ;

    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}

qkf_field_map_t * qkf_field_map_new()
{
    size_t map_size = sizeof(qkf_field_map_t) ;
    qkf_field_map_t * map = (qkf_field_map_t *)::qkf_malloc(kDefaultMMgr , map_size) ;
    if(map == NULL)
        return NULL ;
    if(qkf_field_map_init(map) == false)
    {
        ::qkf_free(kDefaultMMgr , map) ;
        return NULL ;
    }

    return map ;
}

void qkf_field_map_free(qkf_field_map_t * map)
{
    if(map == NULL)
        return ;

    qkf_field_map_final(map) ;
    ::qkf_free(kDefaultMMgr , map) ;
}

int qkf_field_map_node_compare(const rb_node_t * src , const rb_node_t * dst)
{
    if(src == NULL || dst == NULL)
        return 0 ;

    const qkf_field_map_node_t * src_node = (const qkf_field_map_node_t *)src ;
    const qkf_field_map_node_t * dst_node = (const qkf_field_map_node_t *)dst ;

    if(src_node->def.name == NULL || dst_node->def.name == NULL)
        return 0 ;

    return ::strcmp(src_node->def.name , dst_node->def.name) ;
}

bool qkf_field_map_init(qkf_field_map_t * map)
{
    if(map == NULL)
        return false ;
    map->names.root = NULL ;
    map->names.key_compare = qkf_field_map_node_compare ;

    return qkf_vector_init(&map->nodes , sizeof(qkf_field_map_node_t) , 8) ;
}

void qkf_field_map_final(qkf_field_map_t * map)
{
    if(map == NULL)
        return ;

    map->names.root = NULL ;

    //2、释放字段定义中的名字
    int size = qkf_vector_size(&map->nodes) ;
    for(int fidx = 0 ; fidx < size ; ++fidx)
    {
        qkf_field_map_node_t * node = (qkf_field_map_node_t *)::qkf_vector_get(&map->nodes , fidx) ;
        qkf_field_map_node_final(node) ;
    }
    qkf_vector_final(&map->nodes) ;
    ::qkf_free(kDefaultMMgr , map) ;
}

void qkf_field_map_node_final(qkf_field_map_node_t * node) 
{
    if(node == NULL)
        return ;

    if(node->def.name != NULL)
    {
        ::qkf_free(kDefaultMMgr , node->def.name) ;
        node->def.name = NULL ;
    }

    uint8_t type = node->def.type ;
    if(type == kTypeSTR || type == kTypeRAW)
        ::qkf_free(kDefaultMMgr , node->data.str) ;
    else if(type == kTypeLIST)
        qkf_field_list_free(node->data.list) ;
    else if(type == kTypeMAP)
        qkf_field_map_free(node->data.map) ;

    node->data.val = 0 ;
}

int qkf_field_map_add(qkf_field_map_t * map , const char * name , uint8_t type , qkf_field_data_t * data)
{
    if(map == NULL || name == NULL || type < kTypeMIN || type > kTypeMAX || data == NULL)
        return -1 ;

    //qkf_field_map_node_t * node = NULL ;
    int index = -1 ;
    if(qkf_field_map_get_by_name(map , name , index) == true)
        return -1 ;

    qkf_field_map_node_t field ;
    field.def.name = qkf_strdup(name , 0) ;
    field.def.type = type ;
    field.def.index = -1 ;
    field.data.val = data->val ;

    if((index = qkf_vector_add(&map->nodes , &field)) < 0)
    {
        ::qkf_free(kDefaultMMgr , field.def.name) ;
        return -1 ;
    }
    
    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    if(node == NULL)
        return -1 ;   
    node->def.index = index ;

    if(rb_insert(&map->names , &node->link) == true)
        return index ;
    else
        return -1 ;
}

bool qkf_field_map_del(qkf_field_map_t * map , const char * name)
{
    if(map == NULL || name == NULL)
        return false ;

    int index = -1 ;
    if(qkf_field_map_get_by_name(map , name , index) == false || index < 0)
        return false ;

    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    rb_erase(&map->names , &node->link) ;

    if(node != NULL)
        qkf_field_map_node_final(node) ;
    qkf_vector_del(&map->nodes , index) ;

    int size = qkf_vector_size(&map->nodes) ;
    for(int fidx = index ; fidx < size ; ++fidx)
    {
        qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , fidx) ;
        node->def.index = fidx ;
    }
    return true ;
}

bool qkf_field_map_update(qkf_field_map_t * map , const char * name , qkf_field_data_t * data)
{
    if(map == NULL || name == NULL || data == NULL)
        return false ;
    int index = -1 ;
    if(qkf_field_map_get_by_name(map , name , index) == false || index < 0)
        return false ;

    qkf_field_map_node_t * node = (qkf_field_map_node_t *)qkf_vector_get(&map->nodes , index) ;
    node->data.val = data->val ;
    return true ;
}

int qkf_field_map_size(qkf_field_map_t * map)
{
    return (map == NULL) ? 0 : qkf_vector_size(&map->nodes) ;
}

bool qkf_field_map_get_by_index(qkf_field_map_t * map , int index , qkf_field_map_node_t*&node)
{
    if(map == NULL)
        return false ;

    node = (qkf_field_map_node_t *)::qkf_vector_get(&map->nodes , index) ;
    return (node != NULL) ;
}

bool qkf_field_map_get_by_name(qkf_field_map_t * map , const char * name , int& index)
{
    if(map == NULL || name == NULL)
        return false ;

    qkf_field_map_node_t node ;
    node.def.name = (char *)name ;

    qkf_field_map_node_t * f = (qkf_field_map_node_t *)rb_find(&map->names , &node.link) ;
    if(f == NULL)
        return false ;

    index = f->def.index ;
    return true ;
}

