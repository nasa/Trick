
#include <iostream>
#include <math.h>
#include <sys/stat.h>
#if __linux
#include <sys/prctl.h>
#endif

#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"
#include "trick/TrickConstant.hh"

Trick::Executive * the_exec ;

Trick::Executive::Executive() {

    /** @par Detailed Design: */
    the_exec = this ;

    advance_sim_time_job = NULL ;
    attach_debugger = false ;
    curr_job = NULL ;

    struct stat st ;
    if ( stat("/usr/bin/gdb",&st) == 0 ) {
        debugger_command = std::string("/usr/bin/gdb") ;
    } else if ( stat("/usr/bin/lldb",&st) == 0 ) {
        debugger_command = std::string("/usr/bin/lldb") ;
    }

    enable_freeze = false ;
    except_return = 0 ;
    exec_command = NoCmd ;
    async_freeze_command = false ;
    freeze_command = false ;
    freeze_on_frame_boundary = false ;
    restart_called = false ;
    initialization_complete = false ;
    /** @li Assign default freeze_frame to be 1 second. */
    freeze_frame = 1.0 ;
    job_call_time_tics = 0 ;
    mode = Initialization ;
    num_classes = 0 ;
    num_sim_objects = 0 ;
    rt_nap = true ;
    scheduled_start_index = 1000 ;
    num_scheduled_job_classes = 0 ;
    signal_caused_term = false ;
    sim_start = 0.0 ;
    /** @li Assign default software frame to be 1 second. */
    software_frame = 1.0;
    frame_count = 0 ;
    stack_trace = true ;

    debug_pause_flag = false ;
    
    /** @li (if on new-enough Linux) allow any process to ptrace this one.
     *      This allows stack trace / debugger attach when ptrace is
     *      restricted (e.g. on Ubuntu 16).
    */
    #if defined(PR_SET_PTRACER) && defined(PR_SET_PTRACER_ANY)
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY);
    #endif
    /** @li Assign default terminate time to MAX_LONG_LONG tics. */
    terminate_time = TRICK_MAX_LONG_LONG - 1;
    time_last_pass_tics = 0 ;
    /** @li Assign default time_tic value to be 1 us. */
    time_tics = 0 ;
    freeze_time_tics = 0 ;
    freeze_frame_count = 0 ;
    old_time_tic_value = 1000000 ;
    time_tic_value = 1000000 ;

    // Set the JobData copy of the time_tic_value to the executive value
    Trick::JobData::set_time_tic_value(time_tic_value) ;

    // These trap flags are set in init_signal_handlers()
    trap_sigbus = false ;
    trap_sigfpe = false ;
    trap_sigsegv = false ;
    trap_sigabrt = false ;
    trap_sigchld = false ;

    build_date = std::string("unknown") ;
    current_version = std::string("unknown") ;
    version_date_tag = std::string("unknown") ;

    num_all_jobs = 0 ;
    all_jobs_for_checkpoint = NULL ;

    software_frame_tics = (long long)(software_frame * time_tic_value) ;
    next_frame_check_tics = software_frame_tics ;

    freeze_frame_tics = (long long)(freeze_frame * time_tic_value) ;
    next_freeze_frame_check_tics = freeze_frame_tics ;

    freeze_job = NULL ;
    advance_sim_time_job = NULL ;
    curr_job = NULL ;

    /** @li Map non-scheduled job class strings to job queues. */
    class_map["default_data"] = num_classes ;
    class_to_queue[num_classes++] = &default_data_queue ;

    class_map["input_processor"] = num_classes ;
    class_to_queue[num_classes++] = &input_processor_queue ;

    class_map["initialization"] = num_classes ;
    class_to_queue[num_classes++] = &initialization_queue ;

    class_map["input_processor_run"] = num_classes ;
    class_to_queue[num_classes++] = &input_processor_run_queue ;

    class_map["system_thread_sync"] = num_classes ;
    class_to_queue[num_classes++] = &thread_sync_queue ;

    class_map["top_of_frame"] = num_classes ;
    class_to_queue[num_classes++] = &top_of_frame_queue ;

    class_map["end_of_frame"] = num_classes ;
    class_to_queue[num_classes++] = &end_of_frame_queue ;

    class_map["shutdown"] = num_classes ;
    class_to_queue[num_classes++] = &shutdown_queue ;

    class_map["freeze_init"] = num_classes ;
    class_to_queue[num_classes++] = &freeze_init_queue ;

    class_map["freeze_scheduled"] = num_classes ;
    class_to_queue[num_classes++] = &freeze_scheduled_queue ;

    class_map["freeze_automatic"] = num_classes ;
    class_to_queue[num_classes++] = &freeze_scheduled_queue ;

    class_map["freeze"] = num_classes ;
    class_to_queue[num_classes++] = &freeze_queue ;

    class_map["unfreeze"] = num_classes ;
    class_to_queue[num_classes++] = &unfreeze_queue ;

    class_map["exec_time_tic_changed"] = num_classes ;
    class_to_queue[num_classes++] = &time_tic_changed_queue ;


    // Initialize all of default signal handlers
    init_signal_handlers() ;
}

Trick::Executive::~Executive() {
    while ( ! threads.empty() ) {
        delete threads.back() ;
        threads.pop_back() ;
    }
}

int Trick::Executive::get_all_jobs_vector(std::vector<Trick::JobData *> & in_jobs_vector) {
    in_jobs_vector = all_jobs_vector ;
    return(0) ;
}

bool Trick::Executive::get_attach_debugger() {
    return(attach_debugger) ;
}

const std::string & Trick::Executive::get_current_version() {
    return(current_version) ;
}

const std::string & Trick::Executive::get_debugger_command() {
    return(debugger_command) ;
}

SIM_COMMAND Trick::Executive::get_exec_command() {
    return(exec_command) ;
}

bool Trick::Executive::get_enable_freeze() {
    return(enable_freeze) ;
}

bool Trick::Executive::get_freeze_command() {
    return(freeze_command) ;
}

bool Trick::Executive::get_freeze_on_frame_boundary() {
    return(freeze_on_frame_boundary) ;
}

double Trick::Executive::get_freeze_frame() {
    return(freeze_frame) ;
}

long long Trick::Executive::get_freeze_frame_tics() {
    return(freeze_frame_tics) ;
}

SIM_MODE Trick::Executive::get_mode() {
    return(mode) ;
}

unsigned int Trick::Executive::get_num_threads() {
    return(threads.size()) ;
}

int Trick::Executive::get_old_time_tic_value() {
    return(old_time_tic_value) ;
}

bool Trick::Executive::get_rt_nap() {
    return(rt_nap) ;
}

int Trick::Executive::get_scheduled_start_index() {
    return(scheduled_start_index) ;
}

int Trick::Executive::get_sim_objects(std::vector<Trick::SimObject *> & in_sim_objects) {
    in_sim_objects = sim_objects ;
    return(0) ;
}

Trick::SimObject * Trick::Executive::get_sim_object_by_name(std::string sim_object_name) {
    for ( int ii = 0 ; ii < sim_objects.size() ; ii++ ) {
        if ( ! sim_objects[ii]->name.compare(sim_object_name) ) {
            return sim_objects[ii];
        }
    }

    return NULL;
}

double Trick::Executive::get_software_frame() {
    return(software_frame) ;
}

long long Trick::Executive::get_software_frame_tics() {
    return(software_frame_tics) ;
}

long long Trick::Executive::get_frame_count() {
    return(frame_count) ;
}

long long Trick::Executive::get_freeze_frame_count() {
    return(freeze_frame_count) ;
}

Trick::Threads * Trick::Executive::get_thread(unsigned int thread_id) {
    if ( thread_id < threads.size() ) {
        return threads[thread_id] ;
    }
    return NULL ;
}

pthread_t Trick::Executive::get_pthread_id(unsigned int thread_id) {
    if ( thread_id < threads.size() ) {
        return threads[thread_id]->get_pthread_id() ;
    }
    return 0 ;
}

bool Trick::Executive::get_stack_trace() {
    return(stack_trace) ;
}

double Trick::Executive::get_thread_amf_cycle_time(unsigned int thread_id) {
    if ( thread_id < threads.size() ) {
        return threads[thread_id]->amf_cycle ;
    }
    return -1.0 ;
}

double Trick::Executive::get_terminate_time() {
    return(terminate_time / time_tic_value) ;
}

int Trick::Executive::get_time_tic_value() {
    return(time_tic_value) ;
}

long long Trick::Executive::get_freeze_time_tics() {
    return(freeze_time_tics) ;
}

bool Trick::Executive::get_trap_sigbus() {
    return(trap_sigbus) ;
}

bool Trick::Executive::get_trap_sigfpe() {
    return(trap_sigfpe) ;
}

bool Trick::Executive::get_trap_sigsegv() {
    return(trap_sigsegv) ;
}

bool Trick::Executive::get_trap_sigabrt() {
    return(trap_sigabrt) ;
}

bool Trick::Executive::get_trap_sigchld() {
    return(trap_sigchld) ;
}

void Trick::Executive::reset_job_cycle_times() {
    unsigned int ii ;
    for ( ii = 0 ; ii < all_jobs_vector.size() ; ii++ ) {
        Trick::JobData * temp_job = all_jobs_vector[ii] ;
        temp_job->calc_cycle_tics() ;
    }
    return ;
}

void Trick::Executive::reset_job_call_times() {
    unsigned int ii ;
    for ( ii = 0 ; ii < all_jobs_vector.size() ; ii++ ) {
        Trick::JobData * temp_job = all_jobs_vector[ii] ;
        temp_job->start_tics = (long long)round((double)temp_job->start_tics / old_time_tic_value * time_tic_value) + time_tics ;

        if ( temp_job->disabled == false and
             (!temp_job->system_job_class or !temp_job->job_class_name.compare("system_advance_sim_time")) ) {
            if ( ! temp_job->job_class_name.compare("integ_loop")) {
                temp_job->next_tics  = temp_job->cycle_tics + temp_job->start_tics ;
            } else {
                temp_job->next_tics  = temp_job->start_tics ;
            }
        }
    }
    return ;
}

int Trick::Executive::set_attach_debugger(bool on_off) {
    attach_debugger = on_off ;
    return(0) ;
}

int Trick::Executive::set_debugger_command(std::string command) {
    debugger_command = command ;
    return(0) ;
}

int Trick::Executive::set_exec_command(SIM_COMMAND in_command) {
    exec_command = in_command ;
    return(0) ;
}

int Trick::Executive::set_enable_freeze(bool on_off) {
    enable_freeze = on_off ;
    return(0) ;
}

int Trick::Executive::set_freeze_command(bool on_off) {
    freeze_command = on_off ;
    return(0) ;
}

int Trick::Executive::set_freeze_on_frame_boundary(bool on_off) {
    freeze_on_frame_boundary = on_off ;
    return(0) ;
}

int Trick::Executive::set_freeze_frame(double in_frame) {
    freeze_frame = in_frame ;
    freeze_frame_tics = (long long)(freeze_frame * time_tic_value) ;
    return(0) ;
}

int Trick::Executive::set_rt_nap(bool on_off) {
    rt_nap = on_off ;
    return(0) ;
}

int Trick::Executive::set_software_frame(double in_frame) {
    software_frame = in_frame ;
    software_frame_tics = (long long)(software_frame * time_tic_value) ;
    next_frame_check_tics = (long long)((sim_start * time_tic_value) + software_frame_tics) ;
    return(0) ;
}

int Trick::Executive::set_stack_trace(bool on_off) {
    stack_trace = on_off ;
    return(0) ;
}

int Trick::Executive::set_terminate_time(double in_time) {
    terminate_time = (long long)(in_time * time_tic_value) ;
    return(0) ;
}

int Trick::Executive::set_time(double in_time) {
    time_tics = (long long)(in_time * time_tic_value) ;
    for (unsigned int ii = 0 ; ii < threads.size() ; ii++ ) {
       threads[ii]->curr_time_tics = time_tics ;
    }
    reset_job_call_times() ;
    return(0) ;
}

int Trick::Executive::set_time_tics(long long in_tics) {
    time_tics = in_tics ;
    reset_job_call_times() ;
    return(0) ;
}

int Trick::Executive::set_version_date_tag(std::string tag) {
    this->version_date_tag = tag ;
    return(0) ;
}

int Trick::Executive::set_build_date(std::string date) {
    this->build_date = date ;
    return(0) ;
}

int Trick::Executive::set_current_version(std::string version) {
    this->current_version = version ;
    return(0) ;
}

int Trick::Executive::get_except_return() const {
    return except_return;
}
