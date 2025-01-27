
#include <iostream>
#include <sstream>
#include <stdio.h>
#if __linux__
#include <sys/syscall.h>
#include <sys/types.h>
#include <sched.h>
#endif
#include <signal.h>
#include <algorithm>
#include <time.h>

#include "trick/SysThread.hh"

bool Trick::SysThread::shutdown_finished = false;

// Construct On First Use to avoid the Static Initialization Fiasco
pthread_mutex_t& Trick::SysThread::list_mutex() {
    static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
    return list_mutex;
} 

pthread_cond_t& Trick::SysThread::list_empty_cv() {
    static pthread_cond_t list_empty_cv = PTHREAD_COND_INITIALIZER;
    return list_empty_cv;
} 

std::vector<Trick::SysThread *>& Trick::SysThread::all_sys_threads() {
    static std::vector<SysThread *> all_sys_threads;
    return all_sys_threads;
}

Trick::SysThread::SysThread(std::string in_name) : ThreadBase(in_name) {
    pthread_mutex_init(&_restart_pause_mutex, NULL);
    pthread_cond_init(&_thread_has_paused_cv, NULL);
    pthread_cond_init(&_thread_wakeup_cv, NULL);
    _thread_has_paused = true;
    _thread_should_pause = false;

    pthread_mutex_lock(&(list_mutex()));
    all_sys_threads().push_back(this);
    pthread_mutex_unlock(&(list_mutex()));
}


Trick::SysThread::~SysThread() {
    pthread_mutex_lock(&(list_mutex()));
    if (!shutdown_finished) {
        all_sys_threads().erase(std::remove(all_sys_threads().begin(), all_sys_threads().end(), this), all_sys_threads().end());
    }
    pthread_mutex_unlock(&(list_mutex()));
}

int Trick::SysThread::ensureAllShutdown() {
    pthread_mutex_lock(&(list_mutex()));

    // Cancel all threads
    for (SysThread * thread : all_sys_threads()) {
        thread->cancel_thread();
    }

    // Join all threads
    for (SysThread * thread : all_sys_threads()) {
        thread->join_thread();
    }

    // Success!
    shutdown_finished = true;
    pthread_mutex_unlock(&(list_mutex()));

    return 0;
}

// To be called from main thread
void Trick::SysThread::force_thread_to_pause() {
    pthread_mutex_lock(&_restart_pause_mutex);
    // Tell thread to pause, and wait for it to signal that it has
    _thread_should_pause = true;
    while (!_thread_has_paused) {
        pthread_cond_wait(&_thread_has_paused_cv, &_restart_pause_mutex);
    }
    pthread_mutex_unlock(&_restart_pause_mutex);
}

// To be called from main thread
void Trick::SysThread::unpause_thread() {
    pthread_mutex_lock(&_restart_pause_mutex);
    // Tell thread to wake up
    _thread_should_pause = false;
    pthread_cond_signal(&_thread_wakeup_cv);
    pthread_mutex_unlock(&_restart_pause_mutex);
}


// To be called from this thread
void Trick::SysThread::test_pause() {
    pthread_mutex_lock(&_restart_pause_mutex) ;
    if (_thread_should_pause) {
        // Tell main thread that we're pausing
        _thread_has_paused = true;
        pthread_cond_signal(&_thread_has_paused_cv);
        
        // Wait until we're told to wake up
        while (_thread_should_pause) {
            pthread_cond_wait(&_thread_wakeup_cv, &_restart_pause_mutex);
        }
    }

    _thread_has_paused = false;
    pthread_mutex_unlock(&_restart_pause_mutex) ;
}