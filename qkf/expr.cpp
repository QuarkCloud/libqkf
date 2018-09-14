
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

bool qkf_expr_node_free(qkf_expr_node_t * node) 
{
    if(node == NULL)
        return false;

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
    return true ;
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
        ::qkf_free(kDefaultMMgr , arith->name) ;
        arith->name = NULL ;
    }
    if(arith->opera != NULL)
    {
        ::qkf_free(kDefaultMMgr , arith->opera) ;
        arith->opera = NULL ;
    }
    if(arith->value != NULL)
    {
        ::qkf_free(kDefaultMMgr , arith->value) ;
        arith->value = NULL ;
    }
    return true ;
}

int qkf_arith_node_scan(qkf_expr_node_t * node , const char * str , int slen) 
{
    if(node == NULL || str == NULL || slen <= 0 || node->type != kExprNodeArith)
        return 0 ;

    //1、扫描名字
    const char * name = NULL ;
    int nlen = 0 , offset = 0 ;

    int nsize = ::qkf_expr_scan_name(str , slen , name , nlen) ;
    if(nsize == 0)
        return 0 ;
    offset += nsize ;

    //2、扫描操作符
    const char * oper = NULL ;
    int olen = 0 ;
    int osize = ::qkf_expr_scan_relation(str + offset , slen - offset , oper , olen) ;
    if(osize <= 0)
        return 0 ;
    offset += osize ;

    //3、扫描值
    const char * value = NULL  ;
    int vlen = 0 ;
    int vsize = ::qkf_expr_scan_value(str + offset , slen - offset , value , vlen) ;
    if(vsize <= 0)
        return 0 ;
    offset += vsize ;

    //4、赋值
    qkf_arith_node_t * arith =  (qkf_arith_node_t *)node ;
    arith->name = ::qkf_strdup(name , nlen) ;
    arith->opera = ::qkf_strdup(oper , olen) ;
    arith->value = ::qkf_strdup(value , vlen) ;

    return offset ;
}

void qkf_arith_node_final(qkf_arith_node_t * node) 
{
    if(node == NULL)
        return ;

    qkf_arith_node_free(&node->link) ;
}


bool qkf_logical_node_init(qkf_logical_node_t * node) 
{
    if((node == NULL) || (qkf_expr_node_init(&node->link) == false))
        return false ;

    node->opera = NULL ;
    node->link.type = kExprNodeLogical ;
    node->link.free = qkf_logical_node_free ;
    return true ;
}

bool qkf_logical_node_free(qkf_expr_node_t * node) 
{
    if(node == NULL || node->type != kExprNodeLogical)
        return false ;

    qkf_logical_node_t * logical = (qkf_logical_node_t *)node ;
    if(logical->opera != NULL)
    {
        ::qkf_free(kDefaultMMgr , logical->opera) ;
        logical->opera = NULL ;
    }

    return qkf_expr_node_free(node) ;
}

void qkf_logical_node_final(qkf_logical_node_t * node) 
{
    if(node == NULL)
        return ;

    qkf_expr_node_final(&node->link) ;
}


bool qkf_deguacus_node_init(qkf_deguacus_node_t * node) 
{
    if((node == NULL) || (qkf_expr_node_init(&node->link) == false))
        return false ;

    node->opera = NULL ;
    node->link.type = kExprNodeDeguacus ;
    node->link.free = qkf_deguacus_node_free ;
    node->link.scan = qkf_deguacus_node_scan ;
    return true ;
}

bool qkf_deguacus_node_free(qkf_expr_node_t * node) 
{
    if(node == NULL || node->type != kExprNodeDeguacus)
        return false ;

    qkf_deguacus_node_t * deguacus = (qkf_deguacus_node_t *)node ;
    if(deguacus->opera != NULL)
    {
        ::qkf_free(kDefaultMMgr , deguacus->opera) ;
        deguacus->opera = NULL ;
    }

    return qkf_expr_node_free(node) ;
}

int  qkf_deguacus_node_scan(qkf_expr_node_t * node , const char * str , int len) 
{

}

void qkf_deguacus_node_final(qkf_deguacus_node_t * node) 
{
    if(node == NULL || node->link.type != kExprNodeDeguacus)
        return ;

    qkf_deguacus_node_free(&node->link) ;
}


int  qkf_deguacus_node_scan_cont(const char * str , int slen , const char *&dstr , int&dlen) 
{
    int offset = qkf_expr_skip_space(str , slen) ;
    const char * cstr = str + offset ;
    int csize = slen - offset ;

    if(cstr[0] != '(')
        return 0 ;

    ++offset ;
    ++cstr ;
    --csize ;

    char ch = 0 ;
    int size = 0 ;
    while((size < csize) && (ch = cstr[size]) != ')')
        ++size ;

    if(size == csize)
        return 0 ;

    dstr = cstr ;
    dlen = size ;

    return (offset + size + 1) ;
}

int  qkf_deguacus_node_parse_cont(qkf_deguacus_node_t * node , const char * str , int slen) 
{
    if(node == NULL || str == NULL || slen <= 0)
        return -1 ;

    int offset = qkf_expr_skip_space(str , slen) ;
    const char * cstr = str + offset ;
    int clen = slen - offset ;

    int size = 0 ;
    while(size < clen)
    {
        //第一个只会是关系表达式
        const char * astr = cstr + size ;
        int alen = clen - size ;

        qkf_arith_node_t * arith = (qkf_arith_node_t *)::qkf_malloc(kDefaultMMgr , sizeof(qkf_arith_node_t)) ;
        if(arith == NULL)
            return -1 ;
        if(qkf_arith_node_init(arith) == false)
        {
            ::qkf_free(kDefaultMMgr , arith) ;
            return -1 ;
        }

        int asize = qkf_arith_node_scan(&arith->link , astr , alen) ;
        if(asize <= 0)
        {
            qkf_arith_node_final(arith) ;
            ::qkf_free(kDefaultMMgr , arith) ;
            return 0 ;
        }
        if(qkf_deguacus_node_add_arith(node , arith) == false)
        {
            qkf_arith_node_final(arith) ;
            qkf_free(kDefaultMMgr , arith) ;
            return 0 ;
        }
        size += asize ;

        //第二个可能是逻辑符号或者没有
        int left = clen - size ;
        if(left <= 0)
            break ;

        const char *lstr = cstr + size , *gstr = NULL ;
        int glen = 0 ;
        int lsize = qkf_expr_scan_logical(lstr , left , gstr , glen) ;
        if(lsize == 0)
            break ;

        qkf_logical_node_t * logical = (qkf_logical_node_t *)::qkf_malloc(kDefaultMMgr , sizeof(qkf_logical_node_t)) ;
        if(logical == NULL)
            return -1 ;
        if(qkf_logical_node_init(logical) == false)
        {
            ::qkf_free(kDefaultMMgr , logical) ;
            return -1 ;
        }
        logical->opera = ::qkf_strdup(gstr , glen) ;

        if(qkf_deguacus_node_add_logical(node , logical) == false)
        {
            qkf_logical_node_final(logical) ;
            ::qkf_free(kDefaultMMgr , logical) ;
            return 0 ;
        }

        size += lsize ;
    }

    if(size <= 0)
        return 0 ;

    return offset + size ;
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




