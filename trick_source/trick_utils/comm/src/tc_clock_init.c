
/*
 *  Get system clock reference time
 */

#if __linux__
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "trick/tc_proto.h"

double tc_clock_init(void)
{
    double ref_time;

#if __WIN32__
    ref_time = GetTickCount();
#elif __linux__
    struct timespec tp;

    /* Get absolute time from system */
    clock_gettime(CLOCK_REALTIME, &tp);

    /* Convert nanosecond portion to seconds and add to seconds portion */
    ref_time = ((double)(tp.tv_sec)) + (((double)(tp.tv_nsec)) / 1000000000.0);
#else
    struct timeval tp;

    /* Get absolute time from system */
    gettimeofday(&tp, (struct timezone *) NULL);

    /* Convert microsecond portion to seconds and add to seconds portion */
    ref_time = (double) (tp.tv_sec) + ((double) (tp.tv_usec) / 1000000.0);
#endif

    return (ref_time);

}
