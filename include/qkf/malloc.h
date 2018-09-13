
#ifndef __QKF_MALLOC_H
#define __QKF_MALLOC_H 1

#include "qkf/compile.h"

__BEGIN_DECLS

static const int kDefaultMMgr = 0 ;

QKFAPI int qkf_mmgr_create() ;
QKFAPI void qkf_mmrr_destory(int mmgr) ;

QKFAPI void * qkf_malloc(int mmgr , size_t size) ;
QKFAPI void qkf_free(int mmgr , void * addr) ;

QKFAPI char * qkf_strdup(const char * str , int size) ;

__END_DECLS

#endif  /** __QKF_MALLOC_H */
