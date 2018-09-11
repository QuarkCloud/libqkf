
#ifndef __QKF_TIME_H
#define __QKF_TIME_H 1

#include "qkf/compile.h"
#include <stdint.h>
#include <sys/time.h>

__BEGIN_DECLS

QKFAPI int64_t  qkf_now_usec() ;
QKFAPI int64_t  qkf_now_msec() ;

QKFAPI bool  qkf_next_nsec(struct timespec * ts , int64_t nsec) ;


__END_DECLS

#endif  /** __QKF_TIME_H */
