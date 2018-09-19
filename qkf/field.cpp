
#include "qkf/field.h"
#include "qkf/malloc.h"
#include <string.h>

qkf_field_def_t * qkf_field_def_new(const char * name , uint8_t type)
{
    size_t fsize = sizeof(qkf_field_def_t) ;
    qkf_field_def_t * def = (qkf_field_def_t *)::qkf_malloc(kDefaultMMgr , fsize);
    if(def == NULL)
        return NULL ;

    if(qkf_field_def_init(def , name , type) == false)
    {
        ::qkf_free(kDefaultMMgr , def) ;
        return NULL ;
    }

    return def ;
}

void qkf_field_def_free(qkf_field_def_t * def)
{
    if(def == NULL)
        return ;

    qkf_field_def_final(def) ;
    ::qkf_free(kDefaultMMgr , def) ;
}

bool qkf_field_def_init(qkf_field_def_t * def , const char * name , uint8_t type)
{
    if(def == NULL || name == NULL || type < kTypeMIN || type > kTypeMAX)
        return false ;
    ::memset(def , 0 , sizeof(qkf_field_def_t)) ;

    size_t nsize = ::strlen(name) ;
    if(nsize == 0)
        return false ;

    def->name = ::qkf_strdup(name , ::strlen(name)) ;
    def->type = type ;
    return (def->name != NULL) ;
}

void qkf_field_def_final(qkf_field_def_t * def)
{
    if(def == NULL || def->name == NULL)
        return ;

    ::qkf_free(kDefaultMMgr , def->name) ;
    def->name = NULL ;
}

