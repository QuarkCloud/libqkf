
#ifndef __QKF_TIME_H
#define __QKF_TIME_H 1

#include "qkf/compile.h"
#include <stdint.h>
#include <sys/time.h>

__BEGIN_DECLS

/*
	由于是同一天，所以没有必要在时间中保存日期的信息，将日期单独是个明智的处理。
	对于时间来说，每天3600*24 = 86400秒，是个常量。

	这里年月日，都是表示本地时间。

	关于time.h中几个重要函数解释。
	time(NULL) ；返回的值，同本地时间到EPOCH的秒数相同
	mktime(struct tm)；也是基于本地时间进行计算
	除非特别指出，这里的时间秒数，都是基于本地时间

    32位的linux的time_t是32位，windows和64位linux都是64位。请注意在32位linux下的溢出问题。    
*/

QKFAPI int64_t  qkf_now_usec() ;
QKFAPI int64_t  qkf_now_msec() ;

QKFAPI bool  qkf_next_nsec(struct timespec * ts , int64_t nsec) ;

//可读性比较强的日期
typedef struct __st_date{
    int year ;              //年    2018
    int month ;             //月    1...12
    int day ;               //日    1...31
    int wday ;              //周几  1...7
}date_t ;

QKFAPI  bool qkf_time_to_date(time_t t , date_t * date) ;
QKFAPI  bool qkf_time_from_date(time_t * t , const date_t * date) ;

/*
    解析日期时间。支持格式为"YYYY-MM-DD HH:MM:SS.mmm"或者"YYYYMMDD HH:MM:SS.mmm" 或者"YYYY/MM/DD HH:MM:SS.mmm"
    如果日期有分隔符，那么可以不补充0
*/
QKFAPI bool qkf_parse_time(const char * str , int&year , int& month , int& day , int& hour , int& minute , int& second , int& msec) ;

__END_DECLS

#endif  /** __QKF_TIME_H */
