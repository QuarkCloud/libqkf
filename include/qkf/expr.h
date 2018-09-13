
#ifndef __QKF_EXPR_H
#define __QKF_EXPR_H 1

#include "qkf/compile.h"
#include <stdint.h>

__BEGIN_DECLS

/*
    �Ƚ��Ѻõ��������ã�������SQL��WHERE �����������и�������ͬ��֮����OR����ͬ����֮����AND�����£�
    (Name = "ABC" OR Name = "BCD") AND Yield > 0.5
*/
typedef struct __st_qkf_expr_node           qkf_expr_node_t ;
typedef struct __st_qkf_arith_node          qkf_arith_node_t ;
typedef struct __st_qkf_logical_node        qkf_logical_node_t ;
typedef struct __st_qkf_deguacus_node       qkf_deguacus_node_t ;
typedef struct __st_qkf_expr                qkf_expr_t ;

//��������
static const int kExprNodeUnknown   =   0 ;
static const int kExprNodeLogical   =   1 ;     //�߼������ OR ��AND
static const int kExprNodeArith     =   2 ;     //��������� > < = ..
static const int kExprNodeDeguacus  =   3 ;     //С����

struct __st_qkf_expr_node{
    int                     type ;  
    qkf_expr_node_t *       parent ;
    qkf_expr_node_t *       left ;
    qkf_expr_node_t *       right ;

    int (*scan)(qkf_expr_node_t * node , const char * str , int len) ;
    bool (*free)(qkf_expr_node_t * node) ;
} ;

struct __st_qkf_arith_node{
    qkf_expr_node_t         link ;
    char *                  name ;
    char *                  opera ;
    char *                  value ;
} ;

struct __st_qkf_logical_node{
    qkf_expr_node_t         link ;
    char *                  opera ;
} ;

struct __st_qkf_deguacus_node{
    qkf_expr_node_t         link ;
    char *                  opera ;
} ;

struct __st_qkf_expr{
    qkf_expr_node_t * root ;
} ;

QKFAPI bool qkf_expr_node_init(qkf_expr_node_t * node) ;
QKFAPI bool qkf_expr_node_free(qkf_expr_node_t * node) ;
QKFAPI void qkf_expr_node_final(qkf_expr_node_t * node) ;
QKFAPI void qkf_expr_node_assign(qkf_expr_node_t * node , qkf_expr_node_t * src) ;
QKFAPI bool qkf_expr_node_equal(const qkf_expr_node_t * src , const qkf_expr_node_t * dst) ;
QKFAPI bool qkf_expr_node_empty(const qkf_expr_node_t * node) ;

QKFAPI bool qkf_arith_node_init(qkf_arith_node_t * node) ;
QKFAPI bool qkf_arith_node_free(qkf_expr_node_t * node) ;
QKFAPI int  qkf_arith_node_scan(qkf_expr_node_t * node , const char * str , int slen) ;
QKFAPI void qkf_arith_node_final(qkf_arith_node_t * node) ;

QKFAPI bool qkf_logical_node_init(qkf_logical_node_t * node) ;
QKFAPI bool qkf_logical_node_free(qkf_expr_node_t * node) ;
QKFAPI void qkf_logical_node_final(qkf_logical_node_t * node) ;

QKFAPI bool qkf_deguacus_node_init(qkf_deguacus_node_t * node) ;
QKFAPI bool qkf_deguacus_node_free(qkf_expr_node_t * node) ;
QKFAPI int  qkf_deguacus_node_scan(qkf_expr_node_t * node , const char * str , int slen) ;
QKFAPI void qkf_deguacus_node_final(qkf_deguacus_node_t * node) ;
//ɨ��С����֮������ݷ�Χ
QKFAPI int  qkf_deguacus_node_scan_cont(const char * str , int slen , const char *&dstr , int&dlen) ;
//����С����֮�������
QKFAPI int  qkf_deguacus_node_parse_cont(qkf_deguacus_node_t * node , const char * str , int slen) ;
QKFAPI int  qkf_deguacus_node_add_arith(qkf_deguacus_node_t * node , qkf_arith_node_t * arith) ;
QKFAPI int  qkf_deguacus_node_add_logical(qkf_deguacus_node_t * node , qkf_logical_node_t * logical) ;


QKFAPI bool qkf_expr_init(qkf_expr_t * expr) ;
QKFAPI void qkf_expr_final(qkf_expr_t * expr) ;
QKFAPI void qkf_expr_clear(qkf_expr_t * expr) ;
QKFAPI bool qkf_expr_parse(qkf_expr_t * expr , const char * str) ;
QKFAPI const qkf_expr_node_t * qkf_expr_first() ;
QKFAPI const qkf_expr_node_t * qkf_expr_next(const qkf_expr_node_t * node) ;
QKFAPI const qkf_expr_node_t * qkf_expr_left(const qkf_expr_node_t * node) ;

QKFAPI bool qkf_expr_add_node(qkf_expr_t * expr , qkf_expr_node_t * node) ;
QKFAPI bool qkf_expr_add_logical(qkf_expr_t * expr , qkf_logical_node_t * logical) ;

QKFAPI int qkf_expr_skip_space(const char * str , int slen) ; 
//��[A-Z , a-z , _]��ͷ����������[A-Z , a-z , _ , 0-9]�������OR , AND����ô������ɴ�д��
QKFAPI int qkf_expr_scan_name(const char * str , int slen , const char *& name , int& nlen) ;
QKFAPI int qkf_expr_check_logical(const char * str , int slen , char * upper , int ulen) ;
QKFAPI bool qkf_expr_check_digit(const char * str , int slen) ;
QKFAPI int qkf_expr_scan_relation(const char * str , int slen , const char *& oper , int& olen) ;
//ɨ��ֵ�������""Ϊ�磬��ôȡ�����ڲ������ݣ����û�У����Կո�Ϊ��
QKFAPI int qkf_expr_scan_value(const char * str , int slen , const char *& value , int& vlen) ;
//ɨ���߼��������ֻ����"OR"������"AND"
QKFAPI int qkf_expr_scan_logical(const char * str , int slen , const char *& logical , int& llen) ;



__END_DECLS

#endif  /** __QKF_EXPR_H */
