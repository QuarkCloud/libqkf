
#ifndef __QKF_FIELDX_H
#define __QKF_FIELDX_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include <stdint.h>

__BEGIN_DECLS

typedef struct __qkf_st_map_header qkf_map_header_t ;

struct __qkf_st_field_list{
    uint16_t            capacity ;
    uint16_t            size ;
    uint8_t             type ;              //Ԫ�����Ͷ���
    uint8_t             pad[3] ;            //����ֽ�
    qkf_field_data_t    datas ;             //�洢����
} ;

struct __qkf_st_field_map{
    qkf_map_header_t *  header ;
    //λͼ�ֽ��������ֶ������Ӷ����ӣ�����8���ֽ����ӡ������������ݶ�û����ʾ����
    uint64_t            bits ;           
} ;

static const int kMapHeaderBloomSize = 8 ;
struct __qkf_st_map_header{   
    uint8_t             capacity ;
    uint8_t             size ;
    //ʹ�ò�¡������ԭ�����ж��Ƿ��ظ���8���ֽ�64λ
    uint8_t             bloom[kMapHeaderBloomSize] ;      
    qkf_field_def_t     defs ;
} ;

__END_DECLS

#endif  /** __QKF_FIELDX_H */
