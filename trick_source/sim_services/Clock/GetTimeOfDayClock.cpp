/*
PURPOSE:
    ( gettimeofday clock )
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime.))
     ((Robert W. Bailey) (LinCom) (7/22/92) (--) (--))
     ((Eddie J. Paddock) (MDSSC) (Oct 1992) (--) (Realtime.)))
     ((Alex Lin) (NASA) (April 2009) (--) (c++ port)))
*/

#ifdef __APPLE__
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "trick/GetTimeOfDayClock.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"

/**
@details
-# Calls the base Clock constructor
*/
Trick::GetTimeOfDayClock::GetTimeOfDayClock() : Clock(1000000, "GetTimeOfDay - CLOCK_REALTIME")
#if ( __linux__ )
 , clk_id(CLOCK_REALTIME)
#endif
 { }

/**
@details
-# This function is empty
*/
Trick::GetTimeOfDayClock::~GetTimeOfDayClock() { }

/**
@details
-# Set the global "the_clock" pointer to this instance
*/
int Trick::GetTimeOfDayClock::clock_init() {
    set_global_clock() ;
    return 0 ;
}

/**
@details
-# Call the system clock_gettime to get the current real time.
-# Return the current real time as a count of microseconds
*/
long long Trick::GetTimeOfDayClock::wall_clock_time() {
#if ( __linux__ )
    struct timespec tp ;
    clock_gettime( clk_id, &tp ) ;
    return (long long)tp.tv_sec * 1000000LL + (long long)((tp.tv_nsec) / 1000)  ;
#else
    struct timeval tp ;
    gettimeofday(&tp,(struct timezone *)NULL) ;
    return (long long)tp.tv_sec * 1000000LL + tp.tv_usec ;
#endif
}

/**
@details
-# This function is empty
*/
int Trick::GetTimeOfDayClock::clock_stop() {
    return 0 ;
}

void Trick::GetTimeOfDayClock::set_clock_ID( int id ) {
#if ( __linux__ )
    clk_id = (clockid_t)id ;
    switch ( clk_id ) {
        case CLOCK_REALTIME: name = "GetTimeOfDay - CLOCK_REALTIME" ; break ;
        case CLOCK_MONOTONIC: name = "GetTimeOfDay - CLOCK_MONOTONIC" ; break ;
        case CLOCK_MONOTONIC_RAW: name = "GetTimeOfDay - CLOCK_MONOTONIC_RAW" ; break ;
        default: name = "GetTimeOfDay - other" ; break ;
    }
#else
    message_publish(MSG_ERROR, "set_clock_ID has no effect\n");
#endif
}

int Trick::GetTimeOfDayClock::get_clock_ID() {
#if ( __linux__ )
    return clk_id ;
#else
    message_publish(MSG_ERROR, "get_clock_ID not implemented\n");
    return -1 ;
#endif
}

