
#include "qkf/hash.h"

uint32_t hash_time33(const char * str)
{
    if(str == NULL)
        return 0 ;

    uint32_t hash = 0 ;
    const uint8_t * p = (const uint8_t *) str ;

    uint8_t ch ;
    while((ch = *p) != 0)
    {
        hash = (hash << 5) + hash + ch ;
        ++p ;
    }

    return hash ;
}
