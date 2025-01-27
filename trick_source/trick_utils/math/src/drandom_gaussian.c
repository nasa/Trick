/*
   PURPOSE: (Generate a random number consistent with a Gaussian distribution.)

   ASSUMPTIONS AND LIMITATIONS: ((State spaces for independent processes requiring random numbers are not tracked.)
   (Range is specified in standard deviations; i.e. if a range of 3 sigma is specified, then NO numbers greater than
   the plus/minus 3 sigma value will be returned.) (Automatic Seed))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (4/15/91) (Trick-CR-00009) (SAFR Model))) */

#include <stdlib.h>

#include "trick/trick_math.h"


#define MAX_VALUE 32768.0              /* 32768 = 2**15 */

double drandom_gaussian(        /* Return: Random number consistent with Gaussian distribution */
                           double std_deviation,        /* In: One standard deviation */
                           int range)
{                                      /* In: Plus/Minus range for Gaussian output in standard deviations */

    int i;
    double out;
    double number = 0.0;

    for (i = 0; i < 2 * range; i++) {
        /* Call rand() 2*range times to get plus/minus range sigma output */

        /* Normalize output of rand to the range of [-0.5,0.5] */
#ifdef __linux__
        out = (((double) rand()) / RAND_MAX) - 0.5;
#elif __APPLE__
        out = (((double) rand()) / LONG_MAX) - 0.5;
#else
        out = (((double) rand()) / MAX_VALUE) - 0.5;
#endif

        /* Scale normalized output by one sigma value and sum to previous normalized values */
        number += (std_deviation * out);
    }

    /* Resulting summed value is a gaussian random number */
    return (number);
}
