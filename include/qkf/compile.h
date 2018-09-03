
#ifndef __QKF_COMPILE_H
#define __QKF_COMPILE_H 1

#include <stdlib.h>

#if defined(__QUARK_LIBC__)
    #ifdef QKF_EXPORTS
	    #define QKFAPI QUARK_EXPORT
    #else
        #define QKFAPI QUARK_IMPORT
    #endif
#else
    #define QKFAPI
#endif

#endif  /** __QKF_COMPILE_H */
