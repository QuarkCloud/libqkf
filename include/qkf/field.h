
#ifndef __QKF_FIELD_H
#define __QKF_FIELD_H 1

#include "qkf/compile.h"
#include <stdint.h>
#include <sys/time.h>

__BEGIN_DECLS

typedef union  __qkf_un_field_data      qkf_field_data_t ;
typedef struct __qkf_st_field_str       qkf_field_str_t ;
typedef struct __qkf_st_field_raw       qkf_field_raw_t ;
typedef struct __qkf_st_field_list      qkf_field_list_t ;
typedef struct __qkf_st_field_map       qkf_field_map_t ;

typedef struct __qkf_st_field_def       qkf_field_def_t ;

union __qkf_un_field_data{
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


struct __qkf_st_field_def{
    char *          name ;
    uint8_t         type ;              //�������ͣ�ָʾfield_data_t�У�ȡ��һ�ֹ�����
    int8_t          index ;             //�ڼ�¼�е��±꣬-1��ʾ�Ƿ�����0��ʼ
    uint16_t        offset ;            //�ڼ�¼�е�ƫ����
} ;

struct __qkf_st_field_str{
    uint16_t    size ;
    char        data[6] ;               //ƴ��8���ֽ�
} ;

struct __qkf_st_field_raw{
    uint32_t    size ;
    char        data[4] ;    
} ;

QKFAPI qkf_field_def_t * qkf_field_def_new(const char * name , uint8_t type) ;
QKFAPI bool qkf_field_def_init(qkf_field_def_t * def , const char * name , uint8_t type) ;
QKFAPI void qkf_field_def_final(qkf_field_def_t * def , const char * name , uint8_t type) ;


__END_DECLS

#endif  /** __QKF_FIELD_H */
