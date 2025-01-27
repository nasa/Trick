/*
   PURPOSE: (The Trick simulation child executive processing.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS: ((this is executive only from the child process forked by the executive))

   PROGRAMMERS:
      (((Robert W. Bailey) (LinCom) (October 1993) (--) (Realtime))
       ((Eddie J. Paddock) (MDSSC) (April 1992) (--) (Realtime)))
 */

#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>

#ifdef __linux__
#include <cxxabi.h>
#endif

#include "trick/Threads.hh"
#include "trick/release.h"
#include "trick/ExecutiveException.hh"
#include "trick/exec_proto.h"
#include "trick/TrickConstant.hh"
#include "trick/message_proto.h"


/**
@details
-# Wait for all job dependencies to complete.  Requirement  [@ref r_exec_thread_6]
-# Call the job.  Requirement  [@ref r_exec_periodic_0]
-# If the job is a system job, check to see if the next job call time is the lowest next time by
   calling Trick::ScheduledJobQueue::test_next_job_call_time(Trick::JobData *, long long)
-# Set the job complete flag
*/
static int call_next_job(Trick::JobData * curr_job, Trick::ScheduledJobQueue & job_queue, bool rt_nap, long long curr_time_tics) {

    Trick::JobData * depend_job ;
    unsigned int ii ;
    int ret = 0 ;

    //cout << "time = " << curr_time_tics << " " << curr_job->name << " job next = "
    //  << curr_job->next_tics << " id = " << curr_job->id << endl ;

    /* Wait for all jobs that the current job depends on to complete. */
    for ( ii = 0 ; ii < curr_job->depends.size() ; ii++ ) {
        depend_job = curr_job->depends[ii] ;
        while (! depend_job->complete) {
            if (rt_nap == true) {
                RELEASE();
            }
        }
    }

    /* Call the current scheduled job. */
    ret = curr_job->call() ;

    if ( ret != 0 ) {
        exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , "scheduled job did not return 0") ;
    }

    /* System jobs next call time are not set until after they run. We test their next job call time here. */
    if ( curr_job->system_job_class ) {
        job_queue.test_next_job_call_time(curr_job , curr_time_tics) ;
    }

    curr_job->complete = true ;

    return 0 ;
}

/**
@details
-# Block all signals to the child.
-# Set the thread cancel type to asynchronous to allow master to this child at any time.
-# Lock the go mutex so the master has to wait until this child is ready before staring execution.
-# Set thread priority and CPU affinity
-# The child enters an infinite loop
    -# Blocks on mutex or frame trigger until master signals to start processing
    -# Switch if the child is a synchronous thread
        -# For each scheduled jobs whose next call time is equal to the current simulation time [@ref ScheduledJobQueue]
            -# Call call_next_job(Trick::JobData * curr_job, Trick::ScheduledJobQueue & job_queue, bool rt_nap, long long curr_time_tics)
    -# Switch if the child is a asynchronous must finish thread
        -# Do while the job queue time is less than the time of the next AMF sync time.
            -# For each scheduled jobs whose next call time is equal to the current queue time
                -# Call call_next_job(Trick::JobData * curr_job, Trick::ScheduledJobQueue & job_queue, bool rt_nap, long long curr_time_tics)
    -# Switch if the child is a asynchronous thread
        -# For each scheduled jobs
            -# Call call_next_job(Trick::JobData * curr_job, Trick::ScheduledJobQueue & job_queue, bool rt_nap, long long curr_time_tics)
    -# Set the child complete flag
*/
void * Trick::Threads::thread_body() {

    /* Lock the go mutex so the master has to wait until this child is ready before staring execution. */
    trigger_container.getThreadTrigger()->init() ;

    /* signal the master that the child is ready and running */
    child_complete = true;
    running = true ;

    try {
        do {

            /* Block child on trigger until master signals. */
            trigger_container.getThreadTrigger()->wait() ;

            if ( enabled ) {

                switch ( process_type ) {
                    case PROCESS_TYPE_SCHEDULED:
                    /* Loop through all jobs currently scheduled to run at this simulation time step. */
                    job_queue.reset_curr_index() ;
                    job_queue.set_next_job_call_time(TRICK_MAX_LONG_LONG) ;
                    while ( (curr_job = job_queue.find_next_job( curr_time_tics )) != NULL ) {
                        call_next_job(curr_job, job_queue, rt_nap, curr_time_tics) ;
                    }
                    break ;

                    case PROCESS_TYPE_AMF_CHILD:
                    /* call the AMF top of frame jobs */
                    top_of_frame_queue.reset_curr_index() ;
                    while ( (curr_job = top_of_frame_queue.get_next_job()) != NULL ) {
                        int ret ;
                        ret = curr_job->call() ;
                        if ( ret != 0 ) {
                            exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , " top_of_frame job did not return 0") ;
                        }
                    }

                    /* Loop through all jobs currently up to the point of the next AMF frame sync */
                    do {
                        job_queue.reset_curr_index() ;
                        job_queue.set_next_job_call_time(amf_next_tics) ;
                        while ( (curr_job = job_queue.find_next_job( curr_time_tics )) != NULL ) {
                            call_next_job(curr_job, job_queue, rt_nap, curr_time_tics) ;
                        }
                        curr_time_tics = job_queue.get_next_job_call_time() ;
                    } while ( curr_time_tics < amf_next_tics ) ;

                    /* call the AMF end of frame jobs */
                    end_of_frame_queue.reset_curr_index() ;
                    while ( (curr_job = end_of_frame_queue.get_next_job()) != NULL ) {
                        int ret ;
                        ret = curr_job->call() ;
                        if ( ret != 0 ) {
                            exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , " end_of_frame job did not return 0") ;
                        }
                    }
                    break ;

                    case PROCESS_TYPE_ASYNC_CHILD:
                    /* Loop through all jobs once */
                    if ( amf_cycle_tics == 0 ) {
                        // Old behavior, run all jobs once and return.
                        job_queue.reset_curr_index() ;
                        job_queue.set_next_job_call_time(TRICK_MAX_LONG_LONG) ;
                        while ( (curr_job = job_queue.get_next_job()) != NULL ) {
                            call_next_job(curr_job, job_queue, rt_nap, curr_time_tics) ;
                        }
                    } else {

                        // catch up job next times to current frame.
                        job_queue.reset_curr_index() ;
                        while ( (curr_job = job_queue.get_next_job()) != NULL ) {
                            long long start_frame = amf_next_tics - amf_cycle_tics ;
                            while ( curr_job->next_tics < start_frame ) {
                                curr_job->next_tics += curr_job->cycle_tics ;
                            }
                        }

                        // New behavior, run a mini scheduler.
                        /* call the AMF top of frame jobs */
                        top_of_frame_queue.reset_curr_index() ;
                        while ( (curr_job = top_of_frame_queue.get_next_job()) != NULL ) {
                            int ret ;
                            ret = curr_job->call() ;
                            if ( ret != 0 ) {
                                exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , " top_of_frame job did not return 0") ;
                            }
                        }

                        /* Loop through all jobs currently up to the point of the next AMF frame sync */
                        do {
                            job_queue.reset_curr_index() ;
                            job_queue.set_next_job_call_time(amf_next_tics) ;
                            while ( (curr_job = job_queue.find_next_job( curr_time_tics )) != NULL ) {
                                call_next_job(curr_job, job_queue, rt_nap, curr_time_tics) ;
                            }
                            curr_time_tics = job_queue.get_next_job_call_time() ;
                        } while ( curr_time_tics < amf_next_tics ) ;

                        /* call the AMF end of frame jobs */
                        end_of_frame_queue.reset_curr_index() ;
                        while ( (curr_job = end_of_frame_queue.get_next_job()) != NULL ) {
                            int ret ;
                            ret = curr_job->call() ;
                            if ( ret != 0 ) {
                                exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , " end_of_frame job did not return 0") ;
                            }
                        }
                    }
                    break ;

                    default:
                    break ;
                }
            }

            /* After all jobs have completed, set the child_complete flag to true. */
            child_complete = true;

        } while (1);
    } catch (Trick::ExecutiveException & ex ) {
        fprintf(stderr, "\nCHILD THREAD %d TERMINATED with exec_terminate\n  ROUTINE: %s\n  DIAGNOSTIC: %s\n"
         "  THREAD STOP TIME: %f\n" ,
         thread_id, ex.file.c_str(), ex.message.c_str(), exec_get_sim_time()) ;
        exit(ex.ret_code) ;
#ifdef __linux__
    // for post gcc 4.1.2
    } catch (abi::__forced_unwind&) {
        //pthread_exit and pthread_cancel will cause an abi::__forced_unwind to be thrown. Rethrow it.
        throw;
#endif
    }

    pthread_exit(NULL) ;
    return 0 ;
}
