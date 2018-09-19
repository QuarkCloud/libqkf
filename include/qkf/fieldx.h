
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
    uint8_t             type ;              //元素类型定义
    uint8_t             pad[3] ;            //填充字节
    qkf_field_data_t    datas ;             //存储数据
} ;

struct __qkf_st_field_map{
    qkf_map_header_t *  header ;
    //位图字节数随着字段数增加而增加，按照8个字节增加。后续还有数据段没有显示出来
    uint64_t            bits ;           
} ;

static const int kMapHeaderBloomSize = 8 ;
struct __qkf_st_map_header{   
    uint8_t             capacity ;
    uint8_t             size ;
    //使用布隆过滤器原理，简单判定是否重复，8个字节64位
    uint8_t             bloom[kMapHeaderBloomSize] ;      
    qkf_field_def_t     defs ;
} ;

__END_DECLS

#endif  /** __QKF_FIELDX_H */
