
#ifndef __QKF_TABLE_H
#define __QKF_TABLE_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/tuple.h"

__BEGIN_DECLS

/**
    table类如其名，具有相同定义的一组数据的集合。
*/

typedef qkf_tuple_header_t          qkf_table_header_t ;
typedef struct __st_qkf_table       qkf_table_t ;
typedef struct __st_qkf_cursor      qkf_cursor_t ;

struct __st_qkf_table{
    qkf_table_header_t          header ;
    qkf_vector_t                datas ;
} ;

struct __st_qkf_cursor{
    qkf_tuple_t                 tuple ;
    uint32_t                    index ;
} ;

QKFAPI qkf_table_t * qkf_table_new(const qkf_table_header_t * header) ;
QKFAPI bool qkf_table_init(qkf_table_t * table , const qkf_table_header_t * header) ;
QKFAPI void qkf_table_final(qkf_table_t * table) ;
QKFAPI void qkf_table_free(qkf_table_t * table) ;
QKFAPI void qkf_table_clear(qkf_table_t * table) ;

QKFAPI qkf_table_header_t * qkf_table_get_header(qkf_table_t * table) ;
QKFAPI bool qkf_table_add(qkf_table_t * table , qkf_tuple_t * tuple) ;

QKFAPI qkf_cursor_t * qkf_cursor_new(qkf_table_t * table) ;
QKFAPI bool qkf_cursor_init(qkf_table_t * table , qkf_cursor_t * cursor) ;
QKFAPI void qkf_cursor_final(qkf_cursor_t * cursor) ;
QKFAPI void qkf_cursor_free(qkf_cursor_t * cursor) ;

QKFAPI bool qkf_cursor_first(qkf_table_t *table , qkf_cursor_t * cursor) ;
QKFAPI bool qkf_cursor_next(qkf_table_t *table , qkf_cursor_t * cursor) ;
QKFAPI bool qkf_cursor_eof(qkf_table_t *table , qkf_cursor_t * cursor) ;
QKFAPI qkf_tuple_t * qkf_cursor_data(qkf_cursor_t * cursor) ;

__END_DECLS

#endif  /** __QKF_TABLE_H */
