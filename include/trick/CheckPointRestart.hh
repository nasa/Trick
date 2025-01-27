/*
    PURPOSE:
        (CheckPointRestart)
*/

#ifndef MMWRAPPER_HH
#define MMWRAPPER_HH

#include <string>
#include <vector>
#include <queue>
#include <map>

#include "trick/Scheduler.hh"

namespace Trick {

    /**
     *
     * This class wraps the MemoryManager class for use in Trick simulations
     * @author Alexander S. Lin
     *
     */
    class CheckPointRestart : public Trick::Scheduler {

        private:
            /** Flag to track if an automatic freeze has been triggered */
            bool auto_freeze = false;                               /* ** */

            /** Map to track custom named checkpoints based on the scheduled times */
            std::map<long long, std::string> chkpnt_names;          /* ** */

        protected:
            /** queue to hold jobs to be called before a checkpoint is dumped. */
            Trick::ScheduledJobQueue checkpoint_queue ;              /* ** */

            /** queue to hold jobs to be called before a checkpoint is dumped. */
            Trick::ScheduledJobQueue post_checkpoint_queue ;         /* ** */

            /** queue to hold jobs to be called before a checkpoint is reloaded. */
            Trick::ScheduledJobQueue preload_checkpoint_queue ;      /* ** */

            /** queue to hold jobs to be called after a checkpoint is reloaded. */
            Trick::ScheduledJobQueue restart_queue ;                 /* ** */

            /** The specified sim objs for checkpoint, if it's null, checkpoint everything */
            std::vector<const char*> obj_list;                       /* ** */

            /** The specified sim objs for checkpoint, if it's null, checkpoint everything */
            Trick::JobData * write_checkpoint_job ;                  /* ** */

            /** The specified sim objs for checkpoint, if it's null, checkpoint everything */
            Trick::JobData * safestore_checkpoint_job ;              /* ** */

            /**
             * Internal call the MemoryManager checkpoint method with the string argument file_name
             * @param file_name - file name to write checkpoint
             * @param print_status - print a message when checkpoint is written
             * @return always 0
             */
            int do_checkpoint( std::string file_name , bool print_status) ;

        public:

            /** Times to dump a checkpoint. Saved as simulation tics.\n */
            std::priority_queue< long long, std::vector< long long >, std::greater< long long > > checkpoint_times ; /**< trick_units(--) */

            /** Period to dump a recurring checkpoint. Saved as simulation tics.\n */
            long long safestore_period ;                            /**< trick_units(--) */

            /** Next time to dump a recurring checkpoint. Saved as simulation tics.\n */
            long long safestore_time ;                              /**< trick_units(--) */

            /** If true take a pre_init_checkpoint\n */
            bool pre_init_checkpoint ;                              /**< trick_units(--) */

            /** If true take a post_init_checkpoint\n */
            bool post_init_checkpoint ;                             /**< trick_units(--) */

            /** If true take a end_checkpoint\n */
            bool end_checkpoint ;                                   /**< trick_units(--) */

            /** If true enable taking safestore checkpoints\n */
            bool safestore_enabled ;                                /**< trick_units(--) */

            /** output_directory/checkpoint_file_name to dump for a checkpoint\n */
            std::string output_file ;                               /**< ** */

            /** output_directory/checkpoint_file_name to load for a restore\n */
            std::string load_checkpoint_file_name ;                 /**< ** */

            /** CPU to use for checkpoints\n */
            int cpu_num ;                                  /**< trick_units(--) */

            /**
             * This is the constructor of the CheckPointRestart class.  It initializes
             * the checkpoint, pre_load_checkpoint, and the restart_queues
             */
            CheckPointRestart() ;

            /**
             @brief @userdesc Command to set the pre_init_checkpoint flag. If pre_init_checkpoint is set
             a checkpoint will be done before Initialization class jobs are run (at beginning of P1 phase).
             The checkpointed file name is @e chkpnt_pre_init.
             @par Python Usage:
             @code trick.checkpoint_pre_init(<yes_no>) @endcode
             @param yes_no - boolean yes (C integer 1) = dump checkpoint, no (C integer 0) = do not dump
             @return always 0
            */
            int set_pre_init_checkpoint(bool yes_no) ;

            /**
             @brief @userdesc Command to set the post_init_checkpoint flag.  If post_init_checkpoint is set
             a checkpoint will be done after Initialization class jobs are run (at beginning of the P65535 phase).
             The checkpointed file name is @e chkpnt_post_init.
             @par Python Usage:
             @code trick.checkpoint_post_init(<yes_no>) @endcode
             @param yes_no - boolean yes (C integer 1) = dump checkpoint, no (C integer 0) = do not dump
             @return always 0
             */
            int set_post_init_checkpoint(bool yes_no) ;

            /**
             @brief @userdesc Command to set the end_checkpoint flag.  If end_checkpoint is set
             a checkpoint will be done at simulation termination before any other Shutdown class jobs are run.
             The checkpointed file name is @e chkpnt_end.
             @par Python Usage:
             @code trick.checkpoint_end(<yes_no>) @endcode
             @param yes_no - boolean yes (C integer 1) = dump checkpoint, no (C integer 0) = do not dump
             @return always 0
             */
            int set_end_checkpoint(bool yes_no) ;

            /**
             @brief @userdesc Command to set the safestore_enabled flag.  If safestore_enabled is set
             periodic checkpoints will be done according to safestore_period that was set in checkpoint_safestore().
             The checkpointed file name is @e chkpnt_safestore.
             @par Python Usage:
             @code trick.checkpoint_safestore_set_enabled(<yes_no>) @endcode
             @param yes_no - boolean yes (C integer 1) = dump periodic checkpoint, no (C integer 0) = do not dump
             @return always 0
             */
            int set_safestore_enabled(bool yes_no) ;

            /**
             @brief @userdesc Command to get the name of the checkpoint dump file.
             @par Python Usage:
             @code trick.checkpoint_get_output_file() @endcode
             @return string representing <output_directory>/<checkpoint_file_name>
             */
            const char * get_output_file() ;

            /**
             @brief @userdesc Command to get the name of the checkpoint load file.
             @par Python Usage:
             @code trick.checkpoint_get_load_file() @endcode
             @return string representing <output_directory>/<checkpoint_file_name>
             */
            const char * get_load_file() ;

            /**
             @brief @userdesc Command to set the CPU to use for checkpoints.  The default is to use the same CPU as the main thread.
             If the main thread of the simulation is running at high real-time priority, it is recommended to
             choose a different CPU to use for checkpointing.
             @par Python Usage:
             @code trick.checkpoint_cpu(<in_cpu_num>) @endcode
             @param in_cpu_num - CPU number that Trick will use to perform checkpoints
             @return always 0
             */
            int set_cpu_num(int in_cpu_num) ;

            /**
             * Get the write_checkpoint_job and safestore_checkpoint jobs.
             * @return always 0
             */
            int find_write_checkpoint_jobs(std::string sim_object_name) ;

            /**
             @brief @userdesc Command to set the desired period that safestore checkpoints will be dumped. (Sets safestore_period to the integral time tic value corresponding
             to the incoming in_time so that checkpoint occurs periodically.)
             @par Python Usage:
             @code trick.checkpoint_safestore(<in_time>) @endcode
             @param in_time - desired safestore checkpoint time period in seconds.
             @return always 0
             */
            int set_safestore_time(double in_time) ;

            /**
             @brief @userdesc Command to dump a checkpoint now to the specified file.
             Calls the MemoryManager checkpoint method with the string argument file_name
             and sim objects list string separated by "," to specify which sim objects need
             checkpointing. If sim objects are not specified, all will be checkpointed.
             @par Python Usage:
             @code trick.checkpoint() @endcode
             @param file_name - optional: name of checkpoint file to dump (default is "chkpnt_<time>")
             @param print_status - optional: boolean yes (C integer 1) = print the dump checkpoint status message
             @param obj_list_str - optional: sim objects list string for checkpointing (default is dump all)
             @return always 0
             */
            virtual int checkpoint(std::string file_name = "", bool print_status = true , std::string obj_list_str = "") ;

            /**
             @brief @userdesc Command to dump a checkpoint at in_time. (Sets checkpoint_time to the integral time tic value corresponding
             to the incoming in_time so that checkpoint occurs once at that time at the end of the execution frame.)
             The checkpointed file name is @e chkpnt_<in_time> or @e <file_name>.
             @par Python Usage:
             @code trick.checkpoint(<in_time>) @endcode
             @param in_time - desired checkpoint time in seconds.
             @param file_name - checkpoint file name. Defaults to blank in which case the checkpoint follows the expecteed convention.
             @return always 0
             */
            virtual int checkpoint(double in_time, std::string file_name = "") ;

            /**
             * Executes the pre_init_checkpoint
             * @return always 0
             */
            virtual int write_pre_init_checkpoint() ;

            /**
             * Executes the post_init_checkpoint
             * @return always 0
             */
            virtual int write_post_init_checkpoint() ;

            /**
             * Executes the end_checkpoint
             * @return always 0
             */
            virtual int write_end_checkpoint() ;

            /**
             * Creates a file name based on the simulation time, "checkpoint_<time>" and
             * calls checkpoint(string) routine with the filename
             * @param sim_time_tics - current simulation time
             * @return always 0
             */
            virtual int write_checkpoint() ;

            /**
             * Creates a file name based on the simulation time, "checkpoint_<time>" and
             * calls checkpoint(string) routine with the filename
             * @param sim_time_tics - current simulation time
             * @return always 0
             */
            virtual int safestore_checkpoint() ;

            /**
             @brief @userdesc Command to load a checkpoint file. (Calls the preload_checkpoint jobs, calls the MemoryManager restore_managed_memory
             method, then calls the restart jobs.)
             This is invoked when the user clicks the "Load ASCII Chkpnt" button on the sim control panel.
             @par Python Usage:
             @code trick.load_checkpoint("<file_name>") @endcode
             @param file_name - file to read checkpoint data from
             @return always 0
             */
            virtual void load_checkpoint(std::string file_name) ;

            /**
             @brief @userdesc Command to load a checkpoint file. (Calls the preload_checkpoint jobs, calls the MemoryManager restore_managed_memory
             method, then calls the restart jobs.)
             This is invoked when the user clicks the "Load ASCII Chkpnt" button on the sim control panel.
             @par Python Usage:
             @code trick.load_checkpoint("<file_name>") @endcode
             @param file_name - file to read checkpoint data from
             @param stl_restore_on - toggle whether to restore STLs
             @return always 0
             */
            virtual void load_checkpoint(std::string file_name, bool stl_restore_on) ;


            /**
             @brief @userdesc Command to load a checkpoint file. (Calls the preload_checkpoint jobs, calls the MemoryManager restore_managed_memory
             method, then calls the restart jobs.)
             @par Python Usage:
             @code trick.load_checkpoint("<file_name>") @endcode
             @param file_name - file to read checkpoint data from
             @return always 0
             */
            virtual int load_checkpoint_job() ;

            // Removed all doxygen documents for functions that have documents in the parent class since
            // Doxygen inherits the documents from the parent class automatically.

            virtual int write_s_job_execution( FILE * fp ) ;

            virtual int instrument_job_before(Trick::JobData * instrument_job) ;
            virtual int instrument_job_after(Trick::JobData * instrument_job) ;
            virtual int instrument_job_remove(std::string in_job) ;

    } ;

}

#endif

