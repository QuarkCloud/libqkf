
#include "qkf/tuple.h"
#include "qkf/malloc.h"
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
