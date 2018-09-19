
#ifndef __QKF_FIELDX_H
#define __QKF_FIELDX_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include <stdint.h>

__BEGIN_DECLS

typedef struct __st_qkf_map_header qkf_map_header_t ;

//list可以近似于vector
struct __st_qkf_field_list{
    uint8_t             type ;              //元素类型定义
    
} ;

struct __st_qkf_field_map{
    qkf_map_header_t *  header ;
    //位图字节数随着字段数增加而增加，按照8个字节增加。后续还有数据段没有显示出来
    uint64_t            bits ;           
} ;

static const int kMapHeaderBloomSize = 8 ;
struct __st_qkf_map_header{   
    //使用布隆过滤器原理，简单判定是否重复，8个字节64位
    uint8_t             bloom[kMapHeaderBloomSize] ;      
    qkf_vector_t        defs ;
} ;

QKFAPI qkf_field_list_t * qkf_field_list_new(int capacity) ;
QKFAPI bool qkf_field_list_init(qkf_field_list_t * list , int capacity) ;
QKFAPI void qkf_field_list_final(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_free(qkf_field_list_t * list) ;
QKFAPI void qkf_field_list_clear(qkf_field_list_t * list) ;

QKFAPI bool qkf_field_list_push(qkf_field_list_t * list , qkf_field_data_t * data) ;

__END_DECLS

#endif  /** __QKF_FIELDX_H */
