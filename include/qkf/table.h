
#ifndef __QKF_TABLE_H
#define __QKF_TABLE_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/tuple.h"

__BEGIN_DECLS

typedef qkf_tuple_header_t          qkf_table_header_t ;
typedef struct __st_qkf_table       qkf_table_t ;
typedef struct __st_qkf_cursor      qkf_cursor_t ;

struct __st_qkf_table{
    qkf_table_header_t          header ;
    qkf_vector_t                tuples ;
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

__END_DECLS

#endif  /** __QKF_TABLE_H */
