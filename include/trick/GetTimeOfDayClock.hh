/*
PURPOSE:
    ( gettimeofday Clock )
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime.))
     ((Robert W. Bailey) (LinCom) (7/22/92) (--) (--))
     ((Eddie J. Paddock) (MDSSC) (Oct 1992) (--) (Realtime.))
     ((Alex Lin) (NASA) (April 2009) (--) (port to c++)))

*/

#ifndef GETTIMEOFDAYCLOCK_HH
#define GETTIMEOFDAYCLOCK_HH

#ifdef __linux__
#include <time.h>
#endif

#include "trick/Clock.hh"

namespace Trick {

    /* enum ClockID remains until we are using python 3.x where
       these values are available in the time module. Delete this
       when everybody is using python 3.
     */
#ifdef __linux__
    enum ClockID {
       TRICK_CLOCK_REALTIME = CLOCK_REALTIME ,
       TRICK_CLOCK_MONOTONIC = CLOCK_MONOTONIC ,
       TRICK_CLOCK_MONOTONIC_RAW = CLOCK_MONOTONIC_RAW
    } ;
#endif

    class GetTimeOfDayClock : public Clock {

        public:

            GetTimeOfDayClock() ;
            ~GetTimeOfDayClock() ;

            /** @copybrief Trick::Clock::clock_init() */
            virtual int clock_init() ;

            /** @copybrief Trick::Clock::wall_clock_time() */
            virtual long long wall_clock_time() ;

            /** @copybrief Trick::Clock::clock_stop() */
            virtual int clock_stop() ;

            /** Sets the clock ID (system clock type), only for linux */
            void set_clock_ID( int id ) ;

            /** Gets the current clock ID (system clock type), only for linux */
            int get_clock_ID() ;

        protected:
#ifdef __linux__
            clockid_t clk_id ; // trick_io(**)
#endif
    } ;

}

#endif
