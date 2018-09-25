
#ifndef __QKF_FIELDX_H
#define __QKF_FIELDX_H 1

#include "qkf/compile.h"
#include "qkf/field.h"
#include "qkf/vector.h"
#include "qkf/rbtree.h"
#include <stdint.h>

__BEGIN_DECLS

/**
    2018-09-20
    list可以近似于vector，只是vector删除的代价过高，
    所以使用环形缓冲区模式来实现list
*/
struct __st_qkf_field_list{
    uint8_t             type ;              //元素类型定义

    uint32_t            capacity ;
    uint32_t            size ;

    uint32_t            head ;
    uint32_t            tail ;

    qkf_field_data_t *  datas ;              //存放数据
} ;

typedef struct __st_qkf_field_map_node{
    rb_node_t           link ;
    qkf_field_def_t     def ;
    qkf_field_data_t    data ;
} qkf_field_map_node_t ;

struct __st_qkf_field_map{
    rb_tree_t               names ;   
    qkf_vector_t            nodes ;
} ;

//list的接口
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

//map header的接口
QKFAPI qkf_field_map_t * qkf_field_map_new() ;
QKFAPI void qkf_field_map_free(qkf_field_map_t * map) ;
QKFAPI bool qkf_field_map_init(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_final(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_node_final(qkf_field_map_node_t * node) ;

QKFAPI int qkf_field_map_add(qkf_field_map_t * map , const char * name , uint8_t type , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_map_del(qkf_field_map_t * map , const char * name) ;
QKFAPI bool qkf_field_map_update(qkf_field_map_t * map , const char * name , qkf_field_data_t * data) ;

QKFAPI int qkf_field_map_size(qkf_field_map_t * map) ;
QKFAPI bool qkf_field_map_get_by_index(qkf_field_map_t * map , int index , qkf_field_map_node_t*&node) ;
QKFAPI bool qkf_field_map_get_by_name(qkf_field_map_t * map , const char * name , int& index) ;


__END_DECLS

#endif  /** __QKF_FIELDX_H */
