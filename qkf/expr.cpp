
#include "qkf/expr.h"
#include "qkf/malloc.h"
#include <string.h>

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


qkf_arith_node_t * qkf_arith_node_new()
{

    qkf_arith_node_t * arith = (qkf_arith_node_t *)::qkf_malloc(kDefaultMMgr , sizeof(qkf_arith_node_t)) ;
    if((arith != NULL) && (qkf_arith_node_init(arith) == true))
        return arith ;

    if(arith != NULL)
        ::qkf_free(kDefaultMMgr , arith) ;

    return NULL ;
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


qkf_logical_node_t * qkf_logical_node_new() 
{
    qkf_logical_node_t * logical = (qkf_logical_node_t *)::qkf_malloc(kDefaultMMgr , sizeof(qkf_logical_node_t)) ;
    if((logical != NULL) && (qkf_logical_node_init(logical) == true))
        return logical ;

    if(logical != NULL)
        ::qkf_free(kDefaultMMgr , logical) ;

    return NULL ;
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

qkf_deguacus_node_t * qkf_deguacus_node_new() 
{
    qkf_deguacus_node_t * deguacus = (qkf_deguacus_node_t *)::qkf_malloc(kDefaultMMgr , sizeof(qkf_deguacus_node_t)) ;
    if((deguacus != NULL) && (qkf_deguacus_node_init(deguacus) == true))
        return deguacus ;

    if(deguacus != NULL)
        ::qkf_free(kDefaultMMgr , deguacus) ;

    return NULL ;
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

int  qkf_deguacus_node_scan(qkf_expr_node_t * node , const char * str , int slen) 
{
    if(node == NULL || str == NULL || slen <= 0 || node->type != kExprNodeDeguacus)
        return -1 ;

    const char * dstr = NULL ;
    int dlen = 0 ;

    int offset = qkf_deguacus_node_scan_cont(str , slen , dstr , dlen) ;
    if(offset <= 0)
        return 0 ;

    qkf_deguacus_node_t * deguacus = (qkf_deguacus_node_t *)node ;
    deguacus->opera = qkf_strdup("()" , 2) ;

    int clen = qkf_deguacus_node_parse_cont(deguacus , dstr , dlen) ;
    if(clen <= 0)
        return 0 ;    

    return offset ;
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

        qkf_arith_node_t * arith = qkf_arith_node_new() ;
        if(arith == NULL)
            return -1 ;

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

        qkf_logical_node_t * logical = qkf_logical_node_new() ;
        if(logical == NULL)
            return -1 ;

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
    if(node == NULL || arith == NULL)
        return -1 ;

    qkf_expr_node_t * right = node->link.right ;
    if(right != NULL)
        return -1 ;
    qkf_expr_node_t * left = node->link.left ;
    if(left == NULL)
    {
        arith->link.parent = &node->link ;
        node->link.left = &arith->link ;
        return 0 ;
    }

    if(left->type != kExprNodeLogical || left->left == NULL)
        return -1 ;

    left->right = &arith->link ;
    arith->link.parent = left ;
    return true ;
}

int  qkf_deguacus_node_add_logical(qkf_deguacus_node_t * node , qkf_logical_node_t * logical) 
{
    if(node == NULL || logical == NULL)
        return -1 ;

    qkf_expr_node_t * left = node->link.left ;
    if(left == NULL || node->link.right != NULL)
        return -1 ;

    left->parent = &logical->link ;
    logical->link.parent = &node->link ;
    logical->link.left = left ;
    node->link.left = &logical->link ;
    
    return true ;
}



bool qkf_expr_init(qkf_expr_t * expr) 
{
    if(expr == NULL)
        return false ;

    expr->root = NULL ;
    return true ;
}

void qkf_expr_final(qkf_expr_t * expr) 
{
    if(expr == NULL)
        return ;

    qkf_expr_clear(expr) ;
}

void qkf_expr_clear(qkf_expr_t * expr) 
{
    if(expr == NULL || expr->root == NULL)
        return ;

    qkf_expr_node_final(expr->root) ;
    expr->root = NULL ;
}

bool qkf_expr_parse(qkf_expr_t * expr , const char * str) 
{
    if(expr == NULL || str == NULL)
        return false ;

    qkf_expr_clear(expr) ;
    int offset = 0 , elen = 0 ;
    if(str != NULL)
        elen = ::strlen(str) ;

    while(offset < elen)
    {
        int space = qkf_expr_skip_space(str + offset , elen - offset) ;
        if(space > 0)
        {
            offset += space ;
            continue ;
        }

        const char& ch = str[offset] ;
        if(ch == '(')
        {
            qkf_deguacus_node_t * deguacus = qkf_deguacus_node_new() ;
            if(deguacus == NULL)
                return false ;

            int length = qkf_deguacus_node_scan(&deguacus->link , str + offset , elen - offset) ;
            if(length <= 0)
            {
                qkf_deguacus_node_final(deguacus) ;
                return false ;
            }

            if(qkf_expr_add_node(expr , &deguacus->link) == false)
            {
                qkf_deguacus_node_final(deguacus) ;
                return false ;            
            }

            offset += length ;        
        }
        else if(ch == '\0')
        {
            break ;
        }
        else
        {
            const char * nstr = NULL ;
            int nlen = 0 ;
            int length = qkf_expr_scan_name(str + offset , elen - offset , nstr , nlen) ;
            if(length <= 0)
                return false ;

            char lstr[32] ;
            int llen = (int)sizeof(lstr) ;
            if(qkf_expr_check_logical(nstr , nlen , lstr , llen) == true)
            {
                qkf_logical_node_t * logical = qkf_logical_node_new() ;
                if(logical == NULL)
                    return false ;

                logical->opera = qkf_strdup(lstr , llen) ;
                if(qkf_expr_add_node(expr , &logical->link) == false)
                {
                    qkf_logical_node_final(logical) ;
                    ::qkf_free(kDefaultMMgr , logical) ;
                    return false ;
                }            
            }
            else
            {
                qkf_arith_node_t * arith = qkf_arith_node_new() ;
                if(arith == NULL)
                    return false ;

                length = qkf_arith_node_scan(&arith->link , str + offset , elen - offset) ;
                if(length <= 0 || qkf_expr_add_node(expr , &arith->link) == false)
                {
                    qkf_arith_node_final(arith) ;
                    qkf_free(kDefaultMMgr , arith) ;
                    return false ;
                }            
            }

            offset += length ;
        }//end if(ch == ...)
    }//end while(true)

    return true ;
}

const qkf_expr_node_t * qkf_expr_first(qkf_expr_t * expr) 
{
    if(expr == NULL || expr->root == NULL)
        return NULL ;

    return qkf_expr_left(expr , expr->root) ;
}

const qkf_expr_node_t * qkf_expr_next(qkf_expr_t * expr , const qkf_expr_node_t * node) 
{
    if(expr == NULL || node == NULL)
        return NULL ;

    const qkf_expr_node_t * parent = node->parent ;
    if(parent == NULL)
    {
        //没有父亲节点，就是根节点，直接返回。
        return NULL ;    
    }

    const qkf_expr_node_t * left = parent->left ;
    const qkf_expr_node_t * right = parent->right ;

    if(left == node)
    {
        //如果当前节点为父节点的左节点，那么访问父节点的右节点
        if(right == NULL)
            return parent ;
        return qkf_expr_left(expr , right) ;        
    }

    if(right == node)
        return parent ;
    else
        return NULL ;
}

const qkf_expr_node_t * qkf_expr_left(qkf_expr_t * expr , const qkf_expr_node_t * node) 
{
    if(expr == NULL || node == NULL)
        return NULL ;

    const qkf_expr_node_t * left = NULL , *cur = node ;
    while((left = cur->left) != NULL)
        cur = left ;

    return cur ;
}


bool qkf_expr_add_node(qkf_expr_t * expr , qkf_expr_node_t * node) 
{
    if(expr == NULL || node == NULL)
        return false ;

    if(expr->root == NULL)
    {
        expr->root = node ;
        return true ;    
    }

    qkf_expr_node_t * root = expr->root ;
    if(root->type != kExprNodeLogical || root->left == NULL || root->right == NULL)
        return false ;

    node->parent = root ;
    expr->root->right = node ;
    return true ;
}

bool qkf_expr_add_logical(qkf_expr_t * expr , qkf_logical_node_t * logical) 
{
    if(expr == NULL || expr->root == NULL || logical == NULL)
        return false ;

    expr->root->parent = &logical->link ;
    logical->link.left = expr->root ;
    expr->root = &logical->link ;

    return true ;
}


int qkf_expr_skip_space(const char * str , int slen) 
{
    if(str == NULL || slen <= 0)
        return -1 ;

    int size = 0 ;
    char ch = 0 ;
    while((size < slen) && ((ch = str[size]) != 0))
    {
        if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            ++size ;
        else
            break ;
    }

    return  size ;
}
 

int qkf_expr_scan_name(const char * str , int slen , const char *& name , int& nlen) 
{
    if(str == NULL || slen <= 0)
        return -1 ;

    int offset = qkf_expr_skip_space(str , slen) ;
    const char * nstr = str + offset ;
    int size = 0 , left = slen - offset ;
    char ch = 0 ;
    bool first_char = false ;
    while((size < slen) && (ch = nstr[size]) != 0)
    {
        if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_'))
        {
            first_char = true ;
            ++size ;
            continue ;
        }
        else if((ch >= '0' && ch <= '9') && (first_char == true))
        {
            ++size ;
            continue ;
        }

        break ;        
    }

    if(size > 0)
    {
        name = nstr ;
        nlen = size ;
        return (offset + size) ;
    }
    return 0 ;
}

bool qkf_expr_check_logical(const char * str , int slen , char * upper , int ulen) 
{
    if(str == NULL || slen > 3)
        return false ;
    if(upper == NULL || ulen <= 0)
        return false ;

    char ustr[32] = {'\0'} ;
    int usize = 0 ;
    while((usize < slen) && (usize < (int)(sizeof(ustr) - 8)) && (usize < ulen))
    {
        char ch = str[usize] ;
        if(ch >= 'a' && ch <= 'z')
            ustr[usize] = ch - 'a' + 'A' ;
        else
            ustr[usize] = ch ;

        ++usize ;
    }
    ustr[usize] = '\0' ;

    if(strcmp(ustr , "AND") == 0 || strcmp(ustr , "OR") == 0)
    {
        ::memcpy(upper , ustr , usize) ;
        upper[usize] = '\0' ;
        return true ;
    }
    else
        return false ;
}

bool qkf_expr_check_digit(const char * str , int slen) 
{
    if(str == NULL || slen <= 0)
        return false ;

    int cidx = 0 ;
    while(cidx < slen)
    {
        char ch = str[cidx] ;
        if(ch < '0' || ch > '9')
            break ;

        ++cidx ;
    }

    return (cidx == slen) ;
}

int qkf_expr_scan_relation(const char * str , int slen , const char *& oper , int& olen) 
{
    if(str == NULL || slen <= 0)
        return -1 ;

    int offset = qkf_expr_skip_space(str , slen) ;
    int left = slen - offset ;
    if(left <= 0)
        return 0 ;

    olen = 0 ;
    const char * ostr = str + offset ;
    char ch1 = ostr[0] ;
    if(ch1 == '=')
        olen = 1 ;
    else if(ch1 == '<' || ch1 == '>')
    {
        olen = 1 ;
        if(left >= 2)
        {
            char ch2 = ostr[1] ;
            if(ch2 == '=')
                olen = 2 ;
        }
    }
    else if(ch1 == '!')
    {
        if(left >= 2 && ostr[1] == '=')
            olen = 2 ;
    }

    if(olen == 0)
        return 0 ;

    oper = ostr ;
    return (offset + olen) ;
}

int qkf_expr_scan_value(const char * str , int slen , const char *& value , int& vlen) 
{
    if(str == NULL || slen <= 0)
        return -1 ;

    int offset = qkf_expr_skip_space(str , slen) ;
    int left = slen - offset ;
    if(left <= 0)
        return 0 ;

    const char * vstr = str + offset ;
    int size = 0 , begin = 0 ;
    bool quoted = false ;
    while((size < left) && (quoted == false))
    {
        char ch = vstr[size] ;
        if(ch == '"')
        {
            //碰到引号，一直到另外一个引号出现
            if(size != 0)
                return 0 ;

            quoted = true ;
            ++vstr ;
            --left ;

            while(size < left)
            {
                char qch = vstr[size] ;
                if(qch == '"')
                    break ;
                else
                    ++size ;
            }

            if(size == left)
            {
                //没有找到双引号结束，则返回错误
                return 0 ;            
            }

            offset += 2 ;
            break ;        
        }

        if(ch == ' ')
            break ;
        else
            ++size ;        
    }

    if(size <= 0)
        return 0 ;

    value = vstr ;
    vlen = size ;

    return (offset + size) ;
}

int qkf_expr_scan_logical(const char * str , int slen , const char *& logical , int& llen) 
{
    if(str == NULL || slen <= 0)
        return -1 ;

    int offset = qkf_expr_skip_space(str , slen) ;
    const char * lstr = str + offset ;
    int left = slen - offset ;

    char ustr[32] ;
    int ulen = 0 , usize = (int)sizeof(ustr);
    while((ulen < left) && (ulen < usize))
    {
        char ch = lstr[ulen] ;
        if((ch >= 'a') && (ch <= 'z'))
            ustr[ulen] = ch - 'a' + 'A' ;
        else if((ch >= 'A') && (ch <= 'Z'))
            ustr[ulen] = ch ;
        else
            break ;

        ++ulen ;
    }

    if(ulen <= 0 || ulen > 3)
        return 0 ;

    ustr[ulen] = '\0' ;
    if(strcmp(ustr , "OR") == 0 || strcmp(ustr , "AND") == 0)
    {
        logical = lstr ;
        llen = ulen ;
        return (offset + ulen) ;
    }

    return 0 ;
}
