
#include "qkf/tuple.h"
#include "qkf/malloc.h"
#include "qkf/tytils.h"
#include <string.h>

qkf_tuple_header_t * qkf_tuple_header_new()
{
    size_t header_size = sizeof(qkf_tuple_header_t) ;
    qkf_tuple_header_t * header = (qkf_tuple_header_t *)::qkf_malloc(kDefaultMMgr , header_size) ;
    if(header == NULL)
        return NULL ;
    if(qkf_tuple_header_init(header) == false)
    {
        ::qkf_free(kDefaultMMgr , header) ;
        return NULL ;
    }

    return header ;
}

void qkf_tuple_header_free(qkf_tuple_header_t * header)
{
    if(header == NULL)
        return ;
    qkf_tuple_header_final(header) ;
    ::qkf_free(kDefaultMMgr , header) ;
}

int qkf_tuple_header_node_compare(const rb_node_t * s1 , const rb_node_t * s2)
{
    if(s1 == NULL || s2 == NULL)
        return 0 ;
    const qkf_tuple_header_node_t * n1 = (const qkf_tuple_header_node_t *)s1 ;
    const qkf_tuple_header_node_t * n2 = (const qkf_tuple_header_node_t *)s2 ;

    if(n1->def.name == NULL || n2->def.name == NULL)
        return 0 ;

    return ::strcmp(n1->def.name , n2->def.name) ;
}

bool qkf_tuple_header_init(qkf_tuple_header_t * header)
{
    if(header == NULL)
        return false ;
    header->names.root = NULL ;
    header->names.key_compare = qkf_tuple_header_node_compare ;
    return qkf_vector_init(&header->nodes , sizeof(qkf_tuple_header_node_t) , 8) ;
}

void qkf_tuple_header_final(qkf_tuple_header_t * header)
{
    if(header == NULL)
        return ;

    int size = qkf_vector_size(&header->nodes) ;
    for(int fidx = 0 ; fidx < size ; ++fidx)
    {
        qkf_tuple_header_node_t * node = (qkf_tuple_header_node_t *)::qkf_vector_get(&header->nodes , fidx) ;
        if(node == NULL)
            continue ;

        if(node->def.name != NULL)
        {
            ::qkf_free(kDefaultMMgr , node->def.name) ;
            node->def.name = NULL ;
        }    
    }
}

int qkf_tuple_header_add(qkf_tuple_header_t * header , const char * name , uint8_t type)
{
    if(header == NULL || name == NULL || type <= kTypeMIN || type > kTypeMAX)
        return -1 ;
    qkf_tuple_header_node_t node ;
    ::memset(&node , 0 , sizeof(qkf_tuple_header_node_t)) ;
    node.def.name = ::qkf_strdup(name , 0) ;
    node.def.type = type ;
    node.def.index = -1 ;

    int index = qkf_vector_add(&header->nodes , &node) ;
    if(index < 0)
        return -1 ;

    qkf_tuple_header_node_t * f = (qkf_tuple_header_node_t *)qkf_vector_get(&header->nodes , index) ;
    f->def.index = index ;
    f->def.offset = index * sizeof(qkf_field_data_t) ;

    if(rb_insert(&header->names , &f->link) == false)
        return -1 ;
    return index ;
}

bool qkf_tuple_header_del(qkf_tuple_header_t * header , const char * name)
{
    int index = qkf_tuple_header_find(header , name) ;
    if(index < 0)
        return false ;

    qkf_tuple_header_node_t * node = (qkf_tuple_header_node_t *)qkf_vector_get(&header->nodes , index) ;
    if(node == NULL)
        return false ;

    rb_erase(&header->names , &node->link);
    if(qkf_vector_del(&header->nodes , index) == false)
        return false ;

    int size = qkf_vector_size(&header->nodes) ;
    for(int fidx = index ; fidx < size ; ++fidx)
    {
        qkf_tuple_header_node_t * n = (qkf_tuple_header_node_t *)qkf_vector_get(&header->nodes , fidx) ;
        n->def.index = fidx ;
        n->def.offset = fidx * sizeof(qkf_field_data_t) ;        
    }

    return true ;
}

int qkf_tuple_header_find(qkf_tuple_header_t * header , const char * name)
{
    if(header == NULL || name == NULL)
        return -1 ;

    qkf_tuple_header_node_t node ;
    node.def.name = (char *)name ;
    qkf_tuple_header_node_t * f = (qkf_tuple_header_node_t *)rb_find(&header->names , &node.link) ;
    if(f == NULL)
        return -1 ;
    else
        return f->def.index ;
}

bool qkf_tuple_header_get(qkf_tuple_header_t * header , int index , qkf_field_def_t*& def)
{
    qkf_tuple_header_node_t * node = (qkf_tuple_header_node_t *)::qkf_vector_get(&header->nodes , index) ;
    if(node == NULL)
        return false ;
    def = &node->def ;
    return true ;
}

int qkf_tuple_header_size(const qkf_tuple_header_t * header) 
{
    if(header == NULL)
        return -1 ;
    return qkf_vector_size(&header->nodes) ;
}

qkf_tuple_t * qkf_tuple_new(qkf_tuple_header_t * header)
{
    if(header == NULL)
        return NULL ;

    size_t tuple_size = sizeof(qkf_tuple_t) ;
    qkf_tuple_t * tuple = (qkf_tuple_t *)::qkf_malloc(kDefaultMMgr , tuple_size) ;
    if(tuple == NULL)
        return NULL ;
    ::memset(tuple , 0 , tuple_size) ;
    if(qkf_tuple_init(tuple , header) == true)
        return tuple ;
    ::qkf_free(kDefaultMMgr , tuple) ;
    return NULL ;
}

bool qkf_tuple_init(qkf_tuple_t * tuple , qkf_tuple_header_t * header)
{
    if(tuple == NULL || header == NULL)
        return false ;
    int field_count = qkf_tuple_header_size(header)  ;
    if(field_count == 0)
        return false ;

    int bit_size = (field_count >> 3) + ((field_count & 7) ? 1 : 0) ;
    size_t bytes = sizeof(qkf_tuple_body_t) - 1 + bit_size ;
    bytes += sizeof(qkf_field_data_t) * field_count ;

    qkf_tuple_body_t * body = (qkf_tuple_body_t *)::qkf_malloc(kDefaultMMgr , bytes) ;
    if(body == NULL)
        return false ;

    body->capacity = bytes ;
    body->field_count = field_count ;

    tuple->header = header ;
    tuple->body = body ;

    return true ;
}

void qkf_tuple_final(qkf_tuple_t * tuple)
{
    if(tuple == NULL)
        return ;
    qkf_tuple_clear(tuple) ;
    tuple->header = NULL ;
    tuple->body = NULL ;
}

void qkf_tuple_free(qkf_tuple_t * tuple)
{
    if(tuple == NULL)
        return ;
    qkf_tuple_final(tuple) ;
    ::qkf_free(kDefaultMMgr , tuple) ;
}

void qkf_tuple_clear(qkf_tuple_t * tuple)
{
    if(tuple == NULL || tuple->header == NULL || tuple->body == NULL)
        return ;

    qkf_tuple_header_t * header = tuple->header ;
    qkf_tuple_body_t * body = tuple->body ;

    uint8_t * bits = (uint8_t *)(&body->bits) ;

    uint32_t capacity = body->capacity ;
    uint32_t field_count = body->field_count ;
    int bit_size = (field_count >> 6) + (field_count & 63)?1:0 ;
    qkf_field_data_t * datas = (qkf_field_data_t *)(&body->bits + bit_size) ;

    for(uint32_t fidx = 0 ; fidx < field_count ; ++fidx)
    {
        int bytes = fidx >> 3 ;
        int mask = fidx & 7 ;

        if(::qkf_get_bitmap(bits[bytes] , mask) == false)
            continue ;

        qkf_field_def_t * def = NULL ;
        if(::qkf_tuple_header_get(header , fidx , def) == false || def == NULL)
            continue ;

        uint8_t type = def->type ;
        if(type == kTypeSTR || type == kTypeRAW || type == kTypeLIST || type == kTypeMAP)
        {
            qkf_field_data_t * data = datas + fidx ;
            uint64_t val = data->val ;

            if(val < (uint64_t)body || val > (uint64_t)body + capacity)
            {
                //区间外地址
                qkf_field_data_free(type , data) ;
            }
        }        
    }    

    ::qkf_free(kDefaultMMgr , tuple->body) ;
    tuple->body = NULL ;
}

int qkf_tuple_find(qkf_tuple_t * tuple , const char * name)
{
    if(tuple == NULL || tuple->header == NULL)
        return -1 ;

    return qkf_tuple_header_find(tuple->header , name) ;
}

bool qkf_tuple_set(qkf_tuple_t * tuple , int index , uint8_t type , qkf_field_data_t * data)
{
    if(tuple == NULL || tuple->body == NULL)
        return false ;

    qkf_tuple_body_t * body = tuple->body ;
    uint32_t field_count =  body->field_count ;
    if(index < 0 || index >= (int)field_count)
        return false ;

    qkf_field_def_t * def = NULL ;
    if(qkf_tuple_header_get(tuple->header , index , def) == false || def == NULL)
        return false ;

    if(def->type != type)
        return false ;

    uint32_t bit_size = (field_count >> 6) + (field_count & 63) ? 1 : 0 ;
    qkf_field_data_t * datas = (qkf_field_data_t *)(&body->bits + bit_size) ;
    datas[index].val = data->val ;

    uint8_t * bits = (uint8_t *)&body->bits ;
    int bytes = index >> 3 ;
    int mask = index & 7 ;

    qkf_set_bitmap(bits[bytes] , mask , true) ;
    return true ;
}

bool qkf_tuple_get(qkf_tuple_t * tuple , int index , uint8_t& type , qkf_field_data_t *& data)
{
    if(tuple == NULL || tuple->body == NULL || tuple->header == NULL)
        return false ;

    qkf_tuple_body_t * body = tuple->body ;
    uint32_t field_count = body->field_count ;
    if(field_count == 0)
        return false ;

    uint8_t * bits = (uint8_t *)&body->bits ;
    int bytes = index >> 3 ;
    int mask = index & 7 ;

    if(qkf_get_bitmap(bits[bytes] , mask) == false)
        return false ;

    qkf_field_def_t * def = NULL ;
    if(qkf_tuple_header_get(tuple->header , index , def) == false)
        return false ;

    type = def->type ;

    uint32_t bit_size = (field_count >> 6) + (field_count & 63) ? 1 : 0 ;
    qkf_field_data_t * datas = (qkf_field_data_t *)(bits + bit_size) ;


    data = datas + index ;
    return true ;
}

