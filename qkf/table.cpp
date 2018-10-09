
#include "qkf/table.h"
#include "qkf/malloc.h"
#include <string.h>

qkf_table_t * qkf_table_new(const qkf_table_header_t * header)
{
    size_t size = sizeof(qkf_table_t) ;
    qkf_table_t * table = (qkf_table_t *)::qkf_malloc(kDefaultMMgr , size) ;
    if(table == NULL)
        return NULL ;
    if(qkf_table_init(table , header) == true)
        return table ;

    ::qkf_free(kDefaultMMgr , table) ;
    return NULL ;
}

bool qkf_table_init(qkf_table_t * table , const qkf_table_header_t * header)
{
    if(table == NULL ||header == NULL)
        return false ;

    if(qkf_tuple_header_copy(header , &table->header) == false)
        return false ;

    if(qkf_vector_init(&table->datas , sizeof(qkf_tuple_body_t *) , 8) == false)
        return false ;
    return true ;
}

void qkf_table_final(qkf_table_t * table)
{
    if(table == NULL)
        return ;

    qkf_tuple_t tuple ;
    int count = qkf_vector_size(&table->datas) ;
    for(int idx = 0 ; idx < count ; ++idx)
    {
        qkf_tuple_body_t * body = (qkf_tuple_body_t *)::qkf_vector_get(&table->datas , idx) ;
        if(body == NULL)
            continue ;
        tuple.header = &table->header ;
        tuple.body = body ;
        qkf_tuple_final(&tuple) ;
    }
    qkf_vector_final(&table->datas) ;

    qkf_tuple_header_final(&table->header) ;
}

void qkf_table_free(qkf_table_t * table)
{
    if(table == NULL)
        return ;
    qkf_table_final(table) ;
    ::qkf_free(kDefaultMMgr , table) ;
}

void qkf_table_clear(qkf_table_t * table)
{
    if(table == NULL)
        return ;

    qkf_tuple_t tuple ;
    int count = qkf_vector_size(&table->datas) ;
    for(int idx = 0 ; idx < count ; ++idx)
    {
        qkf_tuple_body_t * body = (qkf_tuple_body_t*)::qkf_vector_get(&table->datas , idx) ;
        if(body == NULL)
            continue ;
        tuple.header = &table->header ;
        tuple.body = body ;
        qkf_tuple_final(&tuple) ;
    }

    qkf_vector_clear(&table->datas) ;
}


qkf_table_header_t * qkf_table_get_header(qkf_table_t * table)
{
    if(table == NULL)
        return NULL ;
    return &table->header ;
}

bool qkf_table_add(qkf_table_t * table , qkf_tuple_t * tuple)
{
    if(table == NULL || tuple == NULL)
        return false ;
    return (qkf_vector_add(&table->datas , tuple->body) >= 0) ;
}

qkf_cursor_t * qkf_cursor_new(qkf_table_t * table)
{
    size_t size = sizeof(qkf_cursor_t) ;
    qkf_cursor_t *cursor = (qkf_cursor_t *)::qkf_malloc(kDefaultMMgr , size) ;
    if(cursor != NULL && qkf_cursor_init(table , cursor) == true)
        return cursor ;

    ::qkf_free(kDefaultMMgr , cursor) ;
    return NULL ;
}

bool qkf_cursor_init(qkf_table_t * table , qkf_cursor_t * cursor)
{
    if(table == NULL || cursor == NULL)
        return false ;

    ::memset(cursor , 0 , sizeof(qkf_cursor_t)) ;
    cursor->tuple.header = &table->header ;

    return qkf_cursor_first(table , cursor) ;
}

void qkf_cursor_final(qkf_cursor_t * cursor)
{
    if(cursor == NULL)
        return ;
    ::memset(cursor , 0 , sizeof(qkf_cursor_t)) ;
}

void qkf_cursor_free(qkf_cursor_t * cursor)
{
    qkf_cursor_final(cursor);
    if(cursor != NULL)
        ::qkf_free(kDefaultMMgr , cursor) ;
}

bool qkf_cursor_first(qkf_table_t *table , qkf_cursor_t * cursor)
{
    if(table == NULL || cursor == NULL)
        return false ;

    qkf_tuple_body_t * body = (qkf_tuple_body_t * )qkf_vector_get(&table->datas , 0) ;
    if(body == NULL)
        return false ;

    cursor->index = 0 ;
    cursor->tuple.header = &table->header ;
    cursor->tuple.body = body ;
    return true ;
}

bool qkf_cursor_next(qkf_table_t *table , qkf_cursor_t * cursor)
{
    if(table == NULL || cursor == NULL)
        return false ;

    int index = cursor->index + 1;
    qkf_tuple_body_t * body = (qkf_tuple_body_t *)::qkf_vector_get(&table->datas , index) ;
    if(body == NULL)
        return false ;

    cursor->index = index ;
    cursor->tuple.header = &table->header ;
    cursor->tuple.body = body ;
    return true ;
}

bool qkf_cursor_eof(qkf_table_t *table , qkf_cursor_t * cursor)
{
    if(table == NULL || cursor == NULL)
        return false ;
    uint32_t size = (uint32_t)::qkf_vector_size(&table->datas) ;
    return (cursor->index >= size) ;
}

qkf_tuple_t * qkf_cursor_data(qkf_cursor_t * cursor)
{
    if(cursor == NULL)
        return NULL ;
    return &cursor->tuple ;
}
