
#ifndef __QKF_FIELDX_H
#define __QKF_FIELDX_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/rbtree.h"
#include <stdint.h>

__BEGIN_DECLS

typedef struct __st_qkf_map_header qkf_map_header_t ;

/**
    2018-09-20
    list���Խ�����vector��ֻ��vectorɾ���Ĵ��۹��ߣ�
    ����ʹ�û��λ�����ģʽ��ʵ��list
*/
struct __st_qkf_field_list{
    uint8_t             type ;              //Ԫ�����Ͷ���

    uint32_t            capacity ;
    uint32_t            size ;

    uint32_t            head ;
    uint32_t            tail ;

    qkf_field_data_t *  datas ;              //�������    
} ;

struct __st_qkf_field_map{
    qkf_map_header_t *  header ;
/**
    λͼ�ֽ��������ֶ������Ӷ����ӣ�����8���ֽ����ӡ������������ݶ�û����ʾ������
*/
    uint64_t            bits ;           
} ;

struct __st_qkf_map_header{
    rb_tree_t           names ;   
    qkf_vector_t        defs ;
} ;

//list�Ľӿ�
QKFAPI qkf_field_list_t * qkf_field_list_new(uint8_t type , int capacity) ;
QKFAPI bool qkf_field_list_init(qkf_field_list_t * list , uint8_t type , int capacity) ;
QKFAPI void qkf_field_list_final(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_free(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_clear(qkf_field_list_t * list) ;

QKFAPI bool qkf_field_list_push(qkf_field_list_t * list , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_list_pop(qkf_field_list_t * list , qkf_field_data_t & data) ;

QKFAPI bool qkf_field_list_extend(qkf_field_list_t * list) ;
QKFAPI bool qkf_field_list_shrink(qkf_field_list_t * list) ;
QKFAPI bool qkf_field_list_copy(qkf_field_list_t * list , qkf_field_data_t * datas) ;

QKFAPI uint32_t qkf_field_list_extend_size(uint32_t old_size) ;
QKFAPI uint32_t qkf_field_list_shrink_size(uint32_t old_size) ;


//map header�Ľӿ�
QKFAPI qkf_map_header_t * qkf_map_header_new() ;
QKFAPI void qkf_map_header_free(qkf_map_header_t * header) ;
QKFAPI bool qkf_map_header_init(qkf_map_header_t * header) ;
QKFAPI void qkf_map_header_final(qkf_map_header_t * header) ;

QKFAPI int qkf_map_header_add(qkf_map_header_t * header , const char * name , uint8_t type) ;
QKFAPI int qkf_map_header_size(qkf_map_header_t * header) ;
QKFAPI bool qkf_map_header_get_by_index(qkf_map_header_t * header , int index , const char *& name , uint8_t& type) ;
QKFAPI bool qkf_map_header_get_by_name(qkf_map_header_t * header , const char * name , int& index) ;


//map�Ľӿ�
QKFAPI qkf_field_map_t * qkf_field_map_new() ;
QKFAPI void qkf_field_map_free(qkf_field_map_t * map) ;
QKFAPI bool qkf_field_map_init(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_final(qkf_field_map_t * map) ;


QKFAPI bool qkf_field_map_add(qkf_field_map_t * map , const char * name , uint8_t type , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_map_get(qkf_field_map_t * map , const char * name , qkf_field_data_t *& data) ;



__END_DECLS

#endif  /** __QKF_FIELDX_H */
