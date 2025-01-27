#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <cstring>

#if __linux__
#include <sys/syscall.h>
#include <sys/types.h>
#include <sched.h>
#endif

#include "trick/ThreadBase.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"

Trick::ThreadBase::ThreadBase(std::string in_name) :
 name(in_name) ,
 pthread_id(0) ,
 pid(0) ,
 rt_priority(0),
 created(false),
 should_shutdown(false),
 cancellable(true)
{
    pthread_mutex_init(&shutdown_mutex, NULL);
#if __linux__
    max_cpu = sysconf( _SC_NPROCESSORS_ONLN ) ;
#ifdef CPU_ALLOC
    cpus = CPU_ALLOC(max_cpu) ;
    CPU_ZERO_S(CPU_ALLOC_SIZE(max_cpu), cpus) ;
#else
    cpus = (cpu_set_t *)calloc(1, sizeof(cpu_set_t)) ;
#endif
#endif
#if __APPLE__
    max_cpu = 0 ;
#endif
}

Trick::ThreadBase::~ThreadBase() {
#if __linux__
#ifdef CPU_FREE
    CPU_FREE(cpus) ;
#endif
#endif
}

std::string Trick::ThreadBase::get_name() {
    return name ;
}

void Trick::ThreadBase::set_name(std::string in_name) {
    name = in_name ;
}

pthread_t Trick::ThreadBase::get_pthread_id() {
    return pthread_id ;
}

pid_t Trick::ThreadBase::get_pid() {
    return pid ;
}

void Trick::ThreadBase::set_pid() {
#if __linux__
    pid = syscall( __NR_gettid ) ;
#else
    pid = getpid() ;
#endif
}

int Trick::ThreadBase::cpu_set(unsigned int cpu __attribute__((unused))) {
    int ret =  0 ;
#if __linux__
    if ( cpu < max_cpu ) {
#ifdef CPU_SET_S
        CPU_SET_S(cpu, CPU_ALLOC_SIZE(max_cpu), cpus) ;
#else
        CPU_SET(cpu, cpus) ;
#endif
    } else {
        message_publish(MSG_WARNING, "CPU value %d is out of range (0 through %d)", cpu, max_cpu - 1) ;
        ret = -1 ;
    }
#endif
#if __APPLE__
    message_publish(MSG_WARNING, "Warning: Trick on Darwin does not yet support processor assignment.\n");
#endif
    return ret ;
}

int Trick::ThreadBase::cpu_clr(unsigned int cpu __attribute__((unused))) {
    int ret =  0 ;
#if __linux__
    if ( cpu < max_cpu ) {
#ifdef CPU_CLR_S
        CPU_CLR_S(cpu, CPU_ALLOC_SIZE(max_cpu), cpus) ;
#else
        CPU_CLR(cpu, cpus) ;
#endif
    } else {
        message_publish(MSG_WARNING, "CPU value %d is out of range (0 through %d)", cpu, max_cpu - 1) ;
        ret = -1 ;
    }
#endif
#if __APPLE__
    message_publish(MSG_WARNING, "Warning: Trick on Darwin does not yet support processor assignment.\n");
#endif
    return ret ;
}

#if __linux__
cpu_set_t * Trick::ThreadBase::get_cpus() {
    return cpus ;
}

void Trick::ThreadBase::copy_cpus(cpu_set_t * in_cpus) {
#ifdef CPU_OR_S
    CPU_ZERO_S(CPU_ALLOC_SIZE(max_cpu), cpus) ;
    CPU_OR_S(CPU_ALLOC_SIZE(max_cpu), cpus, cpus, in_cpus) ;
#else
    *cpus = *in_cpus ;
#endif
}
#endif
#if __APPLE__
void * Trick::ThreadBase::get_cpus() {
    return NULL ;
}

void Trick::ThreadBase::copy_cpus(void * in_cpus __attribute__((unused))) {
}
#endif

int Trick::ThreadBase::execute_cpu_affinity() {
#if __linux__
#ifdef CPU_ALLOC_SIZE
    sched_setaffinity(pid, CPU_ALLOC_SIZE(max_cpu), cpus) ;
#else
    sched_setaffinity(pid, sizeof(cpu_set_t), cpus) ;
#endif
#endif
    return(0) ;
}

int Trick::ThreadBase::set_priority(unsigned int req_priority) {
    rt_priority = req_priority ;
    return 0 ;
}

#if __linux__

#include <sched.h>
#include <errno.h>

int Trick::ThreadBase::execute_priority() {

    int max_priority;
    int min_priority;
    int proc_priority;
    struct sched_param sparams;
    int sched_policy = SCHED_FIFO;

    if ( rt_priority > 0 ) {
        if (sched_getparam((pid_t) 0, &sparams)) {
            message_publish(MSG_ERROR, "Failed to get process scheduling parameters: %s\n", std::strerror(errno));
        } else {

            /* Get maximum and minimum RT priority */
            max_priority = sched_get_priority_max(SCHED_FIFO);
            min_priority = sched_get_priority_min(SCHED_FIFO);

            /* Since Trick's max priority starts at 1 and moves to lower priorties as the number goes up and Linux's
               priorities goes up as the number goes up and maxes out at "max_priority", we need to offset as follows:
             */
            proc_priority = max_priority - (rt_priority - 1);

            /* Make sure priority is in bounds. */
            if (proc_priority < min_priority) {

                message_publish(MSG_WARNING, "Warning: Linux process %d priority at %d is too low.  Minimum Trick \npriority is %d.\n",
                         pid, rt_priority, (max_priority - min_priority) + 2);

                proc_priority = min_priority;
            }

            if (pthread_getschedparam(pthread_self(), &sched_policy, &sparams)) {

                message_publish(MSG_ERROR, "Failed to get process scheduling parameters: %s\n", std::strerror(errno));
            }

            /* Set the process priority. */
            sparams.sched_priority = proc_priority;
            if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sparams)) {
                message_publish(MSG_ERROR, "Failed to set thread priority: %s\n", std::strerror(errno));
            }
        }
    }

    return(0) ;

}
#endif

#if __APPLE__

#include <sched.h>
#include <errno.h>

int Trick::ThreadBase::execute_priority() {

    int ret;

    /* Declare scheduling paramters. */
    int max_priority;
    int min_priority;
    int prev_priority;
    int proc_priority;
    int sched_policy = SCHED_FIFO;
    struct sched_param param;

    if ( rt_priority > 0 ) {
        /* Get maximum and minimum RT priority, and current parameters. */
        max_priority = sched_get_priority_max(sched_policy);
        min_priority = sched_get_priority_min(sched_policy);
        pthread_getschedparam(pthread_self(), &sched_policy, &param);
        prev_priority = param.sched_priority;

        /* Trick's max priority starts at 1 and moves to lower priorities as the number goes up.  Darwin's thread
           priorities range between 15 to 47 (observed). The default priority is 31 (observed); higher priorities
           cause more favorable scheduling. */
        proc_priority = max_priority - (rt_priority - 1);

        /* Make sure priority is in bounds. */
        if (proc_priority < min_priority) {

            message_publish(MSG_WARNING, "Warning: Trick CPU priority at %d is too low.\n", rt_priority);
            message_publish(MSG_WARNING, "This corresponds to a Darwin thread priority of %d.\n", proc_priority);
            message_publish(MSG_WARNING, "The Darwin thread priority range is %d:%d (min:max).\n", min_priority, max_priority);
            message_publish(MSG_WARNING, "The corresponding minimum Trick priority is %d.\n", (max_priority - min_priority) + 1);
            message_publish(MSG_WARNING, "Setting Trick priority to minimum!\n");
            proc_priority = min_priority;

        } else if (proc_priority > max_priority) {

            message_publish(MSG_WARNING, "Warning: Trick CPU priority at %d is too high.\n", rt_priority);
            message_publish(MSG_WARNING, "This corresponds to a Darwin thread priority of %d.\n", proc_priority);
            message_publish(MSG_WARNING, "The Darwin thread priority range is %d:%d (min:max).\n", min_priority, max_priority);
            message_publish(MSG_WARNING, "The maximum Trick priority is 1. Setting to maximum!\n");
            proc_priority = max_priority;
        }

        /* Set the process priority. */
        param.sched_priority = proc_priority;

        ret = pthread_setschedparam(pthread_self(), sched_policy, &param);
        if (ret != 0) {

            message_publish(MSG_ERROR, "Failed to set Darwin thread priority to %d: %s\n", param.sched_priority, std::strerror(errno));
            message_publish(MSG_ERROR, "This should correspond to a Trick CPU priority of %d.\n", (max_priority - proc_priority) + 1);
            message_publish(MSG_ERROR, "The current Darwin thread priority is %d.\n", prev_priority);
            message_publish(MSG_ERROR, "The Darwin thread priority range is %d:%d (min:max).\n", min_priority, max_priority);

        } else {

            message_publish(MSG_INFO, "Info: Trick CPU priority set to %d.\n", (max_priority - proc_priority) + 1);
            message_publish(MSG_INFO, "This corresponds to a Darwin thread priority of %d.\n", param.sched_priority);
            message_publish(MSG_INFO, "The previous Darwin thread priority was %d.\n", prev_priority);
            message_publish(MSG_INFO, "The Darwin thread priority range is %d:%d (min:max).\n", min_priority, max_priority);
        }
    }

    return(0) ;

}

#endif

int Trick::ThreadBase::create_thread() {

    if (created) {
        message_publish(MSG_ERROR, "create_thread called on thread %s (%p) which has already been started.\n", name.c_str(), this);
        return 0;
    }

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_create(&pthread_id, &attr, Trick::ThreadBase::thread_helper , (void *)this);
    created = true;

#if __linux__
#ifdef __GNUC__
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
    if ( ! name.empty() ) {
       std::string short_str = name.substr(0,15) ;
       pthread_setname_np(pthread_id, short_str.c_str()) ;
    }
#endif
#endif
#endif
    return(0) ;
}

int Trick::ThreadBase::cancel_thread() {
    pthread_mutex_lock(&shutdown_mutex);
    should_shutdown = true;
    pthread_mutex_unlock(&shutdown_mutex);

    if ( pthread_id != 0 ) {
        if (cancellable)
            pthread_cancel(pthread_id) ;
    }
    return(0) ;
}

int Trick::ThreadBase::join_thread() {
    if ( pthread_id != 0 ) {
        if ((errno = pthread_join(pthread_id, NULL)) != 0) {
            std::string msg = "Thread " + name + " had an error in join";
            perror(msg.c_str());
        } else {
            pthread_id = 0;
        }
    }
    return(0) ;
}

void Trick::ThreadBase::test_shutdown() {
    test_shutdown (NULL, NULL);
}

void Trick::ThreadBase::test_shutdown(void (*exit_handler) (void *), void * exit_arg) {
    pthread_mutex_lock(&shutdown_mutex);
    if (should_shutdown) {
        pthread_mutex_unlock(&shutdown_mutex);

        thread_shutdown(exit_handler, exit_arg);
    }
    pthread_mutex_unlock(&shutdown_mutex);
}


void Trick::ThreadBase::thread_shutdown() {
    thread_shutdown (NULL, NULL);
}

void Trick::ThreadBase::thread_shutdown(void (*exit_handler) (void *), void * exit_arg) {
    if (exit_handler != NULL) {
        exit_handler(exit_arg);
    }

    pthread_exit(0); 
}

void * Trick::ThreadBase::thread_helper( void * context ) {

    sigset_t sigs;
    Trick::ThreadBase * tb = (Trick::ThreadBase *)context ;

    /* block out all signals on this thread */
    sigfillset(&sigs);
    pthread_sigmask(SIG_BLOCK, &sigs, NULL);

    /* Set the cancel type to deffered, the thread will be cancelled at next cancellation point */
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    tb->set_pid() ;

    /* Set thread priority and CPU affinity */
    tb->execute_priority() ;
    tb->execute_cpu_affinity() ;

    return tb->thread_body() ;
}

void Trick::ThreadBase::dump( std::ostream & oss ) {
    oss << "    from Trick::ThreadBase\n";
    oss << "    pthread_id = " << pthread_id << "\n";
    oss << "    process_id = " << pid << "\n";
    oss << "    rt_priority = " << rt_priority << "\n";
#if __linux__
    oss << "    cpus = " ;
    bool first_print = true ;
    for ( unsigned int ii = 0 ; ii < max_cpu ; ii++ ) {
#ifdef CPU_ISSET_S
        if ( CPU_ISSET_S(ii, CPU_ALLOC_SIZE(max_cpu), cpus) ) {
#else
        if ( CPU_ISSET(ii, cpus) ) {
#endif
            if ( first_print == true ) {
                first_print = false ;
            } else {
                oss << "," ;
            }
            oss << ii ;
        }
    }
    if ( first_print ) {
        oss << "none assigned" ;
    }
    oss << std::endl ;
#endif
}


