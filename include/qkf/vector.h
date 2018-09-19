
#ifndef __QKF_VECTOR_H
#define __QKF_VECTOR_H 1

#include "qkf/compile.h"
#include <stdint.h>

__BEGIN_DECLS

typedef struct __st_qkf_vector qkf_vector_t ;
struct __st_qkf_vector{
    uint32_t        capacity ;
    uint32_t        size ;

    uint16_t        elem_size ;         //���ݳ���
    uint16_t        elem_usage ;        //��ʵռ���ڴ��ֽ�������1���⣬����8���ֽڶ���

    uint8_t         *data ;           //����
} ;

QKFAPI qkf_vector_t * qkf_vector_new(int elem_size , int capacity) ;
QKFAPI void qkf_vector_free(qkf_vector_t * vector) ;
QKFAPI bool qkf_vector_init(qkf_vector_t * vector , int elem_size , int capacity) ;
QKFAPI void qkf_vector_final(qkf_vector_t * vector) ;
QKFAPI void qkf_vector_clear(qkf_vector_t * vector) ;
QKFAPI void *qkf_vector_get(qkf_vector_t * vector , int index) ;
QKFAPI bool qkf_vector_set(qkf_vector_t * vector , int index , void * data) ;
QKFAPI bool qkf_vector_add(qkf_vector_t * vector , void * data) ;
QKFAPI bool qkf_vector_del(qkf_vector_t * vector , int index) ;

__END_DECLS

#endif  /** __QKF_VECTOR_H */
