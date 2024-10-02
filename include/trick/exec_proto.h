
#ifndef EXEC_PROTO_H
#define EXEC_PROTO_H

#include "trick/sim_mode.h"

#ifdef __cplusplus
extern "C" {
#endif

    int exec_get_attach_debugger(void) ;
    const char *exec_get_current_version(void) ;
    const char *exec_get_debugger_command(void) ;
    int exec_get_enable_freeze(void) ;
    SIM_COMMAND exec_get_exec_command(void) ;
    long long exec_get_frame_count(void) ;
    double exec_get_freeze_frame(void) ;
    long long exec_get_freeze_frame_count(void) ;
    int exec_get_freeze_command(void) ;
    int exec_get_freeze_on_frame_boundary(void) ;
    long long exec_get_freeze_frame_tics(void) ;
    long long exec_get_freeze_time_tics( void ) ;
    double exec_get_job_cycle(const char * job_name) ;
    SIM_MODE exec_get_mode(void) ;
    unsigned int exec_get_num_threads(void) ;
    int exec_get_old_time_tic_value( void ) ;
    unsigned int exec_get_process_id(void) ;
    int exec_get_rt_nap(void) ;
    int exec_get_scheduled_start_index(void) ;
    double exec_get_sim_time(void) ;
    double exec_get_software_frame(void) ;
    long long exec_get_software_frame_tics(void) ;
    int exec_get_stack_trace(void) ;
    double exec_get_terminate_time(void) ;
    double exec_get_thread_amf_cycle_time(unsigned int thread_id) ;
    int exec_get_time_tic_value( void ) ;
    long long exec_get_time_tics( void ) ;
    int exec_get_trap_sigbus(void) ;
    int exec_get_trap_sigfpe(void) ;
    int exec_get_trap_sigsegv(void) ;
    int exec_get_trap_sigabrt(void) ;
    int exec_get_trap_sigchld(void) ;

    int exec_set_attach_debugger(int on_off) ;
    int exec_set_debugger_command(const char * command) ;
    int exec_set_exec_command(SIM_COMMAND in_command) ;
    int exec_set_freeze_command(int on_off) ;
    int exec_set_freeze_on_frame_boundary(int on_off) ;
    int exec_set_freeze_frame(double) ;
    int exec_set_enable_freeze( int on_off ) ;
    int exec_set_job_cycle(const char * job_name, int instance_num, double in_cycle) ;
    int exec_set_job_onoff(const char * job_name , int instance_num, int on) ;
    int exec_set_rt_nap(int on_off) ;
    int exec_set_sim_object_onoff(const char * sim_object_name , int on) ;
    int exec_set_software_frame(double) ;
    int exec_set_stack_trace(int on_off) ;
    int exec_set_terminate_time(double time_value)  ;
    int exec_set_thread_enabled( unsigned int thread_id , int yes_no ) ;
    int exec_set_thread_amf_cycle_time( unsigned int thread_id , double cycle_time ) ;
    int exec_set_thread_async_cycle_time( unsigned int thread_id , double cycle_time ) ;
    int exec_set_thread_async_wait( unsigned int thread_id , int yes_no ) ;
    int exec_set_thread_rt_semaphores( unsigned int thread_id , int yes_no ) ;
    int exec_set_thread_cpu_affinity(unsigned int thread_id , int cpu_num) ;
    int exec_set_thread_priority(unsigned int thread_id , unsigned int req_priority) ;
    int exec_set_thread_process_type( unsigned int thread_id , int process_type ) ;
    int exec_set_time( double in_time ) ;
    int exec_set_time_tics( long long in_time_tics ) ;
    int exec_set_time_tic_value( int in_time_tics ) ;
    int exec_set_trap_sigbus(int on_off) ;
    int exec_set_trap_sigfpe(int on_off) ;
    int exec_set_trap_sigsegv(int on_off) ;
    int exec_set_trap_sigabrt(int on_off) ;
    int exec_set_trap_sigchld(int on_off) ;
    int exec_set_version_date_tag(const char * tag) ;
    int exec_set_build_date(const char * date) ;
    int exec_set_current_version(const char * version) ;

    int exec_freeze(void) ;
    int exec_run(void) ;
    int exec_step(void) ;

    void exec_signal_terminate(void) ;
    int exec_terminate(const char *, const char *) ;
    int exec_terminate_with_return(int, const char *, int , const char *) ;

    int exec_instrument_before(const char * job_name) ;
    int exec_instrument_after(const char * job_name) ;
    int exec_instrument_remove(const char * job_name) ;

    int exec_add_scheduled_job_class(const char * in_name) ;
    int exec_add_depends_on_job( const char * target_job_string , unsigned int t_instance , const char * depend_job_string , unsigned int d_instance ) ;

    int debug_pause_on(void) ;
    int debug_pause_off(void) ;
    int debug_signal(void) ;

#ifdef __cplusplus
}
#endif

#endif
