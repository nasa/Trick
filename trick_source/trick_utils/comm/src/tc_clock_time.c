
/*
 * Get elapsed time since last call to tc_clock_init()
 */

#ifdef __linux__
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "trick/tc_proto.h"

double tc_clock_time(double ref_time)
{
    double y_secs;

#if __WIN32__
    y_secs = GetTickCount();
#elif __linux__
    struct timespec tp;

    /* Get absolute time from system */
    clock_gettime(CLOCK_REALTIME, &tp);

    /* Convert nanosecond portion to seconds and add to seconds portion */
    y_secs = (double) (tp.tv_sec) + ((double) (tp.tv_nsec) / 1000000000.0);
#else
    struct timeval tp;

    /* Get absolute time from system */
    gettimeofday(&tp, (struct timezone *) NULL);

    /* Convert microsecond portion to seconds and add to seconds portion */
    y_secs = (double) (tp.tv_sec) + ((double) (tp.tv_usec) / 1000000.0);
#endif

    /* Return the delta between the current time and the reference time */
    return (y_secs - ref_time);

}
