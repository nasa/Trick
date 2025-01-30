
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "trick/FrameLog.hh"
#include "trick/FrameDataRecordGroup.hh"
#include "trick/exec_proto.hh"
#include "trick/exec_proto.h"
#include "trick/data_record_proto.h"
#include "trick/command_line_protos.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

Trick::FrameLog * the_fl = NULL ;

//Constructor.
Trick::FrameLog::FrameLog(Trick::Clock & in_clock) : 
 frame_log_flag(false),
 drg_trick(NULL),
 drg_frame(NULL),
 plots_per_page(6),
 timeline(NULL),
 timeline_other(NULL),
 num_threads(1),
 tl_max_samples(100000),
 tl_count(NULL),
 tl_other_count(NULL),
 log_init_start(false),
 log_init_end(false),
 fp_time_main(NULL),
 fp_time_other(NULL),
 clock(in_clock) {

    time_value_attr.type = TRICK_DOUBLE;
    time_value_attr.size = sizeof(double);
    time_value_attr.units = strdup("s") ;

    the_fl = this ;

}

Trick::FrameLog::~FrameLog() {
    free((char *)time_value_attr.units) ;
    // leaking all of the FrameDataRecordGroups
    // leaking timeline memory.
}

/**
@details

This routine is called during the "default_data" phase of initialization.  At that time
the number of threads in the sim will be known and this ensure that everything is
allocated before the input_processor is initially run.

-# Allocate a FrameDataRecordGroup to hold user job information on the main thread
-# Push the FrameDataRecordGroup to drg_users.
-# For each child thread in the sim
 -# Allocate a FrameDataRecordGroup to hold user job information on the child thread
 -# Push the FrameDataRecordGroup to drg_users.
-# Allocate a FrameDataRecordGroup to hold trick job information
-# Allocate a FrameDataRecordGroup to hold frame information
-# Allocate space for timeline for each thread.
*/
void Trick::FrameLog::allocate_recording_groups() {

    Trick::FrameDataRecordGroup * fdrg ;
    int ii ;

    /* Create log_user_jobs group and log_trick_jobs group to record job frame_times. */
    fdrg = new ("trick_frame_userjobs_main") Trick::FrameDataRecordGroup(0, "frame_userjobs_main") ;
    drg_users.push_back(fdrg) ;
    num_threads = exec_get_num_threads();
    for ( ii = 1 ; ii < num_threads ; ii++ ) {
        std::ostringstream group_name ;
        group_name << "trick_frame_userjobs_C" << ii ;
        fdrg = new (group_name.str().c_str()) Trick::FrameDataRecordGroup(ii, group_name.str()) ;
        drg_users.push_back(fdrg) ;
    }
    drg_trick = new ("trick_frame_trick_jobs") FrameDataRecordGroup(0, "frame_trickjobs") ;
    drg_frame = new ("trick_frame") FrameDataRecordGroup(0, "frame") ;

    /* Allocate space for timeline for each thread according to user settable tl_max_samples variable. */
    timeline = (Trick::timeline_t **)calloc( num_threads, sizeof(Trick::timeline_t*));
    timeline_other = (Trick::timeline_t **)calloc( num_threads, sizeof(Trick::timeline_t*));
    for (ii=0; ii<num_threads; ii++) {
        timeline[ii] = (Trick::timeline_t *)calloc( tl_max_samples, sizeof(Trick::timeline_t));
        timeline_other[ii] = (Trick::timeline_t *)calloc( tl_max_samples, sizeof(Trick::timeline_t));
    }
    tl_count = (int *)calloc( num_threads, sizeof(int));
    tl_other_count = (int *)calloc( num_threads, sizeof(int));
}

void Trick::FrameLog::add_recording_vars_for_jobs() {

    unsigned int ii , dot ;
    REF2 * new_ref ;
    char * job_name = NULL ;
    std::vector<Trick::JobData *> all_jobs_vector ;

    exec_get_all_jobs_vector(all_jobs_vector) ;

    for ( ii = 0 ; ii < all_jobs_vector.size() ; ii++ ) {
        /** @li only makes sense to record frame time for scheduled, integ, and end_of_frame jobs */
        /** @li exclude system jobs because they are excluded in instrument_before/after */
        /** @li exclude frame_log jobs */
        if ( (all_jobs_vector[ii]->job_class >= exec_get_scheduled_start_index() ||
             (! all_jobs_vector[ii]->job_class_name.compare("integration")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("derivative")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("dynamic_event")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("post_integration")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("system_thread_sync")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("top_of_frame")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("end_of_frame")))
           ) {
            dot = all_jobs_vector[ii]->name.find_first_of(".");
            if (!all_jobs_vector[ii]->name.compare(dot,11,".frame_log.")) {
                continue;
            }

            new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
            /** @li add job frame id, job class and cycle time to displayed job name, and prepend "JOB_" so it stands out in quickplot */
            if ( all_jobs_vector[ii]->job_class >= exec_get_scheduled_start_index() ) {
                if ( all_jobs_vector[ii]->thread == 0 ) {
                    asprintf(&job_name, "JOB_%s.%2.2f(%s_%2.3f)", all_jobs_vector[ii]->name.c_str(),
                        all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str(), all_jobs_vector[ii]->cycle);
                } else { // add child thread # to name
                    asprintf(&job_name, "JOB_%s_C%d.%2.2f(%s_%2.3f)",  all_jobs_vector[ii]->name.c_str(), all_jobs_vector[ii]->thread,
                        all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str(), all_jobs_vector[ii]->cycle);
                }
            } else { // non-scheduled class
                asprintf(&job_name, "JOB_%s.%2.2f(%s)", all_jobs_vector[ii]->name.c_str(),
                    all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str());
            }
            // replace any colons in (C++) job name with underscores
            char *colon = strchr(job_name, ':') ;
            while (colon) {
                colon[0] = '#';
                colon = strchr(job_name, ':') ;
            }

            new_ref->reference = job_name;
            new_ref->address = &(all_jobs_vector[ii]->frame_time_seconds) ;
            new_ref->attr = &time_value_attr ;
            /** @li use TRK tag in S_define to identify trick jobs */
            // trick jobs
            if ( all_jobs_vector[ii]->tags.count("TRK") ) {
                trick_jobs.push_back(std::string(job_name));
                drg_trick->add_variable(new_ref) ;
                drg_trick->add_rec_job(all_jobs_vector[ii]) ;
                // save names of trick sim objects (ones that user can change) so we can color code jobs in DP file
                if (!all_jobs_vector[ii]->name.compare(dot,5,".drd.")) {
                    dr_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                if (!all_jobs_vector[ii]->name.compare(dot,8,".master.")) {
                    ms_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                if (!all_jobs_vector[ii]->name.compare(dot,9,".rt_sync.")) {
                    rt_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                //TODO: no message jobs yet - fill this in when messaging has a job to look at here
                // if () {
                    msg_sim_object_name = std::string("message");
                //}
            // user jobs
            } else {
                user_jobs.push_back(std::string(job_name));
                drg_users[all_jobs_vector[ii]->thread]->add_variable(new_ref) ;
                drg_users[all_jobs_vector[ii]->thread]->add_rec_job(all_jobs_vector[ii]) ;
            }
        }
    }

    /* set the recoring job be and end of frame job.  Set each recording group to it's proper thread. */
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() , ii = 0 ; fdrg_it != drg_users.end() ; ++fdrg_it , ii++ ) {
        (*fdrg_it)->set_job_class("end_of_frame") ;
        (*fdrg_it)->set_phase(65533);
        (*fdrg_it)->set_thread(ii);
        if ( ii > 0 ) {
            std::ostringstream group_name ;
            group_name << "trick_frame_userjobs_C" << ii ;
            (*fdrg_it)->add_variable( group_name.str() + ".frame_time") ;
        }
    }
    drg_trick->set_job_class("end_of_frame") ;
    drg_trick->set_phase(65535);

}

/**
@details
-# Add the log_userjob frame time we created above to the log_frame group
-# Add the log_trickjob frame time we created above to the log_frame group
-# Set the recording job data_record_group.frame to end of frame -
   phase it last (after rt_monitor) because time set in rt_monitor
*/
void Trick::FrameLog::add_recording_vars_for_frame() {

    REF2 * new_ref ;
    char * job_name = NULL ;
    int ii ;

    drg_frame->add_variable(rt_sim_object_name + std::string(".rt_sync.frame_time")) ;
    drg_frame->add_variable(rt_sim_object_name + std::string(".rt_sync.frame_overrun")) ;

    /* add the log_userjob frame time we created above to the log_frame group */
    for ( ii = 0 ; ii < num_threads ; ii++ ) {
        new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
        if (ii > 0) {
            asprintf(&job_name, "JOB_data_record_group_frame_userjobs_C%d.data_record.%2.2f(end_of_frame)",
             ii,drg_users[ii]->write_job->frame_id);
        } else {
            asprintf(&job_name, "JOB_data_record_group_frame_userjobs.data_record.%2.2f(end_of_frame)",
             drg_users[ii]->write_job->frame_id);
        }
        trick_jobs.push_back(std::string(job_name));
        new_ref->reference = job_name;
        new_ref->address = &(drg_users[ii]->write_job->frame_time_seconds);
        new_ref->attr = &time_value_attr ;
        drg_frame->add_variable(new_ref) ;
        drg_frame->add_rec_job(drg_users[ii]->write_job) ;
    }

    /* add the log_trickjob frame time we created above to the log_frame group */
    new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
    asprintf(&job_name, "JOB_data_record_group.trickjobs.%2.2f(end_of_frame)",drg_trick->jobs[0]->frame_id);
    trick_jobs.push_back(std::string(job_name));
    new_ref->reference = job_name;
    new_ref->address = &(drg_trick->write_job->frame_time_seconds);
    new_ref->attr = &time_value_attr ;
    drg_frame->add_variable(new_ref) ;
    drg_frame->add_rec_job(drg_trick->write_job) ;

    /* set the recording job data_record_group.frame to end of frame -
       phase it last (after rt_monitor) because time set in rt_monitor */
    drg_frame->set_job_class("end_of_frame") ;
    drg_frame->set_phase(65535);
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call add_data_record_group
-# Call add_data_record_group for drg_trick
-# Call add_data_record_group for drg_frame
*/
void Trick::FrameLog::add_recording_groups_to_sim() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; ++fdrg_it ) {
        add_data_record_group( *fdrg_it , Trick::DR_Ring_Buffer) ;
    }
    add_data_record_group(drg_trick, Trick::DR_Ring_Buffer) ;
    add_data_record_group(drg_frame, Trick::DR_Ring_Buffer) ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call remove_data_record_group
-# Call remove_data_record_group for drg_trick
-# Call remove_data_record_group for drg_frame
*/
void Trick::FrameLog::remove_recording_groups_from_sim() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; ++fdrg_it ) {
        remove_data_record_group( *fdrg_it) ;
    }
    remove_data_record_group(drg_trick) ;
    remove_data_record_group(drg_frame) ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call enable
-# Call enable for drg_trick
-# Call enable for drg_frame
*/
void Trick::FrameLog::enable_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; ++fdrg_it ) {
        (*fdrg_it)->enable() ;
    }
    drg_trick->enable() ;
    drg_frame->enable() ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call disable
-# Call disable for drg_trick
-# Call disable for drg_frame
*/
void Trick::FrameLog::disable_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; ++fdrg_it ) {
        (*fdrg_it)->disable() ;
    }
    drg_trick->disable() ;
    drg_frame->disable() ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call init
-# Call init for drg_trick
-# Call init for drg_frame
*/
void Trick::FrameLog::init_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; ++fdrg_it ) {
        (*fdrg_it)->init() ;
    }
    drg_trick->init() ;
    drg_frame->init() ;
}

/**
@details
-# Insert frame log clock jobs in queue before and after all jobs we want to log the time of.
*/
void Trick::FrameLog::add_instrument_jobs() {
    exec_instrument_before("trick_frame_log.frame_log.frame_clock_start") ;
    exec_instrument_after("trick_frame_log.frame_log.frame_clock_stop") ;
}

/**
@details
-# Removes frame log clock jobs in queue before and after all jobs.
*/
void Trick::FrameLog::remove_instrument_jobs() {
    exec_instrument_remove("trick_frame_log.frame_log.frame_clock_start") ;
    exec_instrument_remove("trick_frame_log.frame_log.frame_clock_stop") ;
}

//Instrumentation job to save job timeline start time.
int Trick::FrameLog::frame_clock_start(Trick::JobData * curr_job ) {

    Trick::JobData * target_job = (Trick::JobData *)curr_job->sup_class_data ;

    /** @par Detailed Design: */
    if ( target_job != NULL ) {
        /** @li Set target job's start time. */
        target_job->rt_start_time = clock.clock_time() ;
    }

    return(0) ;

}

//Instrumentation job to save job timeline stop time and frame time.
int Trick::FrameLog::frame_clock_stop(Trick::JobData * curr_job) {

    Trick::JobData * target_job = (Trick::JobData *)curr_job->sup_class_data ;
    int thread, mode;
    double time_scale;
    
    /** @par Detailed Design: */
    if ( target_job != NULL ) {
        if ( target_job->rt_start_time >= 0 ) {
            /** @li Set current job's stop time and frame time. */
            target_job->rt_stop_time = clock.clock_time() ;
            time_scale = 1.0 / target_job->time_tic_value;
            target_job->frame_time += (target_job->rt_stop_time - target_job->rt_start_time);
            target_job->frame_time_seconds = target_job->frame_time * time_scale;
            thread = target_job->thread;

            mode = exec_get_mode();
            /** @li Whatever inits run after & including start_realtime, make them be timelined as cyclic. */
            if (mode == Initialization) {
                if (! target_job->name.compare(rt_sim_object_name + std::string(".rt_sync.start_realtime")) ) {
                    log_init_end = true;
                    // fixup: set start_realtime function's start time to 0 because it will have reset the clock
                    target_job->rt_start_time = 0;
                }
                if (log_init_end) {
                    mode = Run;
                }
            }

            /** @li Save all cyclic job start & stop times for this frame into timeline structure. */
            if ((mode==Run) || (mode==Step)) {                            // cyclic job
                if (tl_count[thread] < tl_max_samples) {
                    timeline[thread][tl_count[thread]].id    = target_job->frame_id;
                    timeline[thread][tl_count[thread]].start = target_job->rt_start_time;
                    timeline[thread][tl_count[thread]].stop  = target_job->rt_stop_time;
                    timeline[thread][tl_count[thread]].trick_job = target_job->tags.count("TRK");
                    timeline[thread][tl_count[thread]].isEndOfFrame = target_job->isEndOfFrame;
                    timeline[thread][tl_count[thread]].isTopOfFrame = target_job->isTopOfFrame;
                    tl_count[thread]++;
                }
            /** @li Save all non-cyclic job start & stop times for this frame into timeline_other structure. */
            } else {                                                      // non-cyclic job
                if (tl_other_count[thread] < tl_max_samples) {
                    timeline_other[thread][tl_other_count[thread]].id    = target_job->frame_id;
                    timeline_other[thread][tl_other_count[thread]].start = target_job->rt_start_time;
                    timeline_other[thread][tl_other_count[thread]].stop  = target_job->rt_stop_time;
                    timeline_other[thread][tl_other_count[thread]].trick_job = target_job->tags.count("TRK");
                    tl_other_count[thread]++;
                }
            }
            // start timeline over
            target_job->rt_start_time = 0;
            target_job->rt_stop_time = 0;
        } else {
            target_job->frame_time = 0 ;
            target_job->frame_time_seconds = 0.0;
        }
    }

    return(0) ;

}

/**
@details
-# If we are enabled already, return
-# If the frame log data recording groups are not in the sim
 -# Add the frame log data recording groups to the sim
 -# Initialize the frame log data recording groups.
-# Add instrument jobs
-# Enable the recording groups
-# Set the frame log flag to true
*/
int Trick::FrameLog::framelog_on() {

    if ( frame_log_flag == true ) {
        return(0) ;
    }
    // test if we have added the frame logging objects to the sim or not.
    // If we haven't add them now and initialize the data recording groups.
    // We do this in framelog_on so we don't add unnecessary jobs and create
    // log_rt files until we have to.
    if ( get_data_record_group(drg_trick->get_group_name()) == NULL ) {
        add_recording_groups_to_sim() ;
        init_recording_groups() ;
    }
    add_instrument_jobs() ;
    enable_recording_groups() ;
    frame_log_flag = true ;
    return(0) ;
}

/**
@details
-# If we are disabled already, return
-# Remove instrument jobs
-# Disable the recording groups
-# Set the frame log flag to false.
*/
int Trick::FrameLog::framelog_off() {

    if ( frame_log_flag == false ) {
        return(0) ;
    }
    remove_instrument_jobs() ;
    disable_recording_groups() ;
    frame_log_flag = false ;
    return(0) ;
}

/**
@details
Command to set the maximum number of job timeline samples to be taken (default = 100000).
-# If num > 0
 -# Set new maximum to num.
 -# For each thread
  -# Reallocate the timeline data array
  -# Reallocate the timeline_other data array
*/
int Trick::FrameLog::set_max_samples(int num) {
    int ii ;
    if (num > 0) {
        tl_max_samples = num ;
        for (ii=0; ii<num_threads; ii++) {
            timeline[ii] = (Trick::timeline_t *)realloc( timeline[ii], tl_max_samples*sizeof(Trick::timeline_t));
            timeline_other[ii] = (Trick::timeline_t *)realloc( timeline_other[ii], tl_max_samples*sizeof(Trick::timeline_t));
        }
        std::vector< Trick::FrameDataRecordGroup *>::iterator it ;
        for ( it = drg_users.begin() ; it != drg_users.end() ; ++it ) {
            (*it)->set_max_buffer_size(num) ;
        }
        drg_trick->set_max_buffer_size(num) ;
        drg_frame->set_max_buffer_size(num) ;
    }
    return(0) ;
}

/**
@details
-# Allocate memory for all of the frame log recording groups.
-# Add variables for both frame and individual job recording groups.
-# Create DP files.
*/
void Trick::FrameLog::default_data() {
    allocate_recording_groups() ;
    add_recording_vars_for_jobs() ;
    add_recording_vars_for_frame() ;
    // reset clock before frame logging
    clock.clock_reset(0);
}

/**
@details

Clears any data_record information related to frame logging from the checkpoint reload.
This job must run before DataRecordGroup::restart.

DataRecordGroups save the list of variables it was recording when a checkpoint is taken.
This list is used by DataRecordGroups to restart data recording.
This list doesn't work with frame log groups because frame logging creates the
data record list of varaibles differently than normal recording groups.  So, we need
to erase the checkpointed list of frame log variables.  The correct variables will
be inserted during FrameLog::restart which is run after DataRecordGroup::restart.

-# For each FrameDataRecordGroup in drg_users
 -# Call clear_checkpoint_vars
-# Call clear_checkpoint_vars for drg_trick
-# Call clear_checkpoint_vars for drg_frame
*/
int Trick::FrameLog::clear_data_record_info() {

    remove_recording_groups_from_sim() ;

    std::vector< Trick::FrameDataRecordGroup *>::iterator it ;
    for ( it = drg_users.begin() ; it != drg_users.end() ; ++it ) {
        (*it)->clear_checkpoint_vars() ;
    }
    drg_trick->clear_checkpoint_vars() ;
    drg_frame->clear_checkpoint_vars() ;

    return 0 ;
}

/**
@details

Restart job that restores framelogging on checkpoint restart.  This is run after
clear_data_record_info.

-# Call data record group restart jobs
-# Add data recording variables for the jobs and frame
-# If frame log is on in checkpoint, turn frame log on.
*/
int Trick::FrameLog::restart() {
// removing the data record groups removed the restart jobs too.  call them here.
    std::vector< Trick::FrameDataRecordGroup *>::iterator it ;
    for ( it = drg_users.begin() ; it != drg_users.end() ; ++it ) {
        (*it)->restart() ;
    }
    drg_trick->restart() ;
    drg_frame->restart() ;

    add_recording_vars_for_jobs() ;
    add_recording_vars_for_frame() ;
    create_DP_files();

    // if frame log is on in the checkpoint, turn it back on now
    if ( frame_log_flag == true ) {
        frame_log_flag = false ;
        remove_instrument_jobs() ; // these will be added back when frame log turned on
        framelog_on() ;
    }
    return 0 ;
}

//Shutdown job that writes the job timeline data to disk and closes log files.
int Trick::FrameLog::shutdown() {

    /** @par Detailed Design: */

    int thread, ii, jj;
    char log_buff[128];
    Trick::timeline_t *tl;
    double start, stop, time_scale;

    if ( frame_log_flag == false ) {
        return(0) ;
    }


// ================================================================
// NEW Time-line for Jperf
// ================================================================
    for (int thread_num = 0; thread_num < num_threads; thread_num ++) {

        if (thread_num == 0) {
            snprintf(log_buff, sizeof(log_buff), "%s/log_newtimeline.csv", command_line_args_get_output_dir());
        } else {
            snprintf(log_buff, sizeof(log_buff), "%s/log_newtimelineC%d.csv", command_line_args_get_output_dir(), thread_num);
        }

        FILE *fp_log;
        if ((fp_log = fopen(log_buff, "w")) == NULL) {
            message_publish(MSG_ERROR, "Could not open log_timeline.csv file for Job Timeline Logging\n") ;
            exit(0);
        }

        fprintf(fp_log,"jobID,startTime,stopTime\n");

        time_scale = 1.0 / exec_get_time_tic_value();
        tl = timeline[thread_num];
        for ( ii = 0 ; ii < tl_count[thread_num] ; ii++ ) {
            start = tl[ii].start * time_scale;
            stop =  tl[ii].stop  * time_scale;
            fprintf(fp_log,"%5.2f, %f, %f\n", tl[ii].id, start, stop);
        }
        fflush(fp_log);
        fclose(fp_log);
    }

    /** @li Manually create the log_timeline and log_timeline_init files from saved timeline data. */
    if (fp_time_main == NULL) {


        snprintf(log_buff, sizeof(log_buff), "%s/log_timeline.csv", command_line_args_get_output_dir());
        if ((fp_time_main = fopen(log_buff, "w")) == NULL) {
            message_publish(MSG_ERROR, "Could not open log_timeline.csv file for Job Timeline Logging\n") ;
            exit(0);
        }
        fprintf(fp_time_main, "trick_frame_log.frame_log.job_time {s},");
        fprintf(fp_time_main, "trick_frame_log.frame_log.job_trick_id {--},frame_log.frame_log.job_user_id {--}");

        for (jj=1; jj<num_threads; jj++) {
            fprintf(fp_time_main, ",trick_frame_log.frame_log.job_userC%d_id {--}",jj);
        }
        fprintf(fp_time_main, "\n");



        snprintf(log_buff, sizeof(log_buff), "%s/log_timeline_init.csv", command_line_args_get_output_dir());
        if ((fp_time_other = fopen(log_buff, "w")) == NULL) {
            message_publish(MSG_ERROR, "Could not open log_timeline_init.csv file for Job Timeline Logging\n") ;
            exit(0);
        }
        fprintf(fp_time_other, "trick_frame_log.frame_log.job_init_time {s},");
        fprintf(fp_time_other, "trick_frame_log.frame_log.job_trickinit_id {--},trick_frame_log.frame_log.job_userinit_id {--}\n");



    }

    time_scale = 1.0 / exec_get_time_tic_value();
            // print to log like this:
            // (only one of the job ids will be filled in depending on what type of job this is)
            //               start job time, 0, 0
            //               start job time, trick job id, user job id
            //               stop  job time, trick job id, user job id
            //               stop  job time, 0, 0
    /** @li print a 0 id before each start time & after each stop time for a stairstep effect in plot. */
    // cyclic jobs

    for ( thread = 0 ; thread < num_threads ; thread++ ) {
        tl = timeline[thread];
        for ( ii = 0 ; ii < tl_count[thread] ; ii++ ) {
            // start & stop time are in tics, so convert to seconds
            start = tl[ii].start * time_scale;
            stop =  tl[ii].stop  * time_scale;

            fprintf(fp_time_main,      "%f,0", start);        // start stairstep
            for (jj=0; jj<num_threads; jj++) {
                fprintf(fp_time_main,  ",0");
            }
            fprintf(fp_time_main,      "\n");

            if (tl[ii].trick_job) {
                fprintf(fp_time_main, "%f,%f", start, tl[ii].id);    // trick job start
                for (jj=0; jj<num_threads; jj++) {
                    fprintf(fp_time_main, ",0");
                }
            } else { // user job
                fprintf(fp_time_main, "%f,0", start);         // user job start
                for (jj=0; jj<num_threads; jj++) {
                    if (jj==thread) {
                        fprintf(fp_time_main, ",%f", tl[ii].id);     // user thread id (0=main)
                    } else {
                        fprintf(fp_time_main, ",0");
                    }
                }
            }
            fprintf(fp_time_main,      "\n");
            if (tl[ii].trick_job) {
                fprintf(fp_time_main, "%f,%f", stop, tl[ii].id);    // trick job end
                for (jj=0; jj<num_threads; jj++) {
                    fprintf(fp_time_main, ",0");
                }
            } else { // user job
                fprintf(fp_time_main, "%f,0", stop);         // user job end
                for (jj=0; jj<num_threads; jj++) {
                    if (jj==thread) {
                        fprintf(fp_time_main, ",%f", tl[ii].id);    // user thread id (0=main)
                    } else {
                        fprintf(fp_time_main, ",0");
                    }
                }
            }
            fprintf(fp_time_main,      "\n");
            fprintf(fp_time_main,      "%f,0", stop);        // end stairstep
            for (jj=0; jj<num_threads; jj++) {
                fprintf(fp_time_main,  ",0");
            }
            fprintf(fp_time_main,      "\n");
        } // end for thread
    } // end for ii

    // non-cyclic jobs
    for ( thread = 0 ; thread < num_threads ; thread++ ) {
        tl = timeline_other[thread];
        for ( ii = 0 ; ii < tl_other_count[thread] ; ii++ ) {
            // start & stop time are in tics, so convert to seconds
            start = tl[ii].start * time_scale;
            stop =  tl[ii].stop  * time_scale;
            fprintf(fp_time_other, "%f,0,0\n", start);          // start stairstep
            if (tl[ii].trick_job) {
                fprintf(fp_time_other, "%f,%f,0\n", start, tl[ii].id); // trick job start
                fprintf(fp_time_other, "%f,%f,0\n", stop, tl[ii].id);  // trick job end
            } else { // user job
                fprintf(fp_time_other, "%f,0,%f\n", start, tl[ii].id); // user job start
                fprintf(fp_time_other, "%f,0,%f\n", stop, tl[ii].id);  // user job end
            }
            fprintf(fp_time_other, "%f,0,0\n", stop);           // end stairstep
        } // end for thread
    } // end for ii

    fclose(fp_time_main);
    fclose(fp_time_other);

    return(0) ;

}

void Trick::FrameLog::set_clock(Trick::Clock & in_clock) {
    clock = in_clock ;
}

//Call all the Create routines for the DP directory and all DP files.
int Trick::FrameLog::create_DP_files() {
    int ret=0;

    ret = create_DP_Product_dir();
    if (ret==0) {
        // Only create DP files if we successfully created the directory
        create_DP_job_files();
        create_DP_timeline_files();
    }

    return ret ;
}

//Create the DP_Product directory where the DP files will be stored.
int Trick::FrameLog::create_DP_Product_dir() {
    int ret=0;
    DP_dir = "DP_Product";
    ret = mkdir(DP_dir.c_str(), 0777);
    if (ret == -1) {
        if (errno == EEXIST) {
            // it's ok if the directory is already there
            ret = 0;
        } else {
            // if you can't create the DP_Product dir in the current directory, try in the output dir (-O)
            DP_dir = std::string(command_line_args_get_output_dir()) + "/DP_Product";
            ret = mkdir(DP_dir.c_str(), 0777);
            if (ret == -1) {
                if (errno == EEXIST) {
                    // it's ok if the directory is already there
                    ret = 0;
                }
            }
        }
    }

    if (ret == -1) {
        message_publish(MSG_WARNING, "Could not create DP_Product directory for Frame Logging. DP files not created.\n") ;
        perror("mkdir");
    }
    return ret;
}

//Create the DP files to display trick and user job execution times.
int Trick::FrameLog::create_DP_job_files() {

    std::vector<std::string>::iterator job_iterator;
    FILE *fpx;
    unsigned int pages, plots, total_plots, vcells, dot;
    char *bg_color;
    std::string DP_buff;
    const char *headerx = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" \
                    "<!DOCTYPE product PUBLIC \"-//Tricklab//DTD Product V1.0//EN\" \"Product.dtd\">\n\n" \
                    "<!-- Description: Plot of Y(t) vs. t, with attributes, titles, labels, units -->\n\n" \
                    "<product version=\"1.0\">\n";

    DP_buff = DP_dir + "/DP_rt_frame.xml";
    if ((fpx = fopen(DP_buff.c_str(), "w")) == NULL) {
        message_publish(MSG_WARNING, "Could not open DP_rt_frame.xml file for Frame Logging\n") ;
        return(0);
    }
    fprintf(fpx, "%s", headerx);
    fprintf(fpx, "    <title>Real-Time Frame Overrun/Underrun History</title>\n    <page>\n        <title>Real-Time Scheduling Frame</title>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>Frame Overrun/Underrun</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Frame Overrun/Underrun</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"darkgreen\" label=\"Overrun/Underrun\">%s.rt_sync.frame_overrun</var>\n", rt_sim_object_name.c_str());
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>Frame Scheduled Jobs Time</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Frame Scheduled Jobs Time</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"red\" label=\"Frame Sched Time\">%s.rt_sync.frame_time</var>\n", rt_sim_object_name.c_str());
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "    </page>\n");

    //unsigned int total_pages = (unsigned int)(drg_users.size() / plots_per_page) + 1 ;
    unsigned int total_pages ;
    if ( drg_users.size() <= 1 ) {
        total_pages = 1 ;
    } else {
        total_pages = (unsigned int)((drg_users.size() - 2)/ plots_per_page) + 1 ;
    }
    unsigned int page_count ;
    for ( page_count = 0 ; page_count < total_pages ; page_count++ ) {
        unsigned int ii = 0 ;
        // this check is to avoid empty page creation
        if ((page_count * plots_per_page + ii + 1) >= drg_users.size()) {
        	continue;
        }
        fprintf(fpx, "    <page>\n");
        for ( ii = 0 ; ii < plots_per_page and (page_count * plots_per_page + ii + 1) < drg_users.size() ; ii++ ) {
            fprintf(fpx, "        <plot grid=\"yes\">\n");
            fprintf(fpx, "            <title>Child thread %u Frame Scheduled Jobs</title>\n", (page_count * plots_per_page + ii + 1));
            fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
            fprintf(fpx, "            <yaxis> <label>Frame Scheduled Jobs Time</label> </yaxis>\n");
            fprintf(fpx, "            <curve>\n");
            fprintf(fpx, "                <var>sys.exec.out.time</var>\n");
            std::ostringstream group_name ;
            group_name << "trick_frame_userjobs_C" << (page_count * plots_per_page + ii + 1) ;
            fprintf(fpx, "                <var line_color=\"red\" label=\"Frame Sched Time\">%s.frame_time</var>\n", group_name.str().c_str());
            fprintf(fpx, "            </curve>\n");
            fprintf(fpx, "        </plot>\n");
        }
        fprintf(fpx, "    </page>\n");
    }

    fprintf(fpx, "    <table>\n        <title>Real-Time Frame Overrun/Underrun History</title>\n");
    fprintf(fpx, "        <column format=\"%%13.6f\">\n");
    fprintf(fpx, "            <label>Sim Time</label>\n            <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "        </column>\n");
    fprintf(fpx, "        <column format=\"%%13.6f\">\n");
    fprintf(fpx, "            <label>Overrun/Underrun</label>\n            <var>%s.rt_sync.frame_overrun</var>\n", rt_sim_object_name.c_str());
    fprintf(fpx, "        </column>\n");
    fprintf(fpx, "        <column format=\"%%13.6f\">\n");
    fprintf(fpx, "            <label>Frame Sched Time</label>\n            <var>%s.rt_sync.frame_time</var>\n", rt_sim_object_name.c_str());
    fprintf(fpx, "        </column>\n");
    fprintf(fpx, "    </table>\n</product>");
    fclose(fpx);

    // sort the saved job names
    sort(user_jobs.begin(), user_jobs.end());
    sort(trick_jobs.begin(), trick_jobs.end());

    // DP_rt_userjobs --------------------------------------------------------------
    int ii ;
    char numstr[21];
    for ( ii = 0 ; ii < num_threads ; ii++ ) {
        if ( ii == 0 ) {
             DP_buff = DP_dir + "/DP_rt_userjobs.xml";
        } else {
             snprintf(numstr, sizeof(numstr), "%d", ii);
             DP_buff = DP_dir + "/DP_rt_userjobs_C" + numstr + ".xml";
        }

        if ((fpx = fopen(DP_buff.c_str(), "w")) == NULL) {
            message_publish(MSG_WARNING, "Could not open DP_rt_userjobs.xml file for Frame Logging\n") ;
            return(0);
        }

        fprintf(fpx, "%s", headerx);
        fprintf(fpx, "    <title>User Job Execution History</title>\n");
        pages = 0; plots = 0;
        total_plots = drg_users[ii]->rec_buffer.size();
        std::vector <Trick::DataRecordBuffer *>::iterator drb_it ;
        bg_color = (char *)"cornsilk2";
        for ( drb_it = drg_users[ii]->rec_buffer.begin() + 1 ; drb_it != drg_users[ii]->rec_buffer.end() ; ++drb_it ) {
            if ( ! (*drb_it)->name.compare(0, 5, "frame") ) continue ;
            // plots_per_page job plots per page
            if ((plots == 0) || (plots > plots_per_page)) {
                pages++;
                vcells = (total_plots/pages > plots_per_page) * 4;
                if (pages > 1) {
                    fprintf(fpx, "    </page>\n");
                }
                fprintf(fpx, "    <page vcells=\"%d\">\n        <title>User Job Execution Times</title>\n", vcells);
                plots = 1;
            }
            fprintf(fpx, "        <plot grid=\"yes\" background_color=\"%s\">\n            <title>%s</title>\n", bg_color, (*drb_it)->name.c_str());
            fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
            fprintf(fpx, "            <yaxis> <label>Execution Time</label> </yaxis>\n");
            fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
            fprintf(fpx, "                <var line_color=\"red\">%s</var>\n", (*drb_it)->name.c_str());
            fprintf(fpx, "            </curve>\n");
            fprintf(fpx, "        </plot>\n");
            plots++;

        }
        fprintf(fpx, "    </page>\n</product>");
        fclose(fpx);
    }

    // DP_rt_trickjobs -------------------------------------------------------------
    DP_buff = DP_dir +  "/DP_rt_trickjobs.xml";
    if ((fpx = fopen(DP_buff.c_str(), "w")) == NULL) {
        message_publish(MSG_WARNING, "Could not open DP_rt_trickjobs.xml file for Frame Logging\n") ;
        return(0);
    }
    fprintf(fpx, "%s", headerx);
    fprintf(fpx, "    <title>Trick Job Execution History</title>\n");
    pages = 0; plots = 0;
    total_plots = trick_jobs.size();
    job_iterator = trick_jobs.begin();
    while (job_iterator != trick_jobs.end()) {
        dot = (*job_iterator).find_first_of(".");
        // default background color, for trick_whatever jobs and anything else we don't list here
        bg_color = (char *)"cornsilk3";
        // give the different types of trick jobs a different background color
        if (!(*job_iterator).compare(0,dot,std::string("JOB_") + msg_sim_object_name)) {
            bg_color = (char *)"lavenderblush3";
        }
        if (!(*job_iterator).compare(0,dot,std::string("JOB_") + dr_sim_object_name) ||
            !(*job_iterator).compare(0,21,std::string("JOB_data_record_group")) ) {
            bg_color = (char *)"slategray2";
        }
        if (!(*job_iterator).compare(0,dot,std::string("JOB_") + rt_sim_object_name)) {
            bg_color = (char *)"honeydew3";
        }
        if (!(*job_iterator).compare(0,dot,std::string("JOB_") + ms_sim_object_name)) {
            bg_color = (char *)"bisque2";
        }
        if (strncmp((*job_iterator).c_str(),"JOB_sys_integ",13)==0) {
            bg_color = (char *)"burlywood";
        }
        //other good colors in case you need more:
            //bg_color = "khaki3";
        // plots_per_page job plots per page
        if ((plots == 0) || (plots > plots_per_page)) {
            pages++;
            vcells = (total_plots/pages > plots_per_page) * 4;
            if (pages > 1) {
                fprintf(fpx, "    </page>\n");
            }
            fprintf(fpx, "    <page vcells=\"%d\">\n        <title>Trick Job Execution Times</title>\n", vcells);
            plots = 1;
        }
        fprintf(fpx, "        <plot grid=\"yes\" background_color=\"%s\">\n            <title>%s</title>\n", bg_color, (*job_iterator).c_str());
        fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
        fprintf(fpx, "            <yaxis> <label>Execution Time</label> </yaxis>\n");
        fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
        fprintf(fpx, "                <var line_color=\"darkgreen\">%s</var>\n", (*job_iterator).c_str());
        fprintf(fpx, "            </curve>\n");
        fprintf(fpx, "        </plot>\n");
        job_iterator++;
        plots++;
    }
    fprintf(fpx, "    </page>\n</product>");
    fclose(fpx);

    return(0) ;
}

//Create the DP files to display job timelines.
int Trick::FrameLog::create_DP_timeline_files() {

    FILE *fpx;
    int jj;
    std::string DP_buff;
    const char *headerx = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" \
                    "<!DOCTYPE product PUBLIC \"-//Tricklab//DTD Product V1.0//EN\" \"Product.dtd\">\n\n" \
                    "<!-- Description: Plot of Y(t) vs. t, with attributes, titles, labels, units -->\n\n" \
                    "<product version=\"1.0\">\n";

    // DP_rt_timeline --------------------------------------------------------------
    DP_buff = DP_dir + "/DP_rt_timeline.xml";
    if ((fpx = fopen(DP_buff.c_str(), "w")) == NULL) {
        message_publish(MSG_WARNING, "Could not open DP_rt_timeline.xml file for Frame Logging\n") ;
        return(0);
    }

    fprintf(fpx, "%s", headerx);
    fprintf(fpx, "    <title>Job Timeline</title>\n    <page vcells=\"3\">\n        <title>Job Timeline</title>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>User Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"red\" label=\"User Job Id\">trick_frame_log.frame_log.job_user_id</var>\n") ;
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>Trick Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"darkgreen\" label=\"Trick Job Id\">trick_frame_log.frame_log.job_trick_id</var>\n") ;
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    // don't generate the plot without a curve
    if (num_threads > 1) {
        fprintf(fpx, "        <plot grid=\"yes\">\n            <title>Child Job Timeline</title>\n");
        fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
        fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
        for (jj=1; jj<num_threads; jj++) {
            fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
            fprintf(fpx, "                <var line_color=\"blue\" label=\"Child%d Job Id\">trick_frame_log.frame_log.job_userC%d_id</var>\n",
             jj,jj);
            fprintf(fpx, "            </curve>\n");
        }
        fprintf(fpx, "        </plot>\n");
    }
    fprintf(fpx, "    </page>\n");

    fprintf(fpx, "    <page>\n        <title>Job Timeline (combined)</title>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>User and Trick Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"darkgreen\" label=\"Trick Job Id\">trick_frame_log.frame_log.job_trick_id</var>\n") ;
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"red\" label=\"User Job Id\">trick_frame_log.frame_log.job_user_id</var>\n");
    fprintf(fpx, "            </curve>\n");
    for (jj=1; jj<num_threads; jj++) {
        fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
        fprintf(fpx, "                <var line_color=\"blue\" label=\"Child%d Job Id\">trick_frame_log.frame_log.job_userC%d_id</var>\n",
                     jj,jj);
        fprintf(fpx, "            </curve>\n");
    }
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "    </page>\n</product>");
    fclose(fpx);

    // DP_rt_timeline_init ---------------------------------------------------------
    DP_buff = DP_dir + "/DP_rt_timeline_init.xml";
    if ((fpx = fopen(DP_buff.c_str(), "w")) == NULL) {
        message_publish(MSG_WARNING, "Could not open DP_rt_timeline_init.xml file for Frame Logging\n") ;
        return(0);
    }

    fprintf(fpx, "%s", headerx);
    fprintf(fpx, "    <title>Non-cyclic Job Timeline</title>\n    <page>\n        <title>Non-cyclic Job Timeline</title>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>User Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"red\" label=\"User Job Id\">trick_frame_log.frame_log.job_userinit_id</var>\n") ;
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>Trick Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"darkgreen\" label=\"Trick Job Id\">trick_frame_log.frame_log.job_trickinit_id</var>\n") ;
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "    </page>\n");

    fprintf(fpx, "    <page>\n        <title>Non-cyclic Job Timeline (combined)</title>\n");
    fprintf(fpx, "        <plot grid=\"yes\">\n            <title>User and Trick Job Timeline</title>\n");
    fprintf(fpx, "            <xaxis> <label>Time</label> <units>s</units> </xaxis>\n");
    fprintf(fpx, "            <yaxis> <label>Job Id</label> </yaxis>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"darkgreen\" label=\"Trick Job Id\">trick_frame_log.frame_log.job_trickinit_id</var>\n");
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "            <curve>\n                <var>sys.exec.out.time</var>\n");
    fprintf(fpx, "                <var line_color=\"red\" label=\"Trick Job Id\">trick_frame_log.frame_log.job_userinit_id</var>\n");
    fprintf(fpx, "            </curve>\n");
    fprintf(fpx, "        </plot>\n");
    fprintf(fpx, "    </page>\n</product>");
    fclose(fpx);

    return(0);

}
