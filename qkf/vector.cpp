
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
    uint8_t * data = (uint8_t *)::qkf_malloc(kDefaultMMgr , vsize) ;
    if(data == NULL)
        return false ;

    vector->capacity = capacity ;
    vector->elem_size = elem_size ;
    vector->elem_usage = elem_usage ;
    vector->data = data ;

    return true ;
}

void qkf_vector_final(qkf_vector_t * vector)
{
    if(vector == NULL || vector->data == NULL)
        return ;

    ::qkf_free(kDefaultMMgr , vector->data) ;
}

void qkf_vector_clear(qkf_vector_t * vector)
{
    if(vector != NULL)
        vector->size = 0 ;
}

void *qkf_vector_get(qkf_vector_t * vector , int index)
{
    if(vector == NULL || index < 0 || index >= (int)vector->size)
        return NULL ;

    return vector->data + index * vector->elem_usage ;
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

bool qkf_vector_add(qkf_vector_t * vector , void * data)
{
    if(vector == NULL || data == NULL || vector->size >= vector->capacity)
        return false ;

    uint8_t * addr = vector->data + vector->elem_usage * vector->size ;

    ::memcpy(addr , data , vector->elem_size) ;
    ++vector->size ;
    return true ;
}

bool qkf_vector_del(qkf_vector_t * vector , int index)
{
    if(vector == NULL || index < 0 || index >= (int)vector->size)
        return false ;
    int count = vector->size - index ;

    if(count > 1)
    {
        uint8_t * to_addr = vector->data + vector->elem_usage * index ;
        uint8_t * from_addr = to_addr + vector->elem_usage ;

        ::memcpy(to_addr , from_addr , count * vector->elem_usage) ;
    }

    --vector->size ;
    return true ;
}

bool qkf_vector_extend(qkf_vector_t * vector)
{
    if(vector == NULL)
        return false ;

    uint32_t capacity = vector->capacity ;
    if(capacity == 0)
        capacity = 8 ;
    else if(capacity < 4096)
        capacity = capacity << 1 ;
    else
        capacity += 4096 ;

    size_t ext_size = capacity * vector->elem_usage ;
    uint8_t * data = (uint8_t *)::qkf_malloc(kDefaultMMgr , ext_size) ;
    if(data == NULL)
        return false ;
    ::memset(data , 0 , ext_size) ;

    uint8_t * old_data = vector->data ;
    size_t old_size = vector->elem_usage * vector->capacity ;
    if(old_size != 0 && vector->data != NULL)
        ::memcpy(data , vector->data , old_size) ;

    vector->data = data ;
    if(old_data != NULL)
        ::qkf_free(kDefaultMMgr , old_data) ;

    vector->capacity = capacity ;
    return true ;
}

bool qkf_vector_shrink(qkf_vector_t * vector)
{
    if(vector == NULL)
        return false ;
    if(vector->data == NULL || vector->capacity == 0)
        return true ;

    uint32_t capacity = vector->capacity ;
    uint32_t size = vector->size ;
    uint32_t newcap = 0 ;

    if(capacity >= 4096)
    {
        if(capacity < (size * 2))
            return  false ;
        newcap = capacity - 4096 ;
    }
    else
    {
        if(capacity < (size * 3))
            return false ;

        newcap = capacity >> 1 ;
    }

    if(newcap < 8)
        newcap = 8 ;
    if(newcap >= capacity)
        return false ;

    size_t shr_size = vector->elem_usage * newcap ;
    uint8_t * data = (uint8_t *)::qkf_malloc(kDefaultMMgr , shr_size) ;
    if(data == NULL)
        return false ;

    ::memcpy(data , vector->data , shr_size) ;
    uint8_t * old_data = vector->data ;
    vector->data = data ;

    if(old_data != NULL)
        ::qkf_free(kDefaultMMgr , old_data) ;

    return true ;
}