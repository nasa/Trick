
#include <iostream>

#include "trick/Executive.hh"
#include "trick/release.h"
#include "trick/clock_proto.h"

/**
@design
-# Loop through all threads.
   -# Wait for thread to finish if the thread is PROCESS_TYPE_SCHEDULED
*/
int Trick::Executive::scheduled_thread_sync() {

    unsigned int ii ;
    long long time_before_rt_nap, time_after_rt_nap ;
    /* Wait for synchronous threads to finish before testing for adjusting time_tics */
    for (ii = 1; ii < threads.size() ; ii++) {
        Threads * curr_thread = threads[ii] ;
        if ( curr_thread->enabled and curr_thread->process_type == PROCESS_TYPE_SCHEDULED) {
            while (curr_thread->child_complete == false ) {
                if (rt_nap == true) {
                    time_before_rt_nap = clock_wall_time() ;
                    RELEASE();
                    time_after_rt_nap = clock_wall_time() ;
                    set_rt_nap_stats(time_before_rt_nap, time_after_rt_nap) ;
                }
            }
        }
    }

    return 0 ;
}

