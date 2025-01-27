
#include <signal.h>
#include <errno.h>

/* Headers for floating point exceptions */
#ifdef __linux__
#include <fenv.h>
#endif

#include "trick/Executive.hh"

void sig_hand(int sig) ;
void ctrl_c_hand(int sig) ;
void term_hand(int sig) ;
void usr1_hand(int sig) ;
void child_handler(int sig) ;

#if (__APPLE__ | __CYGWIN__ | __INTERIX )
void fpe_sig_handler(int sig) ;
#else
void fpe_sig_handler(int sig, siginfo_t * sip, void *uap) ;
#endif

/**
@details
-# If incoming on_off flag is true assign sig_hand() as the signal handler for SIGINT.
-# Else revert to the default signal handler SIG_DFL.
-# set trap_sigbus to the current on_off status
   Requirement [@ref r_exec_signal_0].
*/
int Trick::Executive::set_trap_sigbus(bool on_off) {
    static struct sigaction sigact;

    if ( on_off ) {
        /* Assign sig_hand() as the signal handler for SIGBUS. */
        sigact.sa_handler = (void (*)(int)) sig_hand;
    } else {
        sigact.sa_handler = SIG_DFL;
    }

    if (sigaction(SIGBUS, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGBUS");
    } else {
        trap_sigbus = on_off ;
    }

    return(0) ;
}

/**
@details
-# If incoming on_off flag is true assign fpe_sig_handler() as the signal handler for SIGFPE.
-# Else revert to the default signal handler SIG_DFL.
-# set trap_sigfpe to the current on_off status
   Requirement [@ref r_exec_signal_0].
   Requirement [@ref r_exec_signal_0].
*/
int Trick::Executive::set_trap_sigfpe(bool on_off) {
    static struct sigaction sigact;

    if ( on_off ) {
        /* Assign fpe_sig_handler() as the signal handler for SIGFPE. */
#ifdef __linux__
        feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
#endif
#if (__APPLE__ | __CYGWIN__ | __INTERIX )
        sigact.sa_handler = (void (*)(int)) fpe_sig_handler;
#else
        sigact.sa_flags = SA_SIGINFO;
        sigact.sa_sigaction = (void (*)(int, siginfo_t *, void *)) fpe_sig_handler;
#endif
    } else {
#ifdef __linux__
        fedisableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
#endif
        sigact.sa_handler = SIG_DFL;
    }

    if (sigaction(SIGFPE, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGFPE");
    } else {
        trap_sigfpe = on_off ;
    }

    return(0) ;
}

/**
@details
-# If incoming on_off flag is true assign sig_hand() as the signal handler for SIGSEGV
-# Else revert to the default signal handler SIG_DFL.
-# set trap_sigsegv to the current on_off status
   Requirement [@ref r_exec_signal_0].
*/
int Trick::Executive::set_trap_sigsegv(bool on_off) {
    static struct sigaction sigact;

    if ( on_off ) {
        /* Assign sig_hand() as the signal handler for SIGSEGV. */
        sigact.sa_handler = (void (*)(int)) sig_hand;
    } else {
        sigact.sa_handler = SIG_DFL;
    }

    if (sigaction(SIGSEGV, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGSEGV");
    } else {
        trap_sigsegv = on_off ;
    }
    return(0) ;
}

/**
@details
-# If incoming on_off flag is true assign sig_hand() as the signal handler for SIGSEGV
-# Else revert to the default signal handler SIG_DFL.
-# set trap_sigabrt to the current on_off status
   Requirement [@ref r_exec_signal_0].
*/
int Trick::Executive::set_trap_sigabrt(bool on_off) {
    static struct sigaction sigact;

    if ( on_off ) {
        /* Assign sig_hand() as the signal handler for SIGABRT */
        sigact.sa_handler = (void (*)(int)) sig_hand;
    } else {
        sigact.sa_handler = SIG_DFL;
    }

    if (sigaction(SIGABRT, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGSEGV");
    } else {
        trap_sigabrt = on_off ;
    }
    return(0) ;
}

/**
@details
-# If incoming on_off flag is true assign sig_hand() as the signal handler for SIGCHLD
-# Else revert to the default signal handler SIG_DFL.
-# set trap_sigabrt to the current on_off status
   Requirement [@ref r_exec_signal_0].
*/

int Trick::Executive::set_trap_sigchld(bool on_off) {
    static struct sigaction sigact;

    if ( on_off ) {
        /* Assign sig_hand() as the signal handler for SIGABRT */
        sigact.sa_handler = (void (*)(int)) child_handler;
    } else {
        sigact.sa_handler = SIG_DFL;
    }

    if (sigaction(SIGCHLD, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGCHLD");
    } else {
        trap_sigchld = on_off ;
    }
    return(0) ;
}

/**
@details
-# Catch SIGBUS, SIGSEGV, and SIGABRT errors. Don't catch SIGFPE
-# Assign ctrl_c_hand() as the signal handler for SIGINT.
   Requirement [@ref r_exec_signal_0].
-# Assign sig_hand() as the signal handler for SIGTERM.
   Requirement [@ref r_exec_signal_0].
-# Assign child_handler() as the signal handler for SIGCHLD.
   Requirement [@ref r_exec_signal_1].
*/
int Trick::Executive::init_signal_handlers() {

    static struct sigaction sigact;

    /* By default catch SIGBUS, SIGSEGV, SIGABRT.  Don't catch SIGFPE, SIGCHLD */
    set_trap_sigbus(true) ;
    set_trap_sigfpe(false) ;
    set_trap_sigsegv(true) ;
    set_trap_sigabrt(true) ;
    set_trap_sigchld(false) ;

    /* Assign ctrl_c_hand() as the default signal handler for SIGINT (<CTRL-C> keypress). */
    sigact.sa_handler = (void (*)(int)) ctrl_c_hand;
    if (sigaction(SIGINT, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGINT");
    }

    /* Assign sig_hand() as the default signal handler for SIGTERM (default kill signal). */
    sigact.sa_handler = (void (*)(int)) term_hand;
    if (sigaction(SIGTERM, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGTERM");
    }

    /* Assign sig_hand() as the default signal handler for SIGTERM (default kill signal). */
    sigact.sa_handler = (void (*)(int)) usr1_hand;
    if (sigaction(SIGUSR1, &sigact, NULL) < 0) {
        perror("sigaction() failed for SIGUSR1");
    }

    return(0) ;
}

