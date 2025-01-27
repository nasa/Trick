/*
   PURPOSE: (Floating point exception handler)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime)))
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

/*
 * FPE specific headers
 */
#ifdef __linux__
#include <fenv.h>
#endif

#include "trick/Executive.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"

/**
@design
-# This signal handler processes SIGFPE
-# Write a message to stderr that a signal has been caught.
-# In Linux print out the type of floating point error that occured.
-# If attach_debugger == true, attempt to attach a debugger to the current process.
-# Else if stack_trace == true, attempt to attach a debugger to print a stack trace.
-# Exit the process with a -2 return.
*/

void Trick::Executive::fpe_handler(siginfo_t * sip __attribute__((unused)) ) {

    write( 2 , "\033[31mProcess terminated by signal FPE" , 36 ) ;
#if __linux__
    /* Determine what floating point error occurred */
    if (sip != (siginfo_t *) NULL) {
        switch (sip->si_code) {
            case FPE_FLTRES:          /* inexact */
                write( 2, " (inexact)", 10 );
                return ;
            case FPE_FLTUND:          /* underflow */
                write( 2, " (floating point underflow)", 27 );
                break;
            case FPE_FLTINV:          /* invalid operand */
                write( 2, " (invalid operand)", 18 );
                break;
            case FPE_FLTDIV:          /* division-by-zero */
                write( 2, " (floating point division-by-zero)", 34);
                break;
            case FPE_FLTOVF:          /* overflow */
                write( 2, " (floating point overflow)", 26);
                break;
            case FPE_INTOVF:          /* integer overflow */
                write( 2, " (integer overflow)", 19);
                break;
            case FPE_INTDIV:          /* integer division-by-zero */
                write( 2, " (integer division-by-zero)", 27);
                break;
            default:
                break;
        }
    }
#endif
    write( 2 , "\033[0m\n" , 5 ) ;

    /*
     Attempt to attach with debugger or print stack trace.  Not a requirement.
     snprintf and system are not async signal safe, but we don't have anything to lose.
     */
#if __linux__
    char command[1024];
    if (attach_debugger == true) {
        snprintf(command, sizeof(command), "%s -silent /proc/%d/exe %d", debugger_command.c_str(), getpid(), getpid());
        system(command);
    } else if (stack_trace == true ) {
            snprintf(command, sizeof(command), "%s -silent -batch -x ${TRICK_HOME}/share/trick/gdb_commands "
                    "/proc/%d/exe %d", debugger_command.c_str(), getpid(), getpid());
            system(command);
    }
#endif

    // The user has chosen for Trick to intervene on their behalf.  Exit if handler called.
    _exit(-2) ;

}
