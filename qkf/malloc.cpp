
#include "qkf/malloc.h"
#include <stdlib.h>
#include <string.h>


int qkf_mmgr_create()
{
    return kDefaultMMgr ;
}

void qkf_mmrr_destory(int mmgr)
{
    //
}

void * qkf_malloc(int mmgr , size_t size)
{
    return ::malloc(size) ;
}

void qkf_free(int mmgr , void * addr)
{
    ::free(addr) ;
}

char * qkf_strdup(const char * str , int size)
{
    if(str == NULL || size <= 0)
        return NULL ;

    int nsize = size ;
    if(str[size - 1] != '\0')
        ++nsize ;

    char * result = (char *)qkf_malloc(kDefaultMMgr , nsize) ;
    if(result == NULL)
        return NULL ;

    ::memcpy(result , str , size) ;
    result[size] = '\0' ;
    return result ;
}

