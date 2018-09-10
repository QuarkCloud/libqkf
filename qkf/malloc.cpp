
#include "qkf/malloc.h"
#include <stdlib.h>


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

