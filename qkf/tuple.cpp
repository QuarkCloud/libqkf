
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


}

bool qkf_tuple_header_init(qkf_tuple_header_t * header)
{
    return false ;
}

void qkf_tuple_header_final(qkf_tuple_header_t * header)
{

}

int qkf_tuple_header_add(qkf_tuple_header_t * header , const char * name , uint8_t type)
{
    return 0 ;
}

int qkf_tuple_header_del(qkf_tuple_header_t * header , const char * name)
{
    return 0 ;
}

int qkf_tuple_header_find(qkf_tuple_header_t * header , const char * name)
{
    return 0 ;
}

bool qkf_tuple_header_get(qkf_tuple_header_t * header , int index , qkf_field_def_t*& def)
{
    return false ;
}
