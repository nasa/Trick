/*
PURPOSE:
    (For master/slave sim, this is slave info)
PROGRAMMERS:
    (((Eddie J. Paddock) (LinCom) (April 1996) (--) (Initial Version))
     ((Scott Killingsworth) (LinCom) (September 1997) (--) (--)))
*/

/**
 * @enum SIM_COMMAND
 * The SIM_COMMAND enumeration represents the TRICK simulation commmands.
 */

#ifndef SIMMODE_HH
#define SIMMODE_HH

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

    NoCmd = 0 ,         /* NoCmd */
    FreezeCmd = 2 ,     /* freeze */
    RunCmd = 3 ,        /* run */
    ExitCmd = 10 ,      /* exit */

} SIM_COMMAND;

/**
 * @enum SIM_MODE
 * The SIM_MODE enumeration represents the TRICK simulation modes.
 */

typedef enum {

    Initialization = 0 ,  /* Initialization */
    Freeze = 1 ,          /* freeze */
    Step = 4 ,            /* Debug Stepping */
    Run = 5 ,             /* run */
    ExitMode = 6          /* exit */

} SIM_MODE ;

const char * simModeCharString(SIM_MODE mode);

#ifdef __cplusplus
}
#endif

#endif
