
#include "qkf/time.h"

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
    ts->tv_nsec = nsec ;
    return true ;
}

