
#include "qkf/expr.h"
#include "qkf/malloc.h"


bool qkf_expr_node_init(qkf_expr_node_t * node) 
{
    if(node == NULL)
        return false ;
    node->left = node->right = node->parent = NULL ;
    node->scan = NULL ;
    node->free = NULL ;
    node->type = kExprNodeUnknown ;
    return true ;
}

void qkf_expr_node_free(qkf_expr_node_t * node) 
{
    if(node == NULL)
        return ;

    if(node->left != NULL)
    {
        qkf_expr_node_t * left = node->left ;
        if(left->free != NULL)
            left->free(left) ;
        ::qkf_free(kDefaultMMgr , left) ;
        node->left = NULL ;
    }

    if(node->right != NULL)
    {
        qkf_expr_node_t * right = node->right ;
        if(right->free != NULL)
            right->free(right) ;
        ::qkf_free(kDefaultMMgr , right) ;
        node->right = NULL ;
    }
}

void qkf_expr_node_final(qkf_expr_node_t * node) 
{
    if(node == NULL)
        return ;

    qkf_expr_node_free(node) ;
}

void qkf_expr_node_assign(qkf_expr_node_t * node , qkf_expr_node_t * src) 
{
    if(node == NULL || src == NULL)
        return ;
    node->type = src->type  ;         
}

bool qkf_expr_node_equal(const qkf_expr_node_t * src , const qkf_expr_node_t * dst) 
{
    if(src == NULL || dst == NULL)
        return false ;
    return src->type == dst->type ;
}

bool qkf_expr_node_empty(const qkf_expr_node_t * node) 
{
    if(node == NULL || node->type == kExprNodeUnknown)
        return true ;
    else
        return false ;
}


bool qkf_arith_node_init(qkf_arith_node_t * node) 
{
    if((node == NULL) || (qkf_expr_node_init(&node->link) == false))
        return false ;

    node->link.type = kExprNodeArith ;
    node->name = node->opera = node->value = NULL ;
    node->link.scan = qkf_arith_node_scan ;
    node->link.free = qkf_arith_node_free ;
    return true ;
}

bool qkf_arith_node_free(qkf_expr_node_t * node) 
{
    if(node == NULL || node->type != kExprNodeArith)
        return false ;

    qkf_arith_node_t * arith = (qkf_arith_node_t *) node ;
    if(arith->name != NULL)
    {
        ::qkf_free(arith->name) ;
        arith->name = NULL ;
    }
    if(arith->opera != NULL)
    {
        ::qkf_free(arith->opera) ;
        arith->opera = NULL ;
    }
    if(arith->value != NULL)
    {
        ::qkf_free(arith->value) ;
        arith->value = NULL ;
    }
    return true ;
}

int  qkf_arith_node_scan(qkf_expr_node_t * node , const char * str , int len) 
{

}

bool qkf_arith_node_final(qkf_arith_node_t * node) 
{

}


bool qkf_logical_node_init(qkf_logical_node_t * node) 
{

}

bool qkf_logical_node_free(qkf_expr_node_t * node) 
{

}

bool qkf_logical_node_final(qkf_logical_node_t * node) 
{

}


bool qkf_deguacus_node_init(qkf_deguacus_node_t * node) 
{

}

bool qkf_deguacus_node_free(qkf_expr_node_t * node) 
{

}

int  qkf_deguacus_node_scan(qkf_expr_node_t * node , const char * str , int len) 
{

}

bool qkf_deguacus_node_final(qkf_deguacus_node_t * node) 
{

}


int  qkf_deguacus_node_scan_cont(const char * str , int len , const char *&dstr , int&dlen) 
{

}


int  qkf_deguacus_node_parse_cont(qkf_deguacus_node_t * node , const char * str , int len) 
{

}

int  qkf_deguacus_node_add_arith(qkf_deguacus_node_t * node , qkf_arith_node_t * arith) 
{

}

int  qkf_deguacus_node_add_logical(qkf_deguacus_node_t * node , qkf_logical_node_t * logical) 
{

}



bool qkf_expr_init(qkf_expr_t * expr) 
{

}

void qkf_expr_final(qkf_expr_t * expr) 
{

}

void qkf_expr_clear(qkf_expr_t * expr) 
{

}

bool qkf_expr_parse(qkf_expr_t * expr , const char * str) 
{

}

const qkf_expr_node_t * qkf_expr_first() 
{

}

const qkf_expr_node_t * qkf_expr_next(const qkf_expr_node_t * node) 
{

}

const qkf_expr_node_t * qkf_expr_left(const qkf_expr_node_t * node) 
{

}


bool qkf_expr_add_node(qkf_expr_t * expr , qkf_expr_node_t * node) 
{

}

bool qkf_expr_add_logical(qkf_expr_t * expr , qkf_logical_node_t * logical) 
{

}


int qkf_expr_skip_space(const char * str , int slen) 
{

}
 

int qkf_expr_scan_name(const char * str , int slen , const char *& name , int& nlen) 
{

}

int qkf_expr_check_logical(const char * str , int slen , char * upper , int ulen) 
{

}

bool qkf_expr_check_digit(const char * str , int slen) 
{

}

int qkf_expr_scan_relation(const char * str , int slen , const char *& oper , int& olen) 
{

}


int qkf_expr_scan_value(const char * str , int slen , const char *& value , int& vlen) 
{

}


int qkf_expr_scan_logical(const char * str , int slen , const char *& logical , int& llen) 
{

}




