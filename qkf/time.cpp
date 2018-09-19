
#include "qkf/time.h"
#include <string.h>

int64_t  qkf_now_usec()
{
    struct timeval tv ;
    ::gettimeofday(&tv , NULL) ;

    int64_t ts = tv.tv_sec ;
    ts = ts * 1000000 + tv.tv_usec ;
    return ts ;
}

int64_t  qkf_now_msec() 
{
    struct timeval tv ;
    ::gettimeofday(&tv , NULL) ;

    int64_t ts = tv.tv_sec ;
    ts = ts * 1000 + tv.tv_usec / 1000;
    return ts ;
}

bool  qkf_next_nsec(struct timespec * ts , int64_t nsec) 
{
    if(ts == NULL || nsec <= 0)
        return false;

    struct timeval tv;
    ::gettimeofday(&tv , NULL) ;

    int64_t ns = tv.tv_usec ;
    ns = ns * 1000 + nsec;

    int64_t sec = 0  ;
    if(ns >= 1000000000)
    {
        sec = ns / 1000000000 ;
        nsec = ns % 1000000000 ;
    }
    else
        nsec = ns ;

    ts->tv_sec = tv.tv_sec + sec ;
    ts->tv_nsec = (long)nsec ;
    return true ;
}

bool qkf_time_to_date(time_t t , date_t * date)
{
    if(date == NULL)
        return false ;

    if(t == 0)
        t = ::time(NULL) ;

    struct tm tm ;
    if(::localtime_r(&t , &tm) == NULL)
        return false ;

    date->year = tm.tm_year + 1900 ;
    date->month = tm.tm_mon + 1 ;
    date->day = tm.tm_mday ;
    date->wday = tm.tm_wday ;
    if(date->wday == 0)
        date->wday = 7 ;

    return true ;
}

bool qkf_time_from_date(time_t * t , const date_t * date)
{
    if(t == NULL || date == NULL)
        return false ;

    struct tm tm ;
    ::memset(&tm , 0 , sizeof(tm)) ;

    tm.tm_year = date->year - 1900 ;
	tm.tm_mon = date->month - 1 ;
	tm.tm_mday = date->day ;

	tm.tm_hour = 0 ;
	tm.tm_min = 0 ;
	tm.tm_sec = 0 ;	

	*t = ::mktime(&tm) ;
    return true ;
}

bool qkf_parse_time(const char * str , int&year , int& month , int& day , int& hour , int& minute , int& second , int& msec) 
{
	year = month = day = hour = minute = second = msec = 0 ;
	if(str == NULL)
		return false ;

	const char * pchar = str ;
	int stage = 0 ;
	char ch = 0 , buffer[32] = {'\0'};
	int buflen = 0 ;
	do{
		ch = pchar[0] ;

		if(stage >= 0 && stage <= 2)
		{
			//日期，'-','/'以及最后的空格作为分隔符，如果没有分隔符，按长度分隔
			bool finish = false , seperate = false;
			if(ch >= '0' && ch <= '9')
			{
				buffer[buflen++] = ch ;
			}
			else
			{
				seperate = finish = true ;		//有分隔符，那么同时表示结束
			}

			if(finish == false)
			{
				if(stage == 0)
				{					
					if(buflen >= 5) 
					{
						finish = true ; 
						buflen = 4 ;
					}
				}
				else if(stage == 1 || stage == 2)
				{
					if(buflen >= 3) 
					{
						finish = true ; 
						buflen = 2 ;
					}
				}				
			}

			if(finish == true)
			{
				buffer[buflen] = '\0' ;
				switch(stage)
				{
				case 0 : year = atoi(buffer) ; break ;
				case 1 : month = atoi(buffer) ;  break ;
				case 2 : day = atoi(buffer) ; break ;
				default :break ;
				}

				buflen = 0 ;
				if(seperate == false)
				{
					buffer[buflen++] = ch ;
				}
				stage++ ;				
			}

		}
		else if(stage >= 3 && stage <= 6)
		{
			//时间，必须以":"分隔，中间可能没有0
			if(ch >= '0' && ch <= '9')
			{
				buffer[buflen++] = ch ;
			}
			else if(ch == ':' || ch == '.' || ch == '\0')
			{
				buffer[buflen] = '\0' ;
				switch(stage)
				{
				case 3 : hour = atoi(buffer) ; break ;
				case 4 : minute = atoi(buffer) ; break ;
				case 5 : second  = atoi(buffer) ; break ;
				case 6 : msec = atoi(buffer) ; break ;
				default : break ;
				}
				
				buflen = 0 ;
				stage++ ;
			}
		}

		pchar ++ ;	
		
	} while(ch != 0) ;

	return true ;	
}

time_t qkf_make_time(int year , int month , int day , int hour , int minute , int second)
{
    struct tm tm ;
    memset(&tm , 0 , sizeof(tm)) ;

    tm.tm_year = year - 1900 ;
    tm.tm_mon = month - 1 ;
    tm.tm_mday = day ;

    tm.tm_hour = hour ;
    tm.tm_min = minute ;
    tm.tm_sec = second ;	

    return ::mktime(&tm) ;
}

bool qkf_str_to_time(const char * str , time_t&  t , int& msec)
{
    int year , month , day , hour , minute , second ;
    if(str == NULL || qkf_parse_time(str , year , month , day , hour , minute , second , msec) == false)
	    return false ;

    t = qkf_make_time(year , month , day , hour , minute , second) ;	
    return true ;

}


