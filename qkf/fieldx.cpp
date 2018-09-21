
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

qkf_map_header_t * qkf_map_header_new()
{
    size_t header_size = sizeof(qkf_map_header_t) ;
    qkf_map_header_t * header = (qkf_map_header_t *)::qkf_malloc(kDefaultMMgr , header_size) ;
    if(header == NULL)
        return NULL ;
    ::memset(header , 0 , header_size) ;

    if(qkf_map_header_init(header) == false)
    {
        ::qkf_free(kDefaultMMgr , header) ;
        return NULL ;
    }

    return header ;
}

void qkf_map_header_free(qkf_map_header_t * header)
{
    if(header == NULL)
        return ;

    qkf_map_header_final(header) ;
    ::qkf_free(kDefaultMMgr , header) ;
}

typedef struct __st_qkf_map_header_node{
    rb_node_t       link ;
    const char  *   name ;
    int             index ;
} qkf_map_header_node_t;

int qkf_map_header_compare(const rb_node_t * src , const rb_node_t * dst)
{
    if(src == NULL || dst == NULL)
        return 0 ;

    const qkf_map_header_node_t * src_node = (const qkf_map_header_node_t *) src ;
    const qkf_map_header_node_t * dst_node = (const qkf_map_header_node_t *) dst ;

    if(src_node->name == NULL || dst_node->name == NULL)
        return 0 ;

    return ::strcmp(src_node->name , dst_node->name) ;
}

bool qkf_map_header_init(qkf_map_header_t * header)
{
    if(header == NULL)
        return false ;

    header->names.root = NULL ;
    header->names.key_compare = qkf_map_header_compare ;

    return qkf_vector_init(&header->defs , sizeof(qkf_field_def_t) , 8) ;
}

void qkf_map_header_final(qkf_map_header_t * header)
{
    if(header == NULL)
        return ;

    //1、释放红黑树节点
    rb_node_t * cur = rb_first(&header->names) ;
    rb_node_t * next = NULL ;
    while(cur != NULL)
    {
        next = rb_next(cur) ;
        ::qkf_free(kDefaultMMgr , cur) ;
        cur = next ;
    }
    header->names.root = NULL ;

    //2、释放字段定义中的名字
    int size = qkf_vector_size(&header->defs) ;
    for(int fidx = 0 ; fidx < size ; ++fidx)
    {
        qkf_field_def_t * def = (qkf_field_def_t *)::qkf_vector_get(&header->defs , fidx) ;
        if(def->name != NULL)
            ::qkf_free(kDefaultMMgr , def->name) ;
    }
    qkf_vector_final(&header->defs) ;
    ::qkf_free(kDefaultMMgr , header) ;
}

int qkf_map_header_add(qkf_map_header_t * header , const char * name , uint8_t type)
{
    if(header == NULL || name == NULL || (type < kTypeMIN || type > kTypeMAX))
        return -1 ;

    int index = 0 ;
    if(qkf_map_header_get_by_name(header , name , index) == true)
        return index ;

    index = qkf_vector_size(&header->defs) ;

    qkf_field_def_t def ;
    def.name = qkf_strdup(name , 0) ;
    def.type = type ;
    def.index = index ;
    def.offset = 0 ;
    if(qkf_vector_add(&header->defs , &def) == false)
    {
        ::qkf_free(kDefaultMMgr , def.name) ;
        return -1 ;
    }

    size_t nsize = sizeof(qkf_map_header_node_t) ;
    qkf_map_header_node_t * node = (qkf_map_header_node_t *)::qkf_malloc(kDefaultMMgr , nsize) ;
    if(node == NULL)
        return -1 ;
    ::memset(node , 0 , nsize) ;
    node->name = def.name ;
    node->index = index ;

    if(rb_insert(&header->names , &node->link) == true)
        return index ;
    ::qkf_free(kDefaultMMgr , node) ;
    return -1 ;
}

int qkf_map_header_size(qkf_map_header_t * header)
{
    return (header == NULL)?0:qkf_vector_size(&header->defs) ;
}

bool qkf_map_header_get_by_index(qkf_map_header_t * header , int index , const char *& name , uint8_t& type)
{
    if(header == NULL)
        return false ;

    qkf_field_def_t * def = (qkf_field_def_t *)qkf_vector_get(&header->defs , index) ;
    if(def == NULL)
        return false ;

    name = def->name ;
    type = def->type ;
    return true ;
}

bool qkf_map_header_get_by_name(qkf_map_header_t * header , const char * name , int& index)
{
    if(header == NULL || name == NULL)
        return false ;

    qkf_map_header_node_t node  ;
    ::memset(&node , 0 , sizeof(node)) ;
    node.name = name ;

    qkf_map_header_node_t * f = (qkf_map_header_node_t *)rb_find(&header->names , &node.link) ;
    if(f == NULL)
        return false ;

    index = f->index ;
    return true ;
}

