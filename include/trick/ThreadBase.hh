/*
    PURPOSE:
        (Trick Threads implementation)
*/

#ifndef THREADBASE_HH
#define THREADBASE_HH

#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <string>
#if __linux__
#include <sys/types.h>
#endif
#include <unistd.h>
#include <sched.h>

namespace Trick {

    /**
     * One instance of this class is instantiated for each thread in the simulation.  This class
     * manages the scheduled jobs associated with it's thread.  It also manages the thread specific
     * realtime capabilities
     *
     * @author Robert W. Bailey
     * @author many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @todo implement realtime controls rt_on and rt_off
     */
    class ThreadBase {

        public:

            /**
             * This constructor initializes the member data and initializes the mutexes.
             */
            ThreadBase(std::string in_name = "") ;

            virtual ~ThreadBase() ;

            std::string get_name() ;
            void set_name(std::string in_name) ;

            /**
             * Gets the thread pthread ID.
             */
            pthread_t get_pthread_id() ;

            /**
             * Gets the thread process ID.
             */
            pid_t get_pid() ;

            /**
             * Tells thread to set the thread process ID.
             */
            void set_pid() ;

            /**
             * Adds a CPU affinity to a single CPU
             * @return always 0
             */
            int cpu_set(unsigned int cpu) ;

            /**
             * Clears a CPU to the affinity mask.
             * @return always 0
             */
            int cpu_clr(unsigned int cpu ) ;

#if __linux__
#ifndef SWIG
            /**
             * Gets the cpu set for this thread
             * @return always 0
             */
            cpu_set_t * get_cpus() ;

            /**
             * Copys the cpu set from the incoming set to this thread
             * @return always 0
             */
            void copy_cpus(cpu_set_t * in_cpus) ;
#endif
#endif
#if __APPLE__
            /**
             * Gets the cpu set for this thread
             * @return always 0
             */
            void * get_cpus() ;

            /**
             * Copys the cpu set from the incoming set to this thread
             * @return always 0
             */
            void copy_cpus(void * in_cpus) ;
#endif

            /**
             * Executes the CPU affinity settings
             * @return always 0
             */
            int execute_cpu_affinity() ;

            /**
             * Executes the CPU affinity settings
             * @return always 0
             */
            int set_priority(unsigned int req_priority) ;

            /**
             * Sets the priority to run this thread with.
             * @param yes_no - do we wait for the thread at shutdown or not?
             * @return always 0
             */
            int execute_priority() ;

            /**
             * Creates a new thread.
             * @return always 0
             */
            virtual int create_thread() ;

            /**
             * Cancels thread.
             * @return always 0
             */
            virtual int cancel_thread() ;

            /**
             * Joins thread.
             * @return always 0
             */
            virtual int join_thread() ;

            /**
             * The thread body.
             * @return always 0
             */
            virtual void * thread_body() = 0 ;

            /**
             * pthread_create compatible function to start new thread
             * @return always 0
             */
            static void * thread_helper( void * context ) ;

            /**
             * Prints thread information to the incoming file pointer
             */
            virtual void dump( std::ostream & oss = std::cout ) ;

            virtual void test_shutdown();
            virtual void test_shutdown(void (*exit_handler) (void *), void * exit_arg);

            virtual void thread_shutdown();
            virtual void thread_shutdown(void (*exit_handler) (void *), void * exit_arg);

        protected:

            /** optional name of thread */
            std::string name ;              /**< trick_units(--) */

            /** pthread id associated with thread when created with pthread_create */
            pthread_t pthread_id;           /**< trick_io(**) */

            /** process id associated with thread.  This differs from the pthread_id  */
            pid_t pid;                      /**< trick_io(**) */

            /** Priority to set if rt_nond_pri = Yes , the highest priority is 1 */
            unsigned int  rt_priority;      /**< trick_units(--) */

            /** Set of cpus to use with thread */
            unsigned int max_cpu ;          /**< trick_io(**) */

            /** Track whether the thread has been created */
            bool created ;          /**< trick_io(**) */

            /** Manage thread shutdown */
            bool should_shutdown;   /**< trick_io(**) */
            bool cancellable;   /**< trick_io(**) */
            pthread_mutex_t shutdown_mutex;     /**< trick_io(**) */

#if __linux__
#ifndef SWIG
            /** Set of cpus to use with thread */
            cpu_set_t * cpus ;              /**< trick_io(**) */
#endif
#endif
    } ;

}

#endif

