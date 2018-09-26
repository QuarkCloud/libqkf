
#ifndef __QKF_FIELD_H
#define __QKF_FIELD_H 1

#include "qkf/compile.h"
#include "qkf/rbtree.h"
#include "qkf/vector.h"
#include <stdint.h>
#include <sys/time.h>

__BEGIN_DECLS

typedef union  __un_qkf_field_data      qkf_field_data_t ;
typedef struct __st_qkf_field_str       qkf_field_str_t ;
typedef struct __st_qkf_field_raw       qkf_field_raw_t ;
typedef struct __st_qkf_field_list      qkf_field_list_t ;
typedef struct __st_qkf_field_map       qkf_field_map_t ;

typedef struct __st_qkf_field_def       qkf_field_def_t ;

union __un_qkf_field_data{
    bool                bv  ;
    int64_t		        ts ;

    int8_t              i8  ;
    uint8_t             u8  ;
    int16_t             i16 ;
    uint16_t            u16 ;
    int32_t             i32 ;
    uint32_t            u32 ;
    int64_t             i64 ;
    uint64_t            u64 ;

    float               f32 ;
    double              f64 ;

    qkf_field_str_t *   str ;       //ֻ֧�������ֽڳ��ȵ��ַ�������\0��β��
    qkf_field_raw_t *   raw ;       //���֧���ĸ��ֽڳ��ȵĶ�����
    qkf_field_list_t *  list ;
    qkf_field_map_t *   map ;

    uint64_t            val ;
} ;

static const uint8_t       kTypeMIN         =   0   ;
static const uint8_t       kTypeVOID        =   0   ;
static const uint8_t       kTypeBOOL        =   1   ;
static const uint8_t       kTypeTIME        =   2   ;

static const uint8_t       kTypeINT8        =   3   ;
static const uint8_t       kTypeUINT8       =   4   ;
static const uint8_t       kTypeINT16       =   5   ;
static const uint8_t       kTypeUINT16      =   6   ;
static const uint8_t       kTypeINT32       =   7   ;
static const uint8_t       kTypeUINT32      =   8   ;
static const uint8_t       kTypeINT64       =   9   ;
static const uint8_t       kTypeUINT64      =   10  ;
static const uint8_t       kTypeFLOAT       =   11  ;
static const uint8_t       kTypeDOUBLE      =   12  ;

static const uint8_t       kTypeSTR         =   13  ;
static const uint8_t       kTypeRAW         =   14  ;
static const uint8_t       kTypeLIST        =   15  ;
static const uint8_t       kTypeMAP         =   16  ;
static const uint8_t       kTypeMAX         =   16  ;


struct __st_qkf_field_def{
    char *          name ;
    uint8_t         type ;              //�������ͣ�ָʾfield_data_t�У�ȡ��һ�ֹ�����
    int8_t          index ;             //�ڼ�¼�е��±꣬-1��ʾ�Ƿ�����0��ʼ
    uint16_t        offset ;            //�ڼ�¼�е�ƫ����
} ;

struct __st_qkf_field_str{
    uint16_t    size ;                  //��¼�ַ����ĳ��ȣ������������\0
    char        data[6] ;               //ƴ��8���ֽ�
} ;

struct __st_qkf_field_raw{
    uint32_t    size ;
    char        data[4] ;    
} ;


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

typedef struct __st_qkf_field_map_node{
    rb_node_t           link ;
    qkf_field_def_t     def ;
    qkf_field_data_t    data ;
} qkf_field_map_node_t ;

struct __st_qkf_field_map{
    rb_tree_t               names ;   
    qkf_vector_t            nodes ;
} ;

QKFAPI qkf_field_str_t * qkf_field_str_new(const char * str) ;
QKFAPI qkf_field_raw_t * qkf_field_raw_new(const void * raw , size_t size) ;

//���ݽӿ�
QKFAPI void qkf_field_data_set_bool(qkf_field_data_t * field , bool value) ;
QKFAPI void qkf_field_data_set_time(qkf_field_data_t * field , time_t value) ;
QKFAPI void qkf_field_data_set_int8(qkf_field_data_t * field , int8_t value) ;
QKFAPI void qkf_field_data_set_uint8(qkf_field_data_t * field , uint8_t value) ;
QKFAPI void qkf_field_data_set_int16(qkf_field_data_t * field , int16_t value) ;
QKFAPI void qkf_field_data_set_uint16(qkf_field_data_t * field , uint16_t value) ;
QKFAPI void qkf_field_data_set_int32(qkf_field_data_t * field , int32_t value) ;
QKFAPI void qkf_field_data_set_uint32(qkf_field_data_t * field , uint32_t value) ;
QKFAPI void qkf_field_data_set_int64(qkf_field_data_t * field , int64_t value) ;
QKFAPI void qkf_field_data_set_uint64(qkf_field_data_t * field , uint64_t value) ;
QKFAPI void qkf_field_data_set_float(qkf_field_data_t * field , float value) ;
QKFAPI void qkf_field_data_set_double(qkf_field_data_t * field , double value) ;
QKFAPI void qkf_field_data_set_str(qkf_field_data_t * field , const char * str) ;
QKFAPI void qkf_field_data_set_raw(qkf_field_data_t * field , const void * raw , size_t size) ;
QKFAPI void qkf_field_data_set_list(qkf_field_data_t * field , const qkf_field_list_t *list) ;
QKFAPI void qkf_field_data_set_map(qkf_field_data_t * field , const qkf_field_map_t * map) ;

QKFAPI void qkf_field_data_attach_str(qkf_field_data_t * field , qkf_field_str_t * str) ;
QKFAPI void qkf_field_data_attach_raw(qkf_field_data_t * field , qkf_field_raw_t * raw) ;
QKFAPI void qkf_field_data_attach_list(qkf_field_data_t * field , qkf_field_list_t *list) ;
QKFAPI void qkf_field_data_attach_map(qkf_field_data_t * field , qkf_field_map_t * map) ;


QKFAPI qkf_field_def_t * qkf_field_def_new(const char * name , uint8_t type) ;
QKFAPI void qkf_field_def_free(qkf_field_def_t * def) ;
QKFAPI bool qkf_field_def_init(qkf_field_def_t * def , const char * name , uint8_t type) ;
QKFAPI void qkf_field_def_final(qkf_field_def_t * def) ;

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
QKFAPI bool qkf_field_list_copy_data(qkf_field_list_t * list , qkf_field_data_t * datas) ;
QKFAPI bool qkf_field_list_copy(const qkf_field_list_t * src , qkf_field_list_t * dst) ;

QKFAPI uint32_t qkf_field_list_extend_size(uint32_t old_size) ;
QKFAPI uint32_t qkf_field_list_shrink_size(uint32_t old_size) ;

//map header�Ľӿ�
QKFAPI qkf_field_map_t * qkf_field_map_new() ;
QKFAPI void qkf_field_map_free(qkf_field_map_t * map) ;
QKFAPI bool qkf_field_map_init(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_final(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_clear(qkf_field_map_t * map) ;
QKFAPI void qkf_field_map_node_final(qkf_field_map_node_t * node) ;

QKFAPI int qkf_field_map_add(qkf_field_map_t * map , const char * name , uint8_t type , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_map_del(qkf_field_map_t * map , const char * name) ;
QKFAPI bool qkf_field_map_update(qkf_field_map_t * map , const char * name , qkf_field_data_t * data) ;
QKFAPI bool qkf_field_map_get(qkf_field_map_t * map , int index , qkf_field_map_node_t*&node) ;
QKFAPI int qkf_field_map_find(qkf_field_map_t * map , const char * name) ;

QKFAPI bool qkf_field_map_copy(const qkf_field_map_t * src , qkf_field_map_t * dst) ;
QKFAPI int qkf_field_map_size(qkf_field_map_t * map) ;



__END_DECLS

#endif  /** __QKF_FIELD_H */
