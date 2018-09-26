
#ifndef __QKF_TUPLE_H
#define __QKF_TUPLE_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/rbtree.h"
#include <stdint.h>

__BEGIN_DECLS

/*
	tuple被设计为field的容器，它只适用于固定定义，如类。它有三个区构成，
    1、是定义区，包括字段名称和类型，以及每个字段的偏移量。
    2、位图区，指示字段对应的位图是否存在数据
    3、数据区，由连续的内存构成
*/
typedef struct __st_qkf_tuple_header_node   qkf_tuple_header_node_t ;
typedef struct __st_qkf_tuple_header        qkf_tuple_header_t ;
typedef struct __st_qkf_tuple_body          qkf_tuple_body_t ;
typedef struct __st_qkf_tuple               qkf_tuple_t ;

struct __st_qkf_tuple_header_node{
    rb_node_t                   link ;
    qkf_field_def_t             def ;
} ;

struct __st_qkf_tuple_header{
    rb_tree_t           names ;
    qkf_vector_t        nodes ;
} ;

struct __st_qkf_tuple_body{
    uint32_t                capacity ;          //总字节数
    uint32_t                field_count ;       //多少个字段
    union{
        uint64_t            bits ;
        qkf_field_data_t    datas ;
    } ;
} ;

struct __st_qkf_tuple{
    qkf_tuple_header_t   *  header ;   
    qkf_tuple_body_t     *  body ;
} ;


QKFAPI qkf_tuple_header_t * qkf_tuple_header_new() ;
QKFAPI void qkf_tuple_header_free(qkf_tuple_header_t * header) ;
QKFAPI bool qkf_tuple_header_init(qkf_tuple_header_t * header) ;
QKFAPI void qkf_tuple_header_final(qkf_tuple_header_t * header) ;
QKFAPI int qkf_tuple_header_node_compare(const rb_node_t * s1 , const rb_node_t * s2) ;

QKFAPI int qkf_tuple_header_add(qkf_tuple_header_t * header , const char * name , uint8_t type) ;
QKFAPI bool qkf_tuple_header_del(qkf_tuple_header_t * header , const char * name) ;
QKFAPI int qkf_tuple_header_find(qkf_tuple_header_t * header , const char * name) ;
QKFAPI bool qkf_tuple_header_get(qkf_tuple_header_t * header , int index , qkf_field_def_t*& def) ;

QKFAPI qkf_tuple_t * qkf_tuple_new(qkf_tuple_header_t * header) ;
QKFAPI bool qkf_tuple_init(qkf_tuple_t * tuple , qkf_tuple_header_t * header) ;
QKFAPI void qkf_tuple_final(qkf_tuple_t * tuple) ;
QKFAPI void qkf_tuple_free(qkf_tuple_t * tuple) ;
QKFAPI void qkf_tuple_clear(qkf_tuple_t * tuple) ;

QKFAPI int qkf_tuple_find(qkf_tuple_t * tuple , const char * name) ;
QKFAPI bool qkf_tuple_set(qkf_tuple_t * tuple , int index , uint8_t type , qkf_field_data_t * data) ;
QKFAPI bool qkf_tuple_get(qkf_tuple_t * tuple , int index , uint8_t& type , qkf_field_data_t *& data) ;

QKFAPI bool qkf_tuple_set_bool(qkf_tuple_t * tuple , int index , bool value) ;
QKFAPI bool qkf_tuple_set_time(qkf_tuple_t * tuple , int index , time_t value) ;
QKFAPI bool qkf_tuple_set_int8(qkf_tuple_t * tuple , int index , int8_t value) ;
QKFAPI bool qkf_tuple_set_uint8(qkf_tuple_t * tuple , int index , uint8_t value) ;
QKFAPI bool qkf_tuple_set_int16(qkf_tuple_t * tuple , int index , int16_t value) ;
QKFAPI bool qkf_tuple_set_uint16(qkf_tuple_t * tuple , int index , uint16_t value) ;
QKFAPI bool qkf_tuple_set_int32(qkf_tuple_t * tuple , int index , int32_t value) ;
QKFAPI bool qkf_tuple_set_uint32(qkf_tuple_t * tuple , int index , uint32_t value) ;
QKFAPI bool qkf_tuple_set_int64(qkf_tuple_t * tuple , int index , int64_t value) ;
QKFAPI bool qkf_tuple_set_uint64(qkf_tuple_t * tuple , int index , uint64_t value) ;
QKFAPI bool qkf_tuple_set_float(qkf_tuple_t * tuple , int index , float value) ;
QKFAPI bool qkf_tuple_set_double(qkf_tuple_t * tuple , int index , double value) ;
QKFAPI bool qkf_tuple_set_str(qkf_tuple_t * tuple , int index , const char * str) ;
QKFAPI bool qkf_tuple_set_raw(qkf_tuple_t * tuple , int index , const void * data , size_t size) ;
QKFAPI bool qkf_tuple_set_list(qkf_tuple_t * tuple , int index , qkf_field_list_t *list) ;
QKFAPI bool qkf_tuple_set_map(qkf_tuple_t * tuple , int index , qkf_field_map_t * map) ;


__END_DECLS

#endif  /** __QKF_TUPLE_H */
