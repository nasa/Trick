
#include "trick/ThreadTrigger.hh"
#include "trick/message_proto.h"

/* ThreadTriggerBase empty routines */
Trick::ThreadTriggerBase::ThreadTriggerBase(ThreadTriggerType in_tt_type) : tt_type(in_tt_type) {}
Trick::ThreadTriggerBase::~ThreadTriggerBase() {}
void Trick::ThreadTriggerBase::init() {}

/* ThreadTriggerMutex */
Trick::ThreadTriggerMutex::ThreadTriggerMutex() : ThreadTriggerBase(TT_MUTEX) {
    pthread_cond_init(&go_cv, NULL);
    pthread_mutex_init(&go_mutex, NULL);
}

void Trick::ThreadTriggerMutex::init() {
    pthread_mutex_lock(&go_mutex);
}

void Trick::ThreadTriggerMutex::fire() {
    pthread_mutex_lock(&go_mutex);
    pthread_cond_signal(&go_cv);
    pthread_mutex_unlock(&go_mutex);
}

void Trick::ThreadTriggerMutex::wait() {
    pthread_cond_wait(&go_cv, &go_mutex);
}

void Trick::ThreadTriggerMutex::dump(std::ostream & oss) {
    oss << "    trigger type = mutex" << std::endl ;
}

/* ThreadTriggerFlag */
Trick::ThreadTriggerFlag::ThreadTriggerFlag() : ThreadTriggerBase(TT_FLAG) , frame_trigger(false) {}

void Trick::ThreadTriggerFlag::fire() {
    frame_trigger = true ;
}

void Trick::ThreadTriggerFlag::wait() {
    while (frame_trigger == false) ;
    frame_trigger = false ;
}

void Trick::ThreadTriggerFlag::dump(std::ostream & oss) {
    oss << "    trigger type = flag" << std::endl ;
}

#if __linux__
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>

/* ThreadTriggerEventFD */
Trick::ThreadTriggerEventFD::ThreadTriggerEventFD() : ThreadTriggerBase(TT_EVENTFD) {
    efd = eventfd(0,0);
}

void Trick::ThreadTriggerEventFD::fire() {
    uint64_t value = 1;
    write(efd, &value, sizeof(uint64_t));
}

void Trick::ThreadTriggerEventFD::wait() {
    uint64_t value ;
    read(efd, &value, sizeof(uint64_t));
}

void Trick::ThreadTriggerEventFD::dump(std::ostream & oss) {
    oss << "    trigger type = eventfd" << std::endl ;
}

#include <linux/futex.h>
#include <syscall.h>

/* ThreadTriggerFutex */
Trick::ThreadTriggerFutex::ThreadTriggerFutex() : ThreadTriggerBase(TT_FUTEX), futex_addr(0) {}

void Trick::ThreadTriggerFutex::fire() {
    syscall(SYS_futex, &futex_addr, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void Trick::ThreadTriggerFutex::wait() {
    futex_addr = 0;
    syscall(SYS_futex, &futex_addr, FUTEX_WAIT, 0, NULL, NULL, 0);
}

void Trick::ThreadTriggerFutex::dump(std::ostream & oss) {
    oss << "    trigger type = futex" << std::endl ;
}
#else

/* Empty implementations for OSes other than Linux */

Trick::ThreadTriggerEventFD::ThreadTriggerEventFD() : ThreadTriggerBase(TT_EVENTFD) , efd(0) {}
void Trick::ThreadTriggerEventFD::fire() {}
void Trick::ThreadTriggerEventFD::wait() {}
void Trick::ThreadTriggerEventFD::dump(std::ostream & oss) {
    oss << "    trigger type = eventfd (non-functional). How did you get here?" << std::endl ;
}

Trick::ThreadTriggerFutex::ThreadTriggerFutex() : ThreadTriggerBase(TT_FUTEX) , futex_addr(0) {}
void Trick::ThreadTriggerFutex::fire() {}
void Trick::ThreadTriggerFutex::wait() {}
void Trick::ThreadTriggerFutex::dump(std::ostream & oss) {
    oss << "    trigger type = futex (non-functional). How did you get here?" << std::endl ;
}

#endif

/* ThreadTriggerContainer */
Trick::ThreadTriggerContainer::ThreadTriggerContainer() : ttBase(&ttMutex) {}

Trick::ThreadTriggerBase * Trick::ThreadTriggerContainer::getThreadTrigger() {
    return ttBase ;
}

/* Routine to change trigger type */
void Trick::ThreadTriggerContainer::setThreadTrigger( ThreadTriggerType in_trigger_type ) {
    trigger_type = in_trigger_type ;
    switch ( trigger_type ) {
        case TT_MUTEX :
        default:
            ttBase = &ttMutex ;
            break ;
        case TT_FLAG :
            ttBase = &ttFlag ;
            break ;
        case TT_EVENTFD :
#if __linux__
            ttBase = &ttEventFD ;
#else
            message_publish(MSG_ERROR, "EventFD thread trigger type not available, using mutex\n") ;
            ttBase = &ttMutex ;
#endif
            break ;
        case TT_FUTEX :
#if __linux__
            ttBase = &ttFutex ;
#else
            message_publish(MSG_ERROR, "Futex thread trigger type not available, using mutex\n") ;
            ttBase = &ttMutex ;
#endif
            break ;
    }
}

