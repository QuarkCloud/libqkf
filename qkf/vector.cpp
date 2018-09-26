
#include "qkf/malloc.h"
#include "qkf/vector.h"
#include <string.h>

qkf_vector_t * qkf_vector_new(int elem_size , int capacity)
{
    qkf_vector_t * v = (qkf_vector_t *)qkf_malloc(kDefaultMMgr , sizeof(qkf_vector_t)) ;
    if(v == NULL)
        return NULL ;

    if(qkf_vector_init(v , elem_size , capacity) == true)
        return v ;

    ::qkf_free(kDefaultMMgr , v) ;
    return NULL ;
}

void qkf_vector_free(qkf_vector_t * vector)
{
    if(vector == NULL)
        return ;

    qkf_vector_final(vector) ;
    ::qkf_free(kDefaultMMgr , vector) ;
}

bool qkf_vector_init(qkf_vector_t * vector , int elem_size , int capacity)
{
    if(vector == NULL || elem_size < 0 || capacity <= 0)
        return false;

    int elem_usage = elem_size ;
    if(elem_usage > 1)
        elem_usage = ((elem_usage >> 3) + ((elem_usage & 7) == 0 ? 0 : 1)) << 3 ;

    ::memset(vector , 0 , sizeof(qkf_vector_t)) ;

    size_t vsize = elem_usage * capacity ;
    uint8_t * datas = (uint8_t *)::qkf_malloc(kDefaultMMgr , vsize) ;
    if(datas == NULL)
        return false ;

    vector->capacity = capacity ;
    vector->elem_size = elem_size ;
    vector->elem_usage = elem_usage ;
    vector->datas = datas ;

    return true ;
}

void qkf_vector_final(qkf_vector_t * vector)
{
    if(vector == NULL || vector->datas == NULL)
        return ;

    ::qkf_free(kDefaultMMgr , vector->datas) ;
    vector->datas = NULL ;
}

void qkf_vector_clear(qkf_vector_t * vector)
{
    if(vector != NULL)
        vector->size = 0 ;

    ::memset(vector->datas , 0 , vector->elem_usage * vector->capacity) ;
}

void *qkf_vector_get(qkf_vector_t * vector , int index)
{
    if(vector == NULL || index < 0 || index >= (int)vector->size)
        return NULL ;

    return vector->datas + index * vector->elem_usage ;
}

bool qkf_vector_set(qkf_vector_t * vector , int index , void * data)
{
    void * addr = qkf_vector_get(vector , index) ;
    if(addr == NULL)
        return false ;
 
    uint16_t size = vector->elem_size ;
    if(data == NULL)
        ::memset(addr , 0 , size) ;
    else
        ::memcpy(addr , data , size) ;
    return true ;
}

int qkf_vector_add(qkf_vector_t * vector , void * data)
{
    if(vector == NULL || data == NULL || vector->size >= vector->capacity)
        return -1 ;

    uint8_t * addr = vector->datas + vector->elem_usage * vector->size ;

    ::memcpy(addr , data , vector->elem_size) ;
    int index = vector->size ;
    ++vector->size ;
    return index ;
}

bool qkf_vector_del(qkf_vector_t * vector , int index)
{
    if(vector == NULL || index < 0 || index >= (int)vector->size)
        return false ;
    int count = vector->size - index ;

    if(count > 1)
    {
        uint8_t * to_addr = vector->datas + vector->elem_usage * index ;
        uint8_t * from_addr = to_addr + vector->elem_usage ;

        ::memcpy(to_addr , from_addr , count * vector->elem_usage) ;
    }

    --vector->size ;
    return true ;
}

int qkf_vector_size(qkf_vector_t * vector)
{
    if(vector == NULL)
        return 0 ;
    else
        return vector->size ;
}

uint32_t qkf_vector_extend_size(uint32_t old_size)
{
    uint32_t capacity = old_size ;
    if(capacity == 0)
        capacity = 8 ;
    else if(capacity < 4096)
        capacity = capacity << 1 ;
    else
        capacity += 4096 ;
    return capacity ;
}

bool qkf_vector_extend(qkf_vector_t * vector)
{
    if(vector == NULL)
        return false ;

    uint32_t capacity = qkf_vector_extend_size(vector->capacity) ;

    size_t ext_size = capacity * vector->elem_usage ;
    uint8_t * datas = (uint8_t *)::qkf_malloc(kDefaultMMgr , ext_size) ;
    if(datas == NULL)
        return false ;
    ::memset(datas , 0 , ext_size) ;

    uint8_t * old_datas = vector->datas ;
    size_t old_size = vector->elem_usage * vector->capacity ;
    if(old_size != 0 && vector->datas != NULL)
        ::memcpy(datas , vector->datas , old_size) ;

    vector->datas = datas ;
    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    vector->capacity = capacity ;
    return true ;
}

uint32_t qkf_vector_shrink_size(uint32_t capacity , uint32_t size)
{
    uint32_t newcap = 0 ;
    if(capacity >= 4096)
    {
        if(capacity < (size * 2))
            return  0 ;
        newcap = capacity - 4096 ;
    }
    else
    {
        if(capacity < (size * 3))
            return 0 ;

        newcap = capacity >> 1 ;
    }

    if(newcap < 8)
        newcap = 8 ;
    if(newcap >= capacity)
        return 0 ;
    else
        return newcap ;
}

bool qkf_vector_shrink(qkf_vector_t * vector)
{
    if(vector == NULL)
        return false ;
    if(vector->datas == NULL || vector->capacity == 0)
        return true ;

    uint32_t newcap = qkf_vector_shrink_size(vector->capacity , vector->size) ;
    if(newcap == 0)
        return false ;

    size_t shr_size = vector->elem_usage * newcap ;
    uint8_t * datas = (uint8_t *)::qkf_malloc(kDefaultMMgr , shr_size) ;
    if(datas == NULL)
        return false ;

    ::memcpy(datas , vector->datas , shr_size) ;
    uint8_t * old_datas = vector->datas ;
    vector->datas = datas ;

    if(old_datas != NULL)
        ::qkf_free(kDefaultMMgr , old_datas) ;

    return true ;
}
