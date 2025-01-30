
/*
PURPOSE:
    ( FrameLog base class)
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef FRAMELOG_HH
#define FRAMELOG_HH

#include <vector>
#include "trick/FrameDataRecordGroup.hh"
#include "trick/attributes.h"
#include "trick/JobData.hh"
#include "trick/Clock.hh"

namespace Trick {

    /** Data to save for each timeline sample.\n */
    struct timeline_t {
        bool trick_job;
        bool isEndOfFrame;
        bool isTopOfFrame;
        double id;
        long long start;
        long long stop;
    } ;

/**
  This class provides (optional) logging of Trick frame timing and job performance statistics.
  @author Danny Strauss
 */

    class FrameLog {

        public:

            /** Enable real-time frame logging.\n */
            bool frame_log_flag ;                  /**< trick_io(*io) trick_units(--) */

            /** Data recording groups for logging user jobs.\n */
            std::vector< Trick::FrameDataRecordGroup *> drg_users ; /**< trick_io(**) trick_units(--) */
            /** Data recording group for logging trick jobs.\n */
            Trick::FrameDataRecordGroup * drg_trick;    /**<  trick_io(*io) trick_units(--) */
            /** Data recording group for logging frame/overrun time.\n trick_units(--) */
            Trick::FrameDataRecordGroup * drg_frame;    /**<  trick_io(*io) trick_units(--) */

            unsigned int plots_per_page;              /**< trick_io(*io) trick_units(--) number of plots per page */
            /** Cyclic jobs timeline to log, dimensioned as [num_threads][tl_max_samples].\n */
            Trick::timeline_t **timeline;       /**<  trick_io(**) */
            /** Non-Cyclic jobs timeline to log, dimensioned as [num_threads][tl_max_samples].\n */
            Trick::timeline_t **timeline_other; /**<  trick_io(**) */

            /** Number of threads in this sim.\n */
            int num_threads;                /**<  trick_io(**) */
            /** The maximum number of job samples we can timeline (user settable).\n */
            int tl_max_samples;             /**<  trick_io(**) */
            /** Count how many Cyclic jobs are in the timeline per thread.\n */
            int *tl_count;                  /**<  trick_io(**) */
            /** Count how many Non-Cyclic jobs are in the timeline per thread.\n */
            int *tl_other_count;            /**<  trick_io(**) */

            /** True when logging of initialization jobs started.\n */
            bool log_init_start;            /**<  trick_io(**) */
            /** True when logging of initialization jobs is done.\n */
            bool log_init_end;              /**<  trick_io(**) */
            /** For creating log_timeline logging file.\n */
            FILE *fp_time_main;             /**<  trick_io(**) */
            /** For creating log_timeline_other logging file.\n */
            FILE *fp_time_other;            /**<  trick_io(**) */
            /** Fake attributes to use for setting up data recording.\n */
            ATTRIBUTES time_value_attr ;    /**<  trick_io(**) */

            /** Save the name of the trick messaging sim object.\n */
            std::string msg_sim_object_name;     /**<  trick_io(**) */
            /** Save the name of the trick data recording sim object.\n */
            std::string dr_sim_object_name;      /**<  trick_io(**) */
            /** Save the name of the trick real-time sync sim object.\n */
            std::string rt_sim_object_name;      /**<  trick_io(**) */
            /** Save the name of the trick master/slave sim object.\n */
            std::string ms_sim_object_name;      /**<  trick_io(**) */

            Trick::Clock & clock ;               /**<  trick_io(**) */

            /**
             @brief Constructor.
            */
            FrameLog(Trick::Clock & in_clock) ;

            /**
             @brief Destructor.
            */
            ~FrameLog() ;

            /**
             @brief Instrumentation job to save job timeline start time.
             @param curr_job - pointer to self (current frame_clock_start job)
            */
            int frame_clock_start(Trick::JobData * curr_job) ;

            /**
             @brief Instrumentation job to save job timeline stop time and frame time.
             @param curr_job - pointer to self (current frame_clock_stop job)
            */
            int frame_clock_stop(Trick::JobData * curr_job) ;

            /**
             @brief @userdesc Command to turn on frame logs.
             @par Python Usage:
             @code trick.frame_log_on() @endcode
             @return always 0
            */
            int framelog_on() ;

            /**
             @brief @userdesc Command to turn off frame logs.
             @par Python Usage:
             @code trick.frame_log_off() @endcode
             @return always 0
            */
            int framelog_off() ;

            /**
             @brief @userdesc Allocates and partially sets up frame logging.
            */
            void default_data() ;

            /**
             @brief Routine to call all create routines below for the DP directory and files.
            */
            int create_DP_files() ;

            /**
             @brief @userdesc Command to set the maximum number of job timeline samples to be taken (default = 100000).
             @par Python Usage:
             @code trick.frame_log_set_max_samples(<num>) @endcode
             @param num - the max number of samples
             @return always 0
            */
            int set_max_samples(int num) ;

            /**
             @brief Clears data_record informtion realted to frame logging durning the checkpoint reload.
             @return always 0
            */
            int clear_data_record_info() ;

            /**
             @brief Restores the frame log state upon checkpoint restart
             @return always 0
            */
            int restart() ;

            /**
             @brief Shutdown job that writes the job timeline data to disk and closes log files.
             @return always 0
            */
            int shutdown() ;

            void set_clock(Trick::Clock & in_clock) ;

        private:
            std::vector<std::string> trick_jobs; // ** vector containing all trick job names
            std::vector<std::string> user_jobs;  // ** vector containing all user job names

            std::string DP_dir; // ** name of DP_Product directory

            void allocate_recording_groups() ;

            void add_recording_vars_for_jobs() ;
            void add_recording_vars_for_frame() ;

            void add_recording_groups_to_sim() ;
            void remove_recording_groups_from_sim() ;

            void enable_recording_groups() ;
            void disable_recording_groups() ;
            void init_recording_groups() ;

            void add_instrument_jobs() ;
            void remove_instrument_jobs() ;

            /**
             @brief Create the DP_Product directory where all DP files will be stored.
            */
            int create_DP_Product_dir() ;

            /**
             @brief Create the DP files to display trick and user job execution times.
            */
            int create_DP_job_files();

            /**
             @brief Create the DP files to display job timelines.
            */
            int create_DP_timeline_files();

            // This object is not copyable
            void operator =(const FrameLog &) {};

    } ;

} ;

#endif
