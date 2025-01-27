/*
    PURPOSE:
        (Trick Sys Threads implementation)
*/

#ifndef SYSTHREAD_HH
#define SYSTHREAD_HH

#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#if __linux__
#include <sys/types.h>
#endif
#include <unistd.h>
#include <sched.h>
#include "trick/ThreadBase.hh"


namespace Trick {

    /**
     * The purpose of this class is to ensure safe shutdown for Trick system threads, since user threads are handled separately in
     * the Trick::Threads and Executive classes.
     * 
     * This class was implemented as a solution to issue https://github.com/nasa/trick/issues/1445
     *
     * @author Jackie Deans
     *
     *      
     **/
    class SysThread : public Trick::ThreadBase {
        public:
            SysThread(std::string in_name);
            ~SysThread();

            static int ensureAllShutdown();

        protected:
            // Called from the main thread
            void force_thread_to_pause();
            // Called from the main thread
            void unpause_thread();

            // Called from the sys_thread at a point that would be appropriate to pause
            void test_pause();
        
        private: 
            /** Synchronization to safely pause and restart processing during a checkpoint reload */
            pthread_mutex_t _restart_pause_mutex ;      /**<  trick_io(**) */

            // For the main thread to tell the sys_thread to pause
            bool _thread_should_pause;                 /**<  trick_io(**) */
            // For the main thread to tell the sys_thread to wake up
            pthread_cond_t _thread_wakeup_cv;           /**<  trick_io(**) */

            // For the main thread to wait for the sys_thread to pause
            pthread_cond_t _thread_has_paused_cv;       /**<  trick_io(**) */
            bool _thread_has_paused;                    /**<  trick_io(**) */


            // Had to use Construct On First Use here to avoid the static initialziation fiasco
            static pthread_mutex_t& list_mutex();
            static pthread_cond_t& list_empty_cv();                 

            static std::vector <SysThread *>& all_sys_threads();

            static bool shutdown_finished;

    } ;

}

#endif

