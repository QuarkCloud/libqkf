
#ifndef __QKF_VECTOR_H
#define __QKF_VECTOR_H 1

#include "qkf/compile.h"
#include <stdint.h>

__BEGIN_DECLS

typedef struct __st_qkf_vector qkf_vector_t ;
struct __st_qkf_vector{
    uint32_t        capacity ;          
    uint32_t        size ;

    uint16_t        elem_size ;         //数据长度
    uint16_t        elem_usage ;        //真实占用内存字节数，除1以外，按照8个字节对齐

    uint8_t         *datas ;            //数据
} ;

QKFAPI qkf_vector_t * qkf_vector_new(int elem_size , int capacity) ;
QKFAPI void qkf_vector_free(qkf_vector_t * vector) ;
QKFAPI bool qkf_vector_init(qkf_vector_t * vector , int elem_size , int capacity) ;
QKFAPI void qkf_vector_final(qkf_vector_t * vector) ;
QKFAPI void qkf_vector_clear(qkf_vector_t * vector) ;
QKFAPI void *qkf_vector_get(qkf_vector_t * vector , int index) ;
QKFAPI bool qkf_vector_set(qkf_vector_t * vector , int index , void * data) ;
QKFAPI int qkf_vector_add(qkf_vector_t * vector , void * data) ;
QKFAPI bool qkf_vector_del(qkf_vector_t * vector , int index) ;
QKFAPI int qkf_vector_size(const qkf_vector_t * vector) ;
QKFAPI bool qkf_vector_copy(const qkf_vector_t * src , qkf_vector_t * dst) ;

QKFAPI bool qkf_vector_extend(qkf_vector_t * vector) ;
QKFAPI bool qkf_vector_shrink(qkf_vector_t * vector) ;

QKFAPI uint32_t qkf_vector_calc_extend_size(uint32_t old_size) ;
QKFAPI uint32_t qkf_vector_calc_shrink_size(uint32_t capacity , uint32_t size) ;


__END_DECLS

#endif  /** __QKF_VECTOR_H */
