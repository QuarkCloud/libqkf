
#ifndef __QKF_FIELDX_H
#define __QKF_FIELDX_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
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
    //λͼ�ֽ��������ֶ������Ӷ����ӣ�����8���ֽ����ӡ������������ݶ�û����ʾ����
    uint64_t            bits ;           
} ;

static const int kMapHeaderBloomSize = 8 ;
struct __st_qkf_map_header{   
    //ʹ�ò�¡������ԭ�����ж��Ƿ��ظ���8���ֽ�64λ
    uint8_t             bloom[kMapHeaderBloomSize] ;      
    qkf_vector_t        defs ;
} ;

QKFAPI qkf_field_list_t * qkf_field_list_new(uint8_t type , int capacity) ;
QKFAPI bool qkf_field_list_init(qkf_field_list_t * list , uint8_t type , int capacity) ;
QKFAPI void qkf_field_list_final(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_free(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_clear(qkf_field_list_t * list) ;

QKFAPI bool qkf_field_list_push(qkf_field_list_t * list , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_list_pop(qkf_field_list_t * list , qkf_field_data_t & data) ;

QKFAPI bool qkf_field_list_extend(qkf_field_list_t * list) ;
QKFAPI bool qkf_field_list_shrink(qkf_field_list_t * list) ;

__END_DECLS

#endif  /** __QKF_FIELDX_H */
