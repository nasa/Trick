/*
   PURPOSE: (Wave Form Generator)

   ASSUMPTIONS AND LIMITATIONS: ((Ramp) (Pulse) (Square) (Square Doublet) (Triangle) (Triangle Doublet) (Sine) (Uniform
   Random Noise) (Gaussian Random Noise) (User Defined) (All wave forms are computed as a function of time) (Wave
   output values are normalized: -1.0 <= wave output <= 1.0 ))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (5/92) (--) (--))) */

#include <stdio.h>

#ifndef __vxworks
#if __Lynx__
#include <time.h>
#else
#include <sys/time.h>
#endif
#else
#include <sys/times.h>
#endif

#include "trick/wave_form.h"
#include "trick/trick_math.h"

#if ( __sun || __linux__ || __APPLE__ || __CYGWIN__ || __Lynx__ || __QNX__ || __ghs || __INTERIX )
#define gettimeofday(T) gettimeofday(T,NULL)
#endif

double wave_form(               /* Return: Wave form value for input time */
                    WAVE_FORM * W,      /* In: Wave form attributes */
                    double w_time)
{                                      /* In: s Time at which wave form is evaluated */

    double duration;
    double residual;
    double cycles;
    double tmp;
    int i;

    switch (W->type) {
        case Pulse:
            /* One positive fintie pulse */
            if (W->start <= w_time && w_time < W->stop)
                return (1.0);
            break;

        case Ramp:
            /* One positive fintie pulse with start and stop ramps */
            if (W->start <= w_time && w_time < W->stop + W->ramp) {
                if (w_time <= W->start + W->ramp)
                    return ((w_time - W->start) / W->ramp);
                else if (w_time <= W->stop)
                    return (1.0);
                else
                    return ((W->stop + W->ramp - w_time) / W->ramp);
            }
            break;

        case Square:
            /* Oscillating square wave */
            if (W->start <= w_time && w_time < W->stop) {
                duration = w_time - W->start;
                cycles = W->frequency * duration + W->phase / (2.0 * M_PI);
                residual = cycles - (double) ((int) cycles);
                if (residual < 0.5)
                    return (1.0);
                else
                    return (-1.0);
            }
            break;

        case Square_Doublet:
            /* One positive finite pulse followed by one negative finite pulse */
            if (W->start <= w_time && w_time < W->stop)
                return (1.0);
            else if (W->stop + W->pause <= w_time && w_time < (W->stop + W->pause + (W->stop - W->start)))
                return (-1.0);
            break;

        case Triangle:
            /* Oscillating triangular or sawtooth wave */
            if (W->start <= w_time && w_time < W->stop) {
                duration = w_time - W->start;
                cycles = W->frequency * duration + W->phase / (2.0 * M_PI);
                residual = cycles - (double) ((int) cycles);
                if (residual < 0.25)
                    return (residual / 0.25);
                else if (residual < 0.5)
                    return ((0.5 - residual) / 0.25);
                if (residual < 0.75)
                    return (-(residual - 0.5) / 0.25);
                else
                    return (-(1.0 - residual) / 0.25);
            }
            break;

        case Triangle_Doublet:
            /* One positive finite triangular pulse followed by one negative finite triangular pulse */
            duration = (W->stop - W->start) / 2.0;
            if (W->start <= w_time && w_time < W->stop) {
                if ((w_time - W->start) < duration)
                    return ((w_time - W->start) / duration);
                else
                    return ((W->stop - w_time) / duration);
            } else if (W->stop + W->pause <= w_time && w_time < (W->stop + W->pause + (W->stop - W->start))) {
                if ((w_time - (W->stop + W->pause)) < duration)
                    return (-(w_time - W->stop - W->pause) / duration);
                else
                    return (-(W->stop + W->pause + 2.0 * duration - w_time) / duration);
            }
            break;

        case Sine:
            /* Oscillating sine wave */
            if (W->start <= w_time && w_time < W->stop) {
                duration = w_time - W->start;
                return (sin(W->frequency * duration * 2.0 * M_PI + W->phase));
            }
            break;

        case Noise:
            /* Uniform random noise */
            if (W->init_random == 1) {
                struct timeval tv;

                /* Initialize uniform random noise generator */
                W->init_random = 0;
                if (W->random_seed == 0) {
                    gettimeofday(&tv);
                    W->random_seed = (tv.tv_usec & 0x7FFFFFFF);
                }
            }
            if (W->start <= w_time && w_time < W->stop) {
                W->random_seed = (46341 * W->random_seed) % 0x7FFFFFFF;
                tmp = (double) W->random_seed / (double) 0x7FFFFFFF;
                return (2.0 * (tmp - 0.5));
            }
            break;

        case Gauss_Noise:
            /* Gaussian random noise */
            if (W->init_random == 1) {
                struct timeval tv;
                /* Initialize gaussian random noise generator */
                W->init_random = 0;
                if (W->random_seed == 0) {
                    gettimeofday(&tv);
                    W->random_seed = (tv.tv_usec & 0x7FFFFFFF);
                }
            }
            if (W->start <= w_time && w_time < W->stop) {

                /* n sigma range requires 2n evaluations of random() assume standard deviation (sigma) = 1 */
                tmp = 0;
                for (i = 0; i < 2 * W->sigma_range; i++) {
                    W->random_seed = (46341 * W->random_seed) % 0x7FFFFFFF;
                    tmp += ((double) W->random_seed / (double) 0x7FFFFFFF);
                }
                tmp -= (double) W->sigma_range;
                return (tmp);
            }
            break;

        case User_Wave:

            /* User defined wave form data */

            if (W->init_user_data == 1) {
                /* Open user specified file containing any number of records: <time1> <value1> <time2> <value2>
                   <timen> <valuen> */
                W->init_user_data = 0;
                if ((W->fp = fopen(W->user_data, "r")) == NULL) {
                    fprintf(stderr,
                            "trick_utils/math/wave_form.c: "
                            "Error: Could not open user specified " "wave form data file:\n" "->%s<-\n", W->user_data);
                    return (0.0);
                }
                W->user_time = 0.0;
                W->user_val = 0.0;
                fscanf(W->fp, "%lf %lf", &(W->next_time), &(W->next_val));
            }

            if (w_time < W->next_time)
                return (W->user_val);
            else {
                W->user_time = W->next_time;
                W->user_val = W->next_val;
                fscanf(W->fp, "%lf %lf", &(W->next_time), &(W->next_val));
                return (W->user_val);
            }
            break;

        default:
            break;
    }

    return (0.0);
}
