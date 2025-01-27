/*
   PURPOSE: (Floating point exception handler)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime)))
 */

#include <string.h>
#include <stdio.h>
#include <signal.h>

/*
 * FPE specific headers
 */
#ifdef __linux__
#include <fenv.h>
#endif

#include "trick/Executive.hh"
#include "trick/exec_proto.hh"

/**
 * @relates Trick::Executive
 * C binded function to handle the SIGFPE signal.  The handler prints which type of
 * floating point error has occured and calls exec_terminate()
 * @return void
 */
#if (__APPLE__ | __CYGWIN__ | __INTERIX )
void fpe_sig_handler(int sig __attribute__ ((unused)) )
#else
void fpe_sig_handler(int sig __attribute__ ((unused)), siginfo_t * sip __attribute__ ((unused)), void *uap __attribute__ ((unused)))
#endif
{

    Trick::Executive * E = exec_get_exec_cpp();

#if __APPLE__
    siginfo_t * sip = NULL ;
#endif

    E->fpe_handler(sip) ;

}
