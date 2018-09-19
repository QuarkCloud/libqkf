
#include "qkf/tytils.h"

static const int kByteSize = 8 ;
static const uint8_t __qkf_bitmaps__[kByteSize] = {1 , 2 , 4 , 8 , 16 , 32 , 64 , 128} ;

bool qkf_get_bitmap(uint8_t val , int index)
{
    if(index < 0 || index >= kByteSize)
        return false ;

    return (val & __qkf_bitmaps__[index]) != 0 ;
}

void qkf_set_bitmap(uint8_t& val , int index , bool flag)
{
    if(index < 0 || index >= kByteSize)
        return ;

    if(((val & __qkf_bitmaps__[index]) != 0) == flag)
        return ;

    val = val ^ (__qkf_bitmaps__[index]) ;
}

static const int kInt32PreMax = 8 ;
static const int __qkf_int32_pre_max__[kInt32PreMax] = {
    10 , 
    100 , 
    1000 , 
    10000 ,
    100000 ,
    1000000 ,
    10000000 ,
    100000000 
} ;

int qkf_int32_precision(int32_t val)
{
    if(val < 0)
        val = 0 - val ;
    int low = 0 , high = kInt32PreMax ;
    while(true)
    {
        int mid = (low + high) / 2 ;
        if(val < __qkf_int32_pre_max__[mid])
            high = mid - 1 ;
        else
            low = mid + 1 ;

        if(low <= high)
            continue ;
        return mid ;
    }
    return 0 ;
}

static const int kInt64PreMax = 19 ;
static const int64_t __qkf_int64_pre_max__[kInt64PreMax] = {
    10 , 
    100 , 
    1000 , 
    10000 ,
    100000 ,
    1000000 ,
    10000000 ,
    100000000 ,
    1000000000 ,
    10000000000 ,
    100000000000 ,
    1000000000000 ,
    10000000000000 ,
    100000000000000 ,
    1000000000000000 ,
    10000000000000000 ,
    100000000000000000 ,
    1000000000000000000 ,
    10000000000000000000
} ;

int qkf_int64_precision(int64_t val)
{
    if(val < 0)
        val = 0 - val ;
    int low = 0 , high = kInt64PreMax ;
    while(true)
    {
        int mid = (low + high) / 2 ;
        if(val < __qkf_int64_pre_max__[mid])
            high = mid - 1 ;
        else
            low = mid + 1 ;

        if(low <= high)
            continue ;
        return mid ;
    }
    return 0 ;
}

bool qkf_float_equal(float f1 , float f2)
{
    int i1 = (int)f1 , i2 = (int)f2;
    if(i1 != i2)
        return false ;

    int ipr = qkf_int32_precision(i1) ;
    int fpr = kFloatPrecision - ipr ;
    int multi = __qkf_int32_pre_max__[fpr] ;
    int fv1 = (int)((f1 - i1) * multi) ;
    int fv2 = (int)((f2 - i2) * multi) ;

    return (fv1 == fv2) ;
}

bool qkf_double_equal(double d1 , double d2)
{
    int64_t i1 = (int64_t)d1 , i2 = (int64_t)d2;
    if(i1 != i2)
        return false ;

    int ipr = qkf_int64_precision(i1) ;
    int fpr = kDoublePrecision - ipr ;
    int64_t multi = __qkf_int64_pre_max__[fpr] ;
    int64_t fv1 = (int64_t)((d1 - i1) * multi) ;
    int64_t fv2 = (int64_t)((d2 - i2) * multi) ;

    return (fv1 == fv2) ;
}

double qkf_float_to_double(float value) 
{
    int32_t i32 = (int32_t)value ;
    int ipr = qkf_int32_precision(i32) ;
    int fpr = kFloatPrecision - ipr ;

    int64_t i64 = (int64_t)(value * __qkf_int64_pre_max__[fpr]) ;
    double f64 = (double)i64 ;
    return (f64 / __qkf_int64_pre_max__[fpr]) ;       
}

const char * qkf_str_to_int32(const char * str , int size , int32_t& value)
{
    int64_t i64 = 0 ;
    const char * result = qkf_str_to_int64(str , size , i64) ;
    if(result == NULL)
        return NULL ;
    if(i64 < INT32_MIN || i64 > INT32_MAX)
        return NULL ;

    value = (int32_t)i64 ;
    return result ;
}

const char * qkf_str_to_int64(const char * str , int size , int64_t& value)
{
	bool finish = false ;
	char buf[32] ;
	int  index = 0 ;
	value = 0 ;

	if(str == NULL)
		return NULL ;

	char ch = 0 ;
	while(( ch = str[0]) != '\0' && finish == false)
	{
		if(ch >= '0' && ch <= '9')
		{
			buf[index++] = ch ;
			if(index >= size)
			{
				finish = true ;
			}
		}
		else
		{
			//保证是连续的几个数字作为一个值
			if(index != 0)
			{
				finish = true ;
			}

			if(finish)
			{
				buf[index] = '\0' ;
                char *last = NULL ;
                long long int lli = ::strtoll(buf , &last , 10) ;
                value = (int64_t)lli ;
			}
		}

		str++ ;
	
	}//end while ...

	return str ;	
}

