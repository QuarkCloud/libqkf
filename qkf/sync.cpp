
#include "qkf/sync.h"

#ifdef __QUARK_LIBC__
    #pragma warning( push )
    #pragma warning( disable : 4793 )

    void  qkf_memory_barrier()
    {
        long Barrier;
        __asm {
            xchg Barrier, eax
        }
    }

    #pragma warning( pop )
#else

    void  qkf_memory_barrier()
    {
        __asm__ __volatile__ ("mfence":::"memory") ;
    }   

#endif

