
#ifndef __QKF_TUPLE_H
#define __QKF_TUPLE_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/rbtree.h"
#include <stdint.h>

__BEGIN_DECLS

/*
	tuple�����Ϊfield����������ֻ�����ڹ̶����壬���ࡣ�������������ɣ�
    1���Ƕ������������ֶ����ƺ����ͣ��Լ�ÿ���ֶε�ƫ������
    2��λͼ����ָʾ�ֶζ�Ӧ��λͼ�Ƿ��������
    3�������������������ڴ湹��
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
    uint32_t                capacity ;          //���ֽ���
    uint32_t                field_count ;       //���ٸ��ֶ�
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

QKFAPI int qkf_tuple_header_add(qkf_tuple_header_t * header , const char * name , uint8_t type) ;
QKFAPI int qkf_tuple_header_del(qkf_tuple_header_t * header , const char * name) ;
QKFAPI int qkf_tuple_header_find(qkf_tuple_header_t * header , const char * name) ;
QKFAPI bool qkf_tuple_header_get(qkf_tuple_header_t * header , int index , qkf_field_def_t*& def) ;

__END_DECLS

#endif  /** __QKF_TUPLE_H */
