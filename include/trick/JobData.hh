/*
PURPOSE:
    ( The job class that the executive schedules and runs )

PROGRAMMERS:
    ((Alex Lin) (NASA) (May 2009) (OO))
*/

#ifndef JOBDATA_HH
#define JOBDATA_HH

#include <string>
#include <vector>
#include <set>

#include "trick/InstrumentBase.hh"

namespace Trick {

    class SimObject ;
    class InstrumentBase ;

    /**
     * This class is the base JobData class.  Instances of this class are typically created
     * within the Trick::SimObject::add_job routine.  This class contains all of the class, cycle,
     * start, and stop times of the job.
     *
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class JobData {

        public:

            /** Job source code name */
            std::string name;                    /**< trick_units(--) */

            /** Indicates if the job is enabled */
            bool disabled;                  /**< trick_units(--) */

            /** Indicates if the job is complete */
            bool complete;                  /**< trick_units(--) */

            /** Indicates if a scheduler is handling this job */
            bool handled;                   /**< trick_units(--) */

            /** Indicates whether this is an "top_of_frame" job. */
            bool isTopOfFrame;              /**< trick_units(--) */

            /** Indicates whether this is an "end_of_frame" job. */
            bool isEndOfFrame;              /**< trick_units(--) */

            /** The cycle time */
            double cycle;                   /**< trick_units(s) */

            /** The start time */
            double start;                   /**< trick_units(s) */

            /** The stop time */
            double stop;                    /**< trick_units(s) */

            /** Supplemental data attached to this job.  Typically used internally by instrumentation jobs */
            void * sup_class_data;          /**< trick_io(**) */

            /** Job class id assigned by scheduler */
            int job_class;                  /**< trick_units(--) */

            /** Job class name as specified in S_define file */
            std::string job_class_name;          /**< trick_units(--) */

            /** Indicates a system job class only valid for "system_*" and "automatic*" jobs.  These classes of
                jobs typically reschedule themselves and do not rely on the scheduler to determine the next call time */
            int system_job_class ;          /**< trick_units(--) */

            /** Phase number  */
            unsigned short phase;           /**< trick_units(--) */

            /** Group tag names specified in S_define file */
            std::set< std::string > tags;   /**< trick_units(--) */

            typedef std::set< std::string  >::iterator TagsIterator ;
            TagsIterator tags_begin() { return tags.begin() ; } ;
            TagsIterator tags_end() { return tags.end() ; } ;

            //TODO: Move frame information to it's own structure
            /* Frame log information */

            /** Clock time when job started in frame */
            long long rt_start_time ;       /**< trick_io(**) */

            /** Clock time when job stopped in frame */
            long long rt_stop_time ;        /**< trick_io(**) */

            /** Cumulative time in tics used for job in frame (stop - start) */
            long long frame_time ;          /**< trick_io(**) */

            /** Cumulative time in seconds used for job in frame (rt_stop_time - rt_start_time) / time_tic_value */
            double frame_time_seconds;       /**< trick_io(**) trick_units(s) */

            /** Sim_object_id.id (for job identification in timeline logging) */
            double frame_id;                /**< trick_io(**) */

            /** Thread specified in the S_define file */
            unsigned int thread ;           /**< trick_units(--) */

            /** Job id assigned by CP */
            int id;                         /**< trick_units(--) */

            /** SimObject id assigned by CP */
            int sim_object_id;              /**< trick_units(--) */

            /** Pointer back to parent SimObject.  Used by scheduler to call actual job */
            SimObject * parent_object ;     /**< trick_io(**) */

            /** Depends jobs specified in S_define file, added at initialization */
            std::vector< JobData * > depends ;   /**< trick_io(**) */

            /** Instrumentation jobs to be run before this job */
            std::vector< Trick::InstrumentBase * > inst_before ;   /**< trick_io(**) */

            /** Instrumentation jobs to be run after this job */
            std::vector< Trick::InstrumentBase * > inst_after ;   /**< trick_io(**) */

            // Used internally by the scheduler integer timing.
            /** Internal cycle rate in tics */
            long long cycle_tics;           /**< trick_units(--) */

            /** Internal start rate in tics */
            long long start_tics;           /**< trick_units(--) */

            /** Internal stop rate in tics */
            long long stop_tics;            /**< trick_units(--) */

            /** Internal next rate in tics */
            long long next_tics;            /**< trick_units(--) */

            /** time tic value from the executive */
            static long long time_tic_value ;      /**< trick_io(**) */

            /** Constructor for new blank JobData instance */
            JobData() ;

            /**
             * Constructor that is typically used in S_source.cpp
             * @param in_thread - thread number specified in S_define
             * @param in_id - job id given to job by CP
             * @param in_job_class_name - job name specified in S_define
             * @param in_sup_class_data - supplemental data to attach to job
             * @param in_cycle - job cycle specified in S_define
             * @param in_name - name of job specified in S_define
             * @param in_tag - group tag name specified in S_define
             * @param in_phase - job phase specified in S_define
             * @param in_start - job start time specified in S_define
             * @param in_stop - job stop time specified in S_define
             */
            JobData(int in_thread, int in_id, std::string in_job_class_name , void* in_sup_class_data,
                    double in_cycle, std::string in_name, std::string in_tag = "", int in_phase = 60000 ,
                    double in_start = 0.0 , double in_stop = 1.0e37) ;

            /** Default destructor that doesn't do anything */
            virtual ~JobData() {} ;

            /**
             * Returns the job is handled flag
             * @return the job is handled flag
             */
            virtual bool get_handled() ;

            /**
             * Enable the job.
             */
            void enable() ;

            /**
             * Disable the job.
             */
            void disable() ;

            /**
             * Sets the job is handled flag
             * @param yes_no - requested state of the handled flag
             * @return always 0
             */
            virtual int set_handled(bool yes_no) ;

            /**
             * Sets/Resets the static time_tic value
             * @param in_time_tic_value - number of tics per second
             * @return always 0
             */
            static int set_time_tic_value(long long in_time_tic_value) ;

            /**
             * Sets/Resets the job cycle rate
             * @param rate - desired cycle rate in seconds
             * @return always 0
             */
            virtual int set_cycle(double rate) ;

            /**
             * Calculates the cycle tics based on the cycle rate an the time_tic_value
             * @return always 0
             */
            virtual int calc_cycle_tics() ;

            /**
             * Sets the next job call time greater or equal to the incoming time_tics.
             * @param time_tics - the current time in tics.
             * @return always 0
             */
            virtual int set_next_call_time(long long time_tics) ;

            /**
             * Sets the system job flag
             * @param yes_no - requested state of the system job flag
             * @return always 0
             */
            virtual int set_system_job_class(bool yes_no) ;

            /**
             * Adds another job as a dependency to this job
             * @param in_tag - JobData instance of depends_on job
             * @return always 0
             */
            virtual int add_tag( std::string in_tag) ;

            /**
             * Adds another job as a dependency to this job
             * @param depend - JobData instance of depends_on job
             * @return always 0
             */
            virtual int add_depend( JobData * depend ) ;

            /**
             * Adds an instrumentation job to the before list
             * @param in_job - JobData instance of instrumentation job
             * @return always 0
             */
            virtual int add_inst_before( InstrumentBase * in_job ) ;

            /**
             * Adds an instrumentation job to the before list
             * @param in_job - JobData instance of instrumentation job
             * @return always 0
             */
            virtual int add_inst_after( InstrumentBase * in_job ) ;

            /**
             * Removes an instrumentation job that matches the incoming job name
             * @param job_name - job name
             * @return always 0
             */
            virtual int remove_inst( std::string job_name ) ;

            /**
             * Calls the instumentation jobs and the job itself.
             * @return always 0
             */
            virtual int call() ;

            /**
             * Calls the instumentation jobs and the job itself.
             * @return always 0
             */
            virtual double call_double() ;

            /**
             * Copy command that takes checkpoint data and merges with existing
             * job with the same name.  This allows checkpoints that may have different
             * id numbers (job_id or sim_object_ids) to be restored without breaking
             * the bond between the current job_ids and the call_function routines in S_source.cpp
             * @return always 0
             */
            virtual int copy_from_checkpoint( JobData * in_job ) ;

    } ;

} ;

#endif

