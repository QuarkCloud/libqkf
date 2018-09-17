
#ifndef __QKF_TIME_H
#define __QKF_TIME_H 1

#include "qkf/compile.h"
#include <stdint.h>
#include <sys/time.h>

__BEGIN_DECLS

/*
	������ͬһ�죬����û�б�Ҫ��ʱ���б������ڵ���Ϣ�������ڵ����Ǹ����ǵĴ���
	����ʱ����˵��ÿ��3600*24 = 86400�룬�Ǹ�������

	���������գ����Ǳ�ʾ����ʱ�䡣

	����time.h�м�����Ҫ�������͡�
	time(NULL) �����ص�ֵ��ͬ����ʱ�䵽EPOCH��������ͬ
	mktime(struct tm)��Ҳ�ǻ��ڱ���ʱ����м���
	�����ر�ָ���������ʱ�����������ǻ��ڱ���ʱ��

    32λ��linux��time_t��32λ��windows��64λlinux����64λ����ע����32λlinux�µ�������⡣    
*/

QKFAPI int64_t  qkf_now_usec() ;
QKFAPI int64_t  qkf_now_msec() ;

QKFAPI bool  qkf_next_nsec(struct timespec * ts , int64_t nsec) ;

//�ɶ��ԱȽ�ǿ������
typedef struct __st_date{
    int year ;              //��    2018
    int month ;             //��    1...12
    int day ;               //��    1...31
    int wday ;              //�ܼ�  1...7
}date_t ;

QKFAPI  bool qkf_time_to_date(time_t t , date_t * date) ;
QKFAPI  bool qkf_time_from_date(time_t * t , const date_t * date) ;

/*
    ��������ʱ�䡣֧�ָ�ʽΪ"YYYY-MM-DD HH:MM:SS.mmm"����"YYYYMMDD HH:MM:SS.mmm" ����"YYYY/MM/DD HH:MM:SS.mmm"
    ��������зָ�������ô���Բ�����0
*/
QKFAPI bool qkf_parse_time(const char * str , int&year , int& month , int& day , int& hour , int& minute , int& second , int& msec) ;

__END_DECLS

#endif  /** __QKF_TIME_H */
