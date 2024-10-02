/*
    PURPOSE:
        (Trick runtime simulation executive scheduler.)
*/

#ifndef EXECUTIVE_HH
#define EXECUTIVE_HH

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "Scheduler.hh"
#include "ScheduledJobQueue.hh"
#include "SimObject.hh"
#include "Threads.hh"
#include "sim_mode.h"

namespace Trick {

    /**
     * This class is the main scheduler of the simulation.  The executive schedules job classes: default_data,
     * initialization, all "scheduled" classes, end_of_frame, freeze_init, freeze, unfreeze, and shutdown jobs.
     * The default "scheduled" classes include: integ_loop, automatic, random, environment, sensor,
     * sensor_emitter, * sensor_reflector, sensor_receiver, scheduled, effector, effector_emitter,
     * effector_receiver, automatic_last, logging, data_record, system_advance_sim_time,
     * and system_moding.
     *
     * The executive also handles the POSIX signals, SIGSEGV, SIGBUS, SIGINT, and SIGFPE.
     *
     * @author Robert W. Bailey
     * @author Many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class Executive : public Trick::Scheduler {
        public:
            /** gets #except_return */
            virtual int get_except_return() const;

            /** Set to true to pause before every job call during sim execution.\n */
            bool debug_pause_flag ;               /**< trick_units(--) */

        protected:
            /** Attempts to attach a debugger in the event a signal shuts down the simulation.\n */
            bool attach_debugger;            /**< trick_units(--) */

            /** Debugger command used in the event a signal shuts down the simulation. defaults to /usr/bin/gdb.\n */
            std::string debugger_command;          /**< trick_units(--) */

            /** Allows the use of <Ctrl-C> to freeze and resume the simulation.\n */
            bool enable_freeze;              /**< trick_units(--) */

            /** Attempts to print a stack trace in the event a signal shuts down the simulation.\n */
            bool stack_trace;                /**< trick_units(--) */

            /** Allows the trapping of SIGBUS signals and a graceful shutdown.\n */
            bool trap_sigbus;                /**< trick_units(--) */

            /** Allows the trapping of SIGFPE signals and a graceful shutdown.\n */
            bool trap_sigfpe;                /**< trick_units(--) */

            /** Allows the trapping of SIGSEGV signals and graceful shutdown.\n */
            bool trap_sigsegv;               /**< trick_units(--) */

            /** Allows the trapping of SIGABRT signals and graceful shutdown.\n */
            bool trap_sigabrt;               /**< trick_units(--) */

            /** Allows the trapping of SIGCHLD signals\n */
            bool trap_sigchld;               /**< trick_units(--) */

            /** Flags a restart was loaded\n */
            bool restart_called;    /**< trick_io(**) trick_units(--) */

            /** Flags that all initialization tasks have completed successfully.\n */
            bool initialization_complete;    /**< trick_units(--) */

            /** Allows the current thread to give up the cpu during multi process job completion and dependency checking.\n */
            bool rt_nap;                      /**< trick_units(--) */

            /** Software frame time.  The end_of_frame jobs will be run at this frequency.\n */
            double software_frame;            /**< trick_units(s) */

            /** Software frame in integer tics.\n */
            long long software_frame_tics ;   /**<  trick_units(--) */

            /** Number of elapsed software_frames.\n */
            long long frame_count ;   /**<  trick_units(--) */

            /** Number of elapsed software_frames during freeze.\n */
            long long freeze_frame_count ;   /**<  trick_units(--) */

            /** The specified sim objs for checkpoint, if it's null, checkpoint everything */
            Trick::JobData * freeze_job ;     /* ** */

            /** Times to enter freeze mode in tics.\n */
            std::priority_queue< long long, std::vector< long long >, std::greater< long long > > freeze_times ; /**< trick_units(--) */

            /** Freeze mode software frame time. All freeze jobs will be called at this frequency.\n */
            double freeze_frame;              /**< trick_units(s) */

            /** Freeze mode sofware frame in integer tics.\n */
            long long freeze_frame_tics ;     /**< trick_units(--) */

            /** Next software frame time in integer tics.\n */
            long long next_frame_check_tics ; /**< trick_units(--) */

            /** Usage time at executive initialization in user mode.\n */
            double user_cpu_init;                  /**< trick_io(**) trick_units(s) */

            /** Usage time at executive initialization in kernal mode.\n */
            double kernal_cpu_init;           /**< trick_io(**) trick_units(s) */

            /** CPU usage time at cyclic sim process startup in user mode.\n */
            double user_cpu_start;                 /**< trick_io(**) trick_units(s) */

            /** CPU usage time at cyclic sim process startup in kernal mode.\n */
            double kernal_cpu_start;                 /**< trick_io(**) trick_units(s) */

            /** Voluntary context switche usage at executive initialization.\n */
            long v_context_switch_init;           /**< trick_units(--) */

            /** Involuntary context switche usage at executive initialization.\n */
            long iv_context_switch_init;        /**< trick_units(--) */

            /** Simulation control mode.\n */
            SIM_MODE mode;                    /**< trick_io(*o) trick_units(--) */

            /** User set termination time in tics.\n */
            long long terminate_time;       /**< trick_units(--) */

            /** Number of integer tics per second.\n */
            int time_tic_value ;            /**< trick_units(1/s) */

            /** Previous value of number of integer tics per second.\n */
            int old_time_tic_value ;            /**< trick_units(1/s) */

            /** Current simulation time in tics. Requirement [@ref r_exec_time_0]\n */
            long long time_tics;            /**< trick_units(--) */

            /** Current simulation time in tics. Requirement [@ref r_exec_time_0]\n */
            long long freeze_time_tics;            /**< trick_units(--) */

            /** Next software frame time in integer tics.\n */
            long long next_freeze_frame_check_tics ; /**< trick_units(--) */

            /** Mode command settable by user or code.\n */
            SIM_COMMAND exec_command;         /**< trick_io(**) */

            /** Freeze command set by user or code asynchronously.\n */
            bool async_freeze_command;        /**< trick_io(**) */

            /** A signal is causing termination.\n */
            bool signal_caused_term;          /**< trick_io(**) */

            /** Start the simulation in Freeze.\n */
            bool freeze_command;              /**< trick_units(--) */

            /**  Only allow freezing on software frame boundaries.\n */
            bool freeze_on_frame_boundary ;   /**< trick_units(--) */

            /** Start the simulation at this time.\n */
            double sim_start;               /**< trick_units(s) */

            /** Simulation time of last loop in tics.  Relevant when advance_sim_time is not last job in frame.\n */
            long long time_last_pass_tics ; /**< trick_units(--) */

            /** Next scheduled jobs call time.\n */
            long long job_call_time_tics;   /**< trick_units(--) */

            /** Semaphore used to control pausing.\n */
            sem_t * debug_sem ;                   /**< trick_io(**) trick_units(--) */

            /** Semaphore name for debug_sem.\n */
            std::string debug_sem_name ;                /**< trick_io(**) trick_units(--) */

            /** stream to record elapsed time of default_data, 
                input_processor, and initialization queues \n */
            std::ofstream init_log_stream; /**< trick_units(--) */

            /** Queue to hold default data jobs.\n */
            Trick::ScheduledJobQueue default_data_queue ;     /**< trick_io(**) */

            /** Queue to hold input_processor jobs.\n */
            Trick::ScheduledJobQueue input_processor_queue ;   /**< trick_io(**) */

            /** Queue to hold initialization jobs.\n */
            Trick::ScheduledJobQueue initialization_queue ;   /**< trick_io(**) */

            /** Queue to hold input_processor jobs.\n */
            Trick::ScheduledJobQueue input_processor_run_queue ;   /**< trick_io(**) */

            /** Queue to hold all scheduled type jobs. This queue is only a list of all scheduled jobs.  It is only
                used by the multi-threaded scheduler to clear job completion flags.  Each thread contains its own
                scheduled class job queue.\n */
            Trick::ScheduledJobQueue scheduled_queue ;        /**< trick_io(**) */

            /** Queue to hold end of frame jobs.\n */
            Trick::ScheduledJobQueue top_of_frame_queue ;     /**< trick_io(**) */

            /** Queue to hold end of frame jobs.\n */
            Trick::ScheduledJobQueue end_of_frame_queue ;     /**< trick_io(**) */

            /** Queue to hold thread sync.\n */
            Trick::ScheduledJobQueue thread_sync_queue ;     /**< trick_io(**) */

            /** Queue to hold shutdown jobs.\n */
            Trick::ScheduledJobQueue shutdown_queue ;         /**< trick_io(**) */

            /** Queue to hold freeze_init jobs.\n */
            Trick::ScheduledJobQueue freeze_init_queue ;      /**< trick_io(**) */

            /** Queue to hold freeze jobs.\n */
            Trick::ScheduledJobQueue freeze_scheduled_queue ;           /**< trick_io(**) */

            /** Queue to hold freeze jobs.\n */
            Trick::ScheduledJobQueue freeze_queue ;           /**< trick_io(**) */

            /** Queue to hold unfreeze jobs.\n */
            Trick::ScheduledJobQueue unfreeze_queue ;         /**< trick_io(**) */

            /** Queue to hold time_tic_changed jobs.\n */
            Trick::ScheduledJobQueue time_tic_changed_queue ; /**< trick_io(**) */

            /** Enough threads to accomodate the number of children specified in the S_define file.\n */
            std::vector <Trick::Threads *> threads ;               /**< trick_io(**) */

            /** Number of scheduled job type classes defined in the S_define file.\n */
            int num_classes ;                                 /**< trick_units(--)  */

            /** Starting index to first scheduled class job.\n */
            int scheduled_start_index ;                       /**< trick_io(**) */

            /** How many scheduled job classes have we added\n */
            int num_scheduled_job_classes ;                   /**< trick_io(**) */

            /** Pointer to the advance_sim_time job.\n */
            Trick::JobData * advance_sim_time_job ;   /**< trick_io(**) */

            /** Pointer to the current job in main thread.\n */
            Trick::JobData * curr_job ;               /**< trick_io(**) */

            /** Return code to be returned by Executive:init(), Executive::loop(), and Executive::shutdown() \n */
            int except_return ;                       /**< trick_io(**) */

            /** File name of exceptions caught in init() and loop()\n */
            std::string except_file ;                      /**< trick_io(**) */

            /** Error message of exceptions caught in init() and loop()\n */
            std::string except_message ;                   /**< trick_io(**) */

            /** length of all_jobs_for_checkpoint array\n */
            unsigned int num_all_jobs ;                      /**< trick_units(--) */

            /** Maps job names as given at simobject addtion to JobData pointers.\n */
            std::multimap<std::string, Trick::JobData *> all_jobs ;    /**< trick_io(**) */

            /** Maps job tags as given at simobject addtion to JobData pointers.\n */
            std::multimap<std::string, Trick::JobData *> all_tagged_jobs ;    /**< trick_io(**) */

            /** All jobs known to the scheduler.\n */
            std::vector<Trick::JobData *> all_jobs_vector ;       /**< trick_io(**) */

            /** Contains same info as the all_jobs_vector but in a checkpointable form\n */
            Trick::JobData * all_jobs_for_checkpoint ;       /**< trick_units(--) */

            /** Count of how many sim_objects have been added to the scheduler. May be different than current number of sim_objects\n */
            int num_sim_objects ;                     /**< trick_units(--) */

            /** List of all sim_objects that have been added to scheduler through add_sim_object().\n */
            std::vector <Trick::SimObject *> sim_objects ;        /**< trick_io(*io) trick_units(--) */

            /** List of other schedulers in thes sim.\n */
            std::vector <Trick::Scheduler *> other_schedulers ;   /**< trick_io(**) */

            /** S_run_summary - version date tag.\n */
            std::string version_date_tag;           /**< trick_units(--) */

            /** S_run_summary - S_main build time.\n */
            std::string build_date;                 /**< trick_units(--) */

            /** S_run_summary - Trick version.\n */
            std::string current_version;            /**< trick_units(--) */

            /**
             @userdesc Command to reset job cycle times after the time_tic_value has changed.
             @return void
            */
            void reset_job_cycle_times() ;

            /**
             @userdesc Command to reset job call times when a new start time is inputed
             @return void
            */
            void reset_job_call_times() ;

            /**
             Internal call to test if thread is ready to run
             @return bool
            */
            bool isThreadReadyToRun( Trick::Threads * curr_thread , long long time_tics) ;

        public:

            Executive() ;

            ~Executive() ;

            // GET and SET FUNCTIONS

            /**
             Get all jobs known to the Executive.
             @param in_jobs_vector - returned pointer to vector of all jobs
             @return always 0
            */
            int get_all_jobs_vector(std::vector<Trick::JobData *> & in_jobs_vector) ;

            /**
             @userdesc Command to get the attach debugger toggle value.
             @par Python Usage:
             @code <my_int> = trick.exec_get_attach_debugger() @endcode
             @return boolean (C integer 0/1) Executive::attach_debugger
            */
            bool get_attach_debugger() ;

            /**
             @userdesc Command to get the version used to build the sim
             @par Python Usage:
             @code <my_char*> = trick.exec_get_current_version() @endcode
             @return string (C const char*) Executive::current_version
            */
            const std::string & get_current_version() ;

            /**
             @userdesc Command to get the debugger command value.
             @par Python Usage:
             @code <my_char*> = trick.exec_get_debugger_command() @endcode
             @return string (C const char*) Executive::debugger_command
            */
            const std::string & get_debugger_command() ;

            /**
             @userdesc Command to get the current Executive Mode command.
             @par Python Usage:
             @code <my_sim_command> = trick.exec_get_exec_command() @endcode
             @return Executive::exec_command (Trick::Executive::SIM_COMMAND)
            */
            SIM_COMMAND get_exec_command() ;

            /**
             @userdesc Command to get the <CTRL-C> freezing toggle value.
             @par Python Usage:
             @code <my_int> = trick.exec_get_enable_freeze() @endcode
             @return boolean (C integer 0/1) Executive::enable_freeze
            */
            bool get_enable_freeze() ;

            /**
             @userdesc Command to get the freeze command flag.
             @par Python Usage:
             @code <my_bool> = trick.exec_get_freeze_command() @endcode
             @return Executive::freeze_command
            */
            bool get_freeze_command() ;

            /**
             @userdesc Command to get the freeze_on_boundary flag
             @par Python Usage:
             @code <my_bool> = trick.exec_get_freeze_on_frame_boundary() @endcode
             @return Executive::freeze_on_frame_boundary
            */
            bool get_freeze_on_frame_boundary() ;

            /**
             @userdesc Command to get the freeze job cycle time in seconds.
             @par Python Usage:
             @code <my_double> = trick.exec_get_freeze_frame() @endcode
             @return Executive::freeze_frame
            */
            double get_freeze_frame() ;

            /**
             @userdesc Command to get the freeze job cycle time in integer tics.
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_freeze_frame_tics() @endcode
             @return Executive::freeze_frame_tics
            */
            long long get_freeze_frame_tics() ;

            /**
             @userdesc Command to get the current simulation control Mode.
             @par Python Usage:
             @code <my_sim_mode> = trick.exec_get_mode() @endcode
             @return Executive::mode (Trick::Executive::SIM_MODE)
            */
            SIM_MODE get_mode() ;

            /**
             @userdesc Command to get the number of children in the sim as specified in the S_define.
             @par Python Usage:
             @code <my_int> = trick.exec_get_num_threads() @endcode
             @return size of threads vector
            */
            unsigned int get_num_threads() ;

            /**
             @userdesc Command to get the previous number of integer tics per second.
             @par Python Usage:
             @code <my_int> = trick.exec_get_old_time_tic_value() @endcode
             @return Executive::old_time_tic_value
            */
            int get_old_time_tic_value() ;

            /**
             @userdesc Command to get the rt nap toggle value (does executive give up cpu waiting for children).
             @par Python Usage:
             @code <my_int> = trick.exec_get_rt_nap() @endcode
             @return boolean (C integer 0/1) Executive::rt_nap
            */
            bool get_rt_nap() ;

            /**
             @userdesc Command to get starting index to first scheduled class job.
             @par Python Usage:
             @code <my_int> = trick.exec_get_scheduled_start_index() @endcode
             @return Executive::scheduled_start_index
            */
            int get_scheduled_start_index() ;

            /**
             Get all sim objects known to the Executive.
             @param in_sim_objects - returned pointer to vector of all sim objects
             @return always 0
            */
            int get_sim_objects(std::vector<Trick::SimObject *> & in_sim_objects) ;

            /**
             Search for a sim object with the given name.
             @param sim_object_name - name of desired SimObject
             @return Pointer to sim object or NULL if not found
            */
            SimObject * get_sim_object_by_name(std::string sim_object_name);

            /**
             @userdesc Command to get the real-time software frame in seconds.
             @par Python Usage:
             @code <my_double> = trick.exec_get_software_frame() @endcode
             @return Executive::software_frame
            */
            double get_software_frame() ;

            /**
             @userdesc Command to get the real-time software frame in integer tics.
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_software_frame_tics() @endcode
             @return Executive::software_frame_tics
            */
            long long get_software_frame_tics() ;

            /**
             @userdesc Command to get the number of elapsed frames
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_frame_count() @endcode
             @return Executive::frame_count
            */
            long long get_frame_count() ;

            /**
             @userdesc Command to get the number of elapsed frames
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_freeze_frame_count() @endcode
             @return Executive::freeze_frame_count
            */
            long long get_freeze_frame_count() ;

            /**
             @userdesc Command to get the pthread id of a thread
             @par Python Usage:
             @code <my_longlong> = trick.get_pthread_id() @endcode
             @return pthread_id of thread if successful.  0 if unsuccessful.
            */
            pthread_t get_pthread_id(unsigned int thread_id) ;

            /**
             @userdesc Command to get the stack trace toggle value (will the sim attempt stack trace if signal shuts it down).
             @par Python Usage:
             @code <my_int> = trick.exec_get_stack_trace() @endcode
             @return boolean (C integer 0/1) Executive::stack_trace
            */
            bool get_stack_trace() ;

            /**
             @userdesc Command to get the time that the simulation will terminate, in seconds.
             Formula for termination time is terminate_time / time_tic_value.
             @par Python Usage:
             @code <my_double> = trick.exec_get_terminate_time() @endcode
             @return sim termination time in seconds
            */
            double get_terminate_time() ;

            /**
             @userdesc Command to get the AMF cycle time of a thread
             @par Python Usage:
             @code <my_longlong> = trick.get_thread_amf_cycle_time() @endcode
             @return cycle time in seconds or -1.0 on error.
            */
            double get_thread_amf_cycle_time(unsigned int thread_id) ;

            /**
             @userdesc Command to get the number of integer tics per second.
             @par Python Usage:
             @code <my_int> = trick.exec_get_time_tic_value() @endcode
             @return Executive::time_tic_value
            */
            int get_time_tic_value() ;

            /**
             @userdesc Command to get the current simulation time in tics.
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_time_tics() @endcode
             @return Executive::time_tics
            */
            long long get_time_tics() ;

            /**
             @userdesc Command to get the current simulation time in tics.
             @par Python Usage:
             @code <my_longlong> = trick.exec_get_freeze_time_tics() @endcode
             @return Executive::freeze_time_tics
            */
            long long get_freeze_time_tics() ;

            /**
             @userdesc Command to get the trap sigbus toggle value (will SIGBUS signal be trapped).
             @par Python Usage:
             @code <my_int> = trick.exec_get_trap_sigbus() @endcode
             @return boolean (C integer 0/1) Executive::trap_sigbus
            */
            bool get_trap_sigbus() ;

            /**
             @userdesc Command to get the trap sigfpe toggle value (will SIGFPE signal be trapped).
             @par Python Usage:
             @code <my_int> = trick.exec_get_trap_sigfpe() @endcode
             @return boolean (C integer 0/1) Executive::trap_sigfpe
            */
            bool get_trap_sigfpe() ;

            /**
             @userdesc Command to get the trap sigsegv toggle value (will SIGSEGV signal be trapped).
             @par Python Usage:
             @code <my_int> = trick.exec_get_trap_sigsegv() @endcode
             @return boolean (C integer 0/1) Executive::trap_sigsegv
            */
            bool get_trap_sigsegv() ;

            /**
             @userdesc Command to get the trap sigsegv toggle value (will SIGSEGV signal be trapped).
             @par Python Usage:
             @code <my_int> = trick.exec_get_trap_sigsegv() @endcode
             @return boolean (C integer 0/1) Executive::trap_sigsegv
            */
            bool get_trap_sigabrt() ;

            /**
             @userdesc Command to get the trap sigchld toggle value (will SIGCHLD signal be trapped).
             @par Python Usage:
             @code <my_int> = trick.exec_get_trap_sigsegv() @endcode
             @return boolean (C integer 0/1) Executive::trap_sigsegv
            */
            bool get_trap_sigchld() ;

            /**
             @brief @userdesc Command to set the attach debugger toggle value.
             @param on_off - boolean yes (C integer 1) = attach debugger if signal shuts sim down. no (C integer 0) = do not attach debugger.
             @return always 0
            */
            int set_attach_debugger(bool on_off) ;

            /**
             @userdesc Command to set the debugger command value.
             This command is used in the event a signal shuts down the simulation (defaults to "/usr/bin/gdb").
             @par Python Usage:
             @code trick.exec_set_debugger_command("<command>") @endcode
             @param command - the debugger command you want to use when sim is shut down by a signal
             @return always 0
            */
            int set_debugger_command(std::string command) ;

            /**
             @userdesc Command to toggle <CTRL-C> freezing.  If enable_freeze is true, <CTRL-C> will freeze
             and unfreeze the simulation.  If false, the simulation will terminate when <CTRL-C> is caught.
             @par Python Usage:
             @code trick.exec_set_enable_freeze(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = allow <CTRL-C> freezing. no (C integer 0) = disallow <CTRL-C> freezing.
             @return always 0
            */
            int set_enable_freeze(bool on_off) ;

            /**
             @userdesc Command to tell the simulation to start in freeze mode.
             @par Python Usage:
             @code trick.exec_set_freeze_command(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = go to freeze mode from initialization. no (C integer 0) = go to run mode from initialization
             @return always 0
            */
            int set_freeze_command(bool on_off) ;

            /**
             @userdesc Command to tell the simulation to freeze only on software frame boundaries
             @par Python Usage:
             @code trick.exec_set_freeze_on_frame_boundary(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = freeze on boundary . no (C integer 0) = freeze anytime
             @return always 0
            */
            int set_freeze_on_frame_boundary(bool on_off) ;

            /**
             @userdesc Command to set job cycle time for freeze class jobs.
             @par Python Usage:
             @code trick.exec_set_freeze_frame(<in_frame>) @endcode
             @param in_frame - new freeze job cycle time in seconds
             @return always 0
             */
            int set_freeze_frame(double in_frame) ;

            /**
             @userdesc Command to set the current Executive Mode command.
             @par Python Usage:
             @code trick.exec_set_exec_command(<in_command>) @endcode
             @param in_command - Trick::Executive::SIM_COMMAND
            */
            int set_exec_command(SIM_COMMAND in_command) ;

            /**
             @userdesc Command to enable releasing the processor when waiting for a child thread to complete.
             The release function should yield the processor from the Trick executive scheduler.
             This allows the operating system scheduler to arbitrate process priorities based on the status of
             processes waiting to run.
             The default behavior (rt_nap is disabled) is to spin on the clock until the child thread completes.
             @par Python Usage:
             @code trick.exec_set_rt_nap(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable rt_nap, no (C integer 0) = disable rt_nap
             @return always 0
             */
            int set_rt_nap(bool on_off) ;

            /**
             @userdesc Command to set the real-time frame for real-time synchronization.
             @par Python Usage:
             @code trick.exec_set_software_frame(<in_frame>) @endcode
             @param in_frame - the real-time frame: the frequency at which to sync to the real-time clock
             @return always 0
            */
            int set_software_frame(double in_frame) ;

            /**
             @userdesc Command to set the stack trace toggle value (sim will attempt stack trace if signal shuts it down).
             @par Python Usage:
             @code trick.exec_set_stack_trace(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable stack trace, no (C integer 0) = disable stack trace
             @return always 0
            */
            int set_stack_trace(bool on_off) ;

            /**
             @userdesc Command to set the simulation time using a double (seconds) value.
             @par Python Usage:
             @code trick.exec_set_time(<in_time>) @endcode
             @param in_time - simulation time in seconds
            */
            int set_time(double in_time) ;

            /**
             @userdesc Command to set the simulation time using a long long (nteger tics) value.
             @par Python Usage:
             @code trick.exec_set_time_tics(<in_tics>) @endcode
             @param in_tics - simulation time in integer tics
            */
            int set_time_tics(long long in_tics) ;

            /**
             @userdesc Command to set the time tic value. A tic is 1/tic_value seconds.
             @par Python Usage:
             @code trick.exec_set_time_tic_value(<num_tics>) @endcode
             @param in_tics - simulation time value in integer tics
            */
            int set_time_tic_value(int in_tics) ;

            /**
             @userdesc Command to set the time to terminate the simulation. Same as stop(in_time).
             @par Python Usage:
             @code trick.exec_set_terminate_time(<in_time>) @endcode
             @param in_time - simulation time in seconds
            */
            int set_terminate_time(double in_time) ;

            /**
             @userdesc Command to enable/disable the trapping of SIGBUS signals and a graceful shutdown.
             Setting on_off to true (default) will trap the operating system communications bus error signal and then
             perforam a graceful simulation shutdown. If on_off is set to false, the signal will not be trapped and
             termination will occur producing a core file.
             @par Python Usage:
             @code trick.exec_set_trap_sigbus(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable trap, no (C integer 0) = disable trap
             @return always 0
            */
            int set_trap_sigbus(bool on_off) ;

            /**
             @userdesc Command to enable/disable the trapping of SIGFPE signals and a graceful shutdown.
             If on_off is set to false (default), the signal will not be trapped which causes any affected variable to contain Nan.
             Setting on_off to true will trap the operating system floating point exception signal and then
             perforam a graceful simulation shutdown. Trapping this signal is useful in debugging where floating point
             exception occurs.
             @par Python Usage:
             @code trick.exec_set_trap_sigfpe(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable trap, no (C integer 0) = disable trap
             @return always 0
            */
            int set_trap_sigfpe(bool on_off) ;

            /**
             @userdesc Command to enable/disable the trapping of SIGSEGV signals and a graceful shutdown.
             Setting on_off to true (default) will trap the operating system communications bus error signal and then
             perforam a graceful simulation shutdown. If on_off is false, the signal will not be trapped and
             termination will occur producing a core file.
             @par Python Usage:
             @code trick.exec_set_trap_sigsegv(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable trap, no (C integer 0) = disable trap
             @return always 0
            */
            int set_trap_sigsegv(bool on_off) ;

            /**
             @userdesc Command to enable/disable the trapping of SIGABRT signals and a graceful shutdown.
             Setting on_off to true (default) will trap the operating system communications bus error signal and then
             perforam a graceful simulation shutdown. If on_off is false, the signal will not be trapped and
             termination will occur producing a core file.
             @par Python Usage:
             @code trick.exec_set_trap_sigabrt(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable trap, no (C integer 0) = disable trap
             @return always 0
            */
            int set_trap_sigabrt(bool on_off) ;

            /**
             @userdesc Command to enable/disable the trapping of SIGCHILD signals.
             @par Python Usage:
             @code trick.exec_set_trap_child(<on_off>) @endcode
             @param on_off - boolean yes (C integer 1) = enable trap, no (C integer 0) = disable trap
             @return always 0
            */
            int set_trap_sigchld(bool on_off) ;

            /**
             @userdesc Command to get the simulation time in seconds.
             Formula for simulation time is time_tics / time_tic_value.
             @par Python Usage:
             @code <my_double> = trick.exec_get_sim_time() @endcode
             @return simulation time in seconds
            */
            double get_sim_time() ;

            /**
             @userdesc Command to get the Trick thread index of the current thread.
             @par Python Usage:
             @code <myint> = trick.exec_process_id() @endcode
             @return thread_id.  Master = 0, Child 1 = 1, Child 2 = 2, etc.
             */
            unsigned int get_process_id() ;

            /**
             Get the freeze job with the name "trick_sys.sched.freeze".  Used in S_define.
             @param job_name - sim object name
             @return 0 if success,  -1 and execterminate on error
             */
             int get_freeze_job(std::string sim_object_name) ;

            /**
             Get the job with the name "job_name".
             @param job_name - string name of the job to find
             @param j_instance - the instance number of the job in the sim_object.  Defaults to 1.
             @return Trick::JobData pointer to the job
             */
            Trick::JobData * get_job(std::string job_name, unsigned int j_instance = 1 ) ;

            /**
             Get the current executive job executing in the current thread.
             @return Trick::JobData pointer to the job
             */
            Trick::JobData * get_curr_job() ;

            /**
             @userdesc Command to return the job cycle of the job_name if job_name is not empty.
             If job_name is empty ("") then the current job cycle is returned.
             @par Python Usage:
             @code <mydouble> = trick.exec_get_job_cycle("<job_name>") @endcode
             @param job_name - name of job from S_job_execution file
             @return the job's cycle time in seconds, or 0.0 if job not found
             */
            double get_job_cycle(std::string job_name) ;

            /**
             @userdesc Command to turn on/off job with the name "job_name".
             If job_name is a job tag (from the S_define file), then turn on/off all jobs with that tag.
             @par Python Usage:
             @code trick.exec_set_job_onoff("<job_name>", <instance>, <on>) @endcode
             @param job_name - name of job from S_job_execution file
             @param instance - the instance number of the job in the sim_object.  Starts at 1.
             @param on - 1 to turn job on, 0 to turn job off
             @return 0 if successful or -1 if the job cannot be found
             */
            int set_job_onoff(std::string job_name, int instance_num, int on) ;

            /**
             @userdesc Command to change job cycle time with the name "job_name".
             If job_name is a job tag (from the S_define file), then change cycle time of all jobs with that tag.
             @par Python Usage:
             @code trick.exec_set_job_cycle("<job_name>", <instance>, <in_cycle>) @endcode
             @param job_name - name of job from S_job_execution file, or a job tag from S_define file
             @param instance - the instance number of the job in the sim_object.  Starts at 1.
             @param in_cycle - new cycle time in seconds
             @return 0 if successful, -1 if the job cannot be found, or -2 if in_cycle is invalid
             */
            int set_job_cycle(std::string job_name, int instance_num, double in_cycle) ;

            /**
             @userdesc Command to enable or disable all jobs in a sim object (without setting sim object status).
             @par Python Usage:
             @code trick.exec_set_sim_object_onoff("<sim_object_name>", <on>) @endcode
             @param sim_object_name - name of sim_object from S_define file
             @param on - 1 to turn jobs on, 0 to turn jobs off
             @return 0 if successful or -1 if the object cannot be found
             */
            int set_sim_object_jobs_onoff(std::string sim_object_name, int on) ;

            /**
             @userdesc Command to turn on/off the sim_object.
             @par Python Usage:
             @code trick.exec_set_sim_object_onoff("<sim_object_name>", <on>) @endcode
             @param sim_object_name - name of sim_object from S_define file
             @param on - 1 to turn object on, 0 to turn object off
             @return 0 if successful or -1 if the object cannot be found
             */
            int set_sim_object_onoff(std::string sim_object_name, int on) ;

            /**
             @userdesc Command to get whether the sim object is on or off.
             @par Python Usage:
             @code trick.exec_get_sim_object_onoff("<sim_object_name>") @endcode
             @param sim_object_name - name of sim_object from S_define file
             @return 1 if the object is enabled, 0 if the object is disabled, or -1 if the object cannot be found
             */
            int get_sim_object_onoff (std::string sim_object_name) ;

            /**
             @userdesc Trick Version & Date at the time executable was built.
             @par Python Usage:
             @code trick.exec_set_version_date_tag("<tag>") @endcode
             @param tag - Trick version date tag
             @return 0 if successful
             */
            int set_version_date_tag(std::string tag) ;

            /**
             @userdesc Store Time/Date that an executable was built.
             @par Python Usage:
             @code trick.exec_set_build_date("<date>") @endcode
             @param date - Trick version number
             @return 0 if successful
             */
            int set_build_date(std::string date) ;

            /**
             @userdesc Store Trick Version at the time an executable was built.
             @par Python Usage:
             @code trick.exec_set_current_version("<version>") @endcode
             @param version - Trick version number
             @return 0 if successful
             */
            int set_current_version(std::string version) ;

            // END GET and SET FUNCTIONS

            /**
             * Handles command line arguments specific to the executive scheduler.
             * @return always 0
             */
            virtual int process_sim_args() ;

            /**
             * Calls the default_data and initialization jobs.  Also sets initial values
             * simulation timing values.
             * @return error code or 0 for no errors.
             */
            virtual int init() ;

            /**
             * Calls the default_data jobs.
             * @return 0 for no errors or throws exception otherwise.
             */
            virtual int call_default_data() ;

            /**
             * Calls the input_processor jobs.
             * @return 0 for no errors or throws exception otherwise.
             */
            virtual int call_input_processor() ;

            /**
             * Calls the initialization jobs.
             * @return 0 for no errors or throws exception otherwise.
             */
            virtual int call_initialization() ;

            /**
             * This job copies the job information from the executive to a checkpointable form.
             * @return error code or 0 for no errors.
             */
            virtual int checkpoint() ;

            /**
             * This job deletes the memory created for checkpoints.
             * @return error code or 0 for no errors.
             */
            virtual int post_checkpoint() ;

            /**
             * This job resets the scheduler queues during a checkpoint restart.
             * @return error code or 0 for no errors.
             */
            virtual int restart() ;

            /**
             * This job resets the scheduler queues during a checkpoint restart.
             * @return error code or 0 for no errors.
             */
            virtual int clear_scheduled_queues() ;

            /**
             * @brief Writes the command line command executed, the environment used to build the simulation.
             * @param fp - open FILE * or NULL.
             * @return always 0
             */
            virtual int write_s_run_summary( FILE * fp ) ;

            /**
             * Starts all of the child threads.
             * @return always 0
             */
            virtual int create_threads() ;

            /**
             * Either calls the simpler loop_single_thread() if only 1 thread of execution is detected,
             * otherwise loop_multi_thread() is called
             * @return error code or 0 for no errors.
             */
            virtual int loop() ;

            /**
             * @brief Called by loop().  Starts frame execution for all child threads.
             * Calls scheduled jobs in the main thread.  Calls end_of_frame jobs at end of frame.
             * @return always 0
             */
            virtual int loop_multi_thread() ;

            /**
             * @brief Called by loop().  Calls jobs in the main thread.  Calls end_of_frame jobs at end of frame.
             * @return always 0
             */
            virtual int loop_single_thread() ;

            /**
             * Sets the exec mode to freeze either at end of frame or anytime if freeze was called.
             * @return always 0
             */
            virtual int async_freeze_to_exec_command() ;

            /**
             * Sets the exec mode to freeze either at end of frame or anytime if freeze was scheduled.
             * @param end_of_frame Job called at end of frame?
             * @return always 0
             */
            virtual int sched_freeze_to_exec_command(bool end_of_frame) ;

            /**
             * Advances sim time to the lower of next job call time or end of frame time.
             * @return always 0
             */
            virtual int advance_sim_time() ;

            /**
             * Job to synchronize AMF and ASYNC threads to the master.
             * @return always 0
             */
            virtual int thread_sync() ;

            /**
             * Job to synchronize SCHEDULED threads to the master.
             * @return always 0
             */
            virtual int scheduled_thread_sync() ;

            /**
             * @brief freeze_init job that initialized the freeze_scheduled loop
             * @return always 0
             */
            virtual int init_freeze_scheduled() ;

            /**
             * @brief Upon entry, calls freeze_init jobs.
             *  Loops through freeze jobs while mode != Run.  Calls unfreeze jobs when exiting freeze.
             * @return always 0
             */
            virtual int freeze_loop() ;

            /**
             * @brief Calls shutdown jobs, prints simulation time information, cancels all threads of execution.
             * @return always 0
             */
            virtual int shutdown() ;

            /**
             * Autocoded in S_Source.cpp.  Assigns an id to each scheduled job class found in the S_define file.
             * @param in_name - scheduled job class name
             * @return 0 if successful, -1 if name was empty
             */
            virtual int add_scheduled_job_class( std::string in_name ) ;

            // Functions to add new objects
            /**
             * @brief Non class function that finds the executive pointer and adds the incoming
             *  sim_object to the scheduler.
             * @param in_object - Trick::SimObject pointer to the sim_object.
             * @param in_name - unique string name for sim_object, set to instantiated name for
             *  sim_objects processed by CP
             * @return always 0
             */
            virtual int add_sim_object(Trick::SimObject * in_object ) ;
            virtual int add_sim_object(Trick::SimObject * in_object , const char * in_name ) ;


            /**
             * @brief Initializes executive parameters in all sim_object jobs and adds the jobs to the sceduling queues.
             * @param sim_object - sim_object to process jobs
             * @param restart - true when a checkpoint restart is in progress
             * @return always 0
             */
            virtual int add_jobs_to_queue(Trick::SimObject * sim_object, bool restart = false ) ;

            /**
             * @brief Adds JobData pointer to assigned thread queue if scheduled class job.  For non-scheduled jobs i.e. "initialization",
             * all jobs added to main thread queues.
             * @param job_data - pointer to current job to be added to queue.
             * @return always 0
             */
            virtual int add_job_to_queue( Trick::JobData * job_data ) ;

            /**
             * @brief Removes the sim_object and all of its jobs from the simulation.
             * @param in_object - Trick::SimObject pointer to the sim_object.
             * @return always 0
             */
            virtual int delete_sim_object(Trick::SimObject * in_object ) ;

            /**
             * @brief Removes jobs from a sim_object from the scheduling queues.
             * @param sim_object - sim_object to remove
             * @return always 0
             */
            virtual int remove_jobs(Trick::SimObject * sim_object ) ;

            /**
             * Adds a "depends on" job to the target job.  The target job will wait for the dependent job to finish before
             * starting.  The target and depends on jobs must be on different threads.
             * @param target_job - string name of the job that will wait for the dependent job to finish.
             * @param t_instance - the instance number of the target job in the sim_object
             * @param depend_job - string name of the job that is to be waited on.
             * @param d_instance - the instance number of the depends job in the sim_object
             * @return 0 if dependency added, -1 if target_job not found, -2 if depend_job not found -3 if target and depend job on same thread
             */
            virtual int add_depends_on_job( std::string target_job , unsigned int t_instance , std::string depend_job , unsigned int d_instance ) ;

            /**
             * Check to see all jobs in the simulation are handled .
             * @return always 0
             */
            virtual int check_all_jobs_handled( ) ;

            /**
             * Check to see all jobs are scheduled with exact frequencies
             * @return always 0
             */
            virtual int check_all_job_cycle_times() ;

            // Functions called during init

            /**
             * Adds signal handlers for SIGINT, SIGTERM, SIGSEGV, SIGBUS, and SIGFPE. Called by the constructor but
             * is user callable to reset the default signal handler.
             * @return always 0
             */
            virtual int init_signal_handlers() ;

            /**
             * @brief Handles signals SIGSEGV and SIGBUS if directed so in initialization jobs.
             * @return always 0
             */
            virtual void signal_handler(int sig) ;

            /**
             * @brief Handles signal SIGINT if directed so in initialization jobs.
             * @return always 0
             */
            virtual void ctrl_c_handler() ;

            /**
             * @brief Handles signal SIGTERM
             * @return always 0
             */
            virtual void term_handler() ;

            /**
             * @brief Handles signal SIGTERM
             * @return always 0
             */
            virtual void usr1_handler() ;

            /**
             * Handles signal handlers for SIGSEGV, SIGBUS, and SIGINT if directed so in initialization jobs.
             * @return always 0
             */
            virtual void fpe_handler(siginfo_t * sip) ;

            /**
             @userdesc Get a child Thread class
             @par Python Usage:
             @code trick.get_thread(<thread_id>) @endcode
             @param thread_id - thread id as specified in S_define file
             @return Trick::Thread if successful, NULL if thread does not exist
            */
            Trick::Threads * get_thread(unsigned int thread_id) ;

            /**
             @userdesc Command to set the thread scheduling type of a child thread.  Does not affect main thread, which is fixed at scheduled.
             @par Python Usage:
             @code trick.exec_set_thread_enabled(<thread_id>, <yes_no>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param yes_no - True or False
             @return 0 if successful, -2 if thread does not exist or is the main thread which cannot be disabled.
            */
            virtual int set_thread_enabled(unsigned int thread_id , bool enabled) ;

            /**
             @userdesc Command to set the thread scheduling type of a child thread.  Does not affect main thread, which is fixed at scheduled.
             @par Python Usage:
             @code trick.exec_set_thread_process_type(<thread_id>, <process_type>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param process_type - integer representation of enumeration.  0 = scheduled, 1 = asynchronous, 2 = asynchronous_must_finish
             @return 0 if successful, -2 if thread does not exist. -1 if process_type does not exist
            */
            virtual int set_thread_process_type(unsigned int thread_id , int process_type) ;

            /**
             @userdesc Command to set an asynchronous_must_finish child thread's cycle_time.
             @par Python Usage:
             @code trick.exec_set_thread_amf_cycle_time(<thread_id>, <cycle_time>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param cycle_time - how often to synchronize with the master thread.  Time is in seconds.
             @return 0 if successful, -2 if thread does not exist. -1 if the thread is not asynchronous_must_finish
            */
            virtual int set_thread_amf_cycle_time(unsigned int thread_id , double cycle_time) ;

            /**
             @userdesc Command to set a child thread's asynchronous wait at shutdown flag, so that the executive will wait for the child to finish
             before shutting down (before shutdown class jobs are run, etc).
             @par Python Usage:
             @code trick.exec_set_thread_async_wait(<thread_id>, <yes_no>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param yes_no - boolean (C integer 0/1) indicating do we wait at shutdown for the thread or not.
             @return 0 if successful, -2 if thread does not exist.
            */
            virtual int set_thread_async_wait(unsigned int thread_id , bool yes_no) ;

            /**
             @userdesc Command to set a child thread's busy loop spin on flag while waiting.
             @par Python Usage:
             @code trick.exec_set_thread_rt_semaphore(<thread_id>, <yes_no>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param yes_no - boolean (C integer 0/1) indicating do we want to busy loop spin.
             @return 0 if successful, -2 if thread does not exist.
            */
            virtual int set_thread_rt_semaphores(unsigned int thread_id , bool yes_no) ;

            /**
             @userdesc Command to set the scheduling priority of a thread.
             @par Python Usage:
             @code trick.exec_set_thread_priority(<thread_id>, <req_priority>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param req_priority - the requested priority.  1 = highest.
             @return 0 if successful, -2 if thread does not exist.
            */
            virtual int set_thread_priority(unsigned int thread_id , unsigned int req_priority) ;

            /**
             @userdesc Command to set the processor for a thread to run on.
             @par Python Usage:
             @code trick.exec_set_thread_cpu_affinity(<thread_id>, <cpu_num>) @endcode
             @param thread_id - thread id as specified in S_define file
             @param cpu_num - the processor number to run thread on
             @return 0 if successful, -2 if thread does not exist.
            */
            virtual int set_thread_cpu_affinity(unsigned int thread_id , int cpu_num) ;

            /**
             @userdesc Command to run the simulation (after a freeze). Set exec_command to RunCmd.
             @par Python Usage:
             @code trick.run() @endcode
             @return always 0
            */
            virtual int run() ;

            /**
             @userdesc Command to step the simulation, pausing after each job. Set exec_command to StepCmd.
             @par Python Usage:
             @code trick.step() @endcode
             @return always 0
            */
            virtual int step() ;

            /**
             @userdesc Command to freeze the simulation now. Set exec_command to FreezeCmd.
             @par Python Usage:
             @code trick.freeze() @endcode
             @return always 0
            */
            virtual int freeze() ;

            /**
             @userdesc Command to freeze the simulation at a future time.
             @par Python Usage:
             @code trick.freeze() @endcode
             @param in_time - time in simulation seconds to freeze
             @return always 0
            */
            virtual int freeze(double in_time) ;

            /**
             @userdesc Command to terminate the simulation now. Set exec_command to ExitCmd.
             @par Python Usage:
             @code trick.stop() @endcode
             @return always 0
            */
            virtual int stop() ;

            /**
             @userdesc Command to set the time to terminate the simulation. Same as set_terminate_time(in_time).
             @par Python Usage:
             @code trick.stop(<in_time>) @endcode
             @param in_time - simulation time in seconds
            */
            virtual int stop(double in_time) ;

            /**
             Add an instrumentation job with the name in_job before target_job (if specified),
             otherwise add in_job before each job in the initialization, integration, scheduled, end_of_frame, & shutdown job queues.
             Requirement [@ref r_exec_instrument_0]
             @param in_job - name of the instrument job
             @param target_job - name of the target job, empty string means all jobs are to be instrumented
             @param in_event - instrumentation event data to be stored with instrument job
             @return number of times the job was inserted
            */
            virtual int instrument_job_before(std::string in_job, unsigned int instance = 1) ;

            /**
             Actually performs the job insertions described above. Called by the above instrument_job_before with the appropriate
             instrument job pointer.
             Requirement [@ref r_exec_instrument_0]
             @param instrument_job - pointer to the instrument job
             @param target_job - name of the target job, empty string means all jobs are to be instrumented
             @param in_event - instrumentation event data to be stored with instrument job
             @return number of times the job was inserted
            */
            virtual int instrument_job_before(Trick::JobData * instrument_job ) ;

            /**
             Add an instrumentation job with the name in_job after target_job (if specified),
             otherwise add in_job after each job in the initialization, integration, scheduled, end_of_frame, & shutdown job queues.
             Requirement [@ref r_exec_instrument_2]
             @param in_job - name of the instrument job
             @param target_job - name of the target job, empty string means all jobs are to be instrumented
             @param in_event - instrumentation event data to be stored with instrument job
             @return number of times the job was inserted
            */
            virtual int instrument_job_after(std::string in_job , unsigned int instance = 1 ) ;
            /**
             Actually performs the job insertions described above. Called by the above instrument_job_after with the appropriate
             instrument job pointer.
             Requirement [@ref r_exec_instrument_2]
             @param instrument_job - pointer to the instrument job
             @param target_job - name of the target job, empty string means all jobs are to be instrumented
             @param in_event - instrumentation event data to be stored with instrument job
             @return number of times the job was inserted
            */
            virtual int instrument_job_after(Trick::JobData * instrument_job) ;

            /**
             Remove an instrumentation job with the name in_job from all job queues where it was inserted.
             Requirement [@ref r_exec_instrument_3]
             @param in_job - name of the instrument job
             @param in_event - associated instrumentation event data
             @return always 0
            */
            virtual int instrument_job_remove(std::string in_job) ;

            /**
             @brief Writes the S_job_execution file which details the jobs known to the scheduler at the end of initialization.
             @param fp - either an open file pointer or NULL.  If NULL write_s_job_execution will open a new file.
             @return always 0
            */
            virtual int write_s_job_execution( FILE * fp ) ;

            /**
             Register other schedulers with the executive.  Other schedulers can be instrumented. The
             executive will call the instrument_job_* jobs for the other schedulers when it is asked to
             instrument itself.
             @param scheduler - the incoming scheduler (like an integrator or memory manager wrapper)
             @return always 0
            */
            virtual int register_scheduler(Trick::Scheduler * scheduler ) ;

            /**
             @userdesc Transition to Shutdown mode.  This matches the calling arguments of the legacy exec_terminate call.
             Same as calling exec_terminate_with_return() with ret_code=-1 and line=0.
             @param file_name - file name where exception occurred
             @param error - error message to print
             @return always 0
            */
            virtual int exec_terminate(const char *file_name, const char *error);

            /**
             @brief Instrumentation class job that pauses before each job when debug_pause is on.
             Waits on a semapahore before execution continues.
             @param curr_job - pointer to current instrument job that points to the job to pause at
             @return always 0
             */
            int debug_pause(Trick::JobData * curr_job) ;

            /**
             @brief Command to post the semaphore so that execution continues after pause.
             @return always 0
             */
            int debug_signal() ;

            /**
             @brief Command to set debug_pause_flag to true which turns on the debug pausing.
             Calls Trick::Executive::instrument_job_before to insert the debug_pause routine before every job to pause at.
             @return always 0
             */
            int debug_pause_on() ;

            /**
             @brief Command to set debug_pause_flag to false which turns off the debug pausing.
             Calls Trick::Executive::instrument_job_remove to remove the debug_pause routine from all job queues that it was inserted in.
             @return always 0
             */
            int debug_pause_off() ;
            
            /* deleted functions  */
      private:
            /* SWIG doesn't like the Executive assignment operator because of ofstream init_log_stream */
            Executive& operator=(const Executive&); /* = delete; '= delete' is not compatible with SWIG 2.0.
                                                                  stick to 'private' for now */

    } ;

}

#ifndef SWIG
extern Trick::Executive* the_exec;
#endif

#endif

