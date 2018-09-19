
#ifndef __QKF_TYTILS_H
#define __QKF_TYTILS_H 1

#include "qkf/compile.h"
#include <stdint.h>

__BEGIN_DECLS

//各种类型的工具集

QKFAPI bool qkf_get_bitmap(uint8_t val , int index) ;
QKFAPI void qkf_set_bitmap(uint8_t& val , int index , bool flag) ;

QKFAPI int qkf_int32_precision(int32_t val) ;
QKFAPI int qkf_int64_precision(int64_t val) ;


static const int kFloatPrecision    =   7 ;
static const int kDoublePrecision    =   16 ;

QKFAPI bool qkf_float_equal(float f1 , float f2) ;
QKFAPI bool qkf_double_equal(double d1 , double d2) ;

//double和float由于精度不同，从float转换成double会产生精度之外的垃圾数字
QKFAPI double qkf_float_to_double(float value) ;

QKFAPI const char * qkf_str_to_int32(const char * str , int size , int32_t& value) ;
QKFAPI const char * qkf_str_to_int64(const char * str , int size , int64_t& value) ;

__END_DECLS

#endif  /** __QKF_TYTILS_H */
