#include	"../include/Motor.hh"
#include	"../include/interpolate.h"
#include	<iostream>

#define	NUM_ELEMENTS	29
const	double	E9_sample_time_array[NUM_ELEMENTS]	=	{
0.000,	0.010,	0.020,	0.030,	0.040,	0.050,	0.100,	0.110,	0.120,	0.130,
0.140,	0.155,	0.165,	0.180,	0.190,	0.200,	0.215,	0.230,	0.250,	0.275,
0.300,	0.400,	0.500,	2.600,	2.700,	2.725,	2.750,	2.800,	100.0	};

const	double	E9_thrust_array[NUM_ELEMENTS]	=	{
	0.0,		0.1,		0.3,		0.5,		0.8,		1.5,	10.0,	12.6,	15.2,	17.8,
20.4,	23.0,	24.6,	25.0,	24.8,	24.4,	22.0,	17.0,	13.7,	12.6,
11.9,	10.7,	10.6,	10.5,	10.3,	10.0,		9.7,	0.0,	0.0	};

int	Motor::default_data	()	{

				thrust_sample_times	=	E9_sample_time_array;
				thrust_sample_values	=	E9_thrust_array;
				thrust_sample_count	=	NUM_ELEMENTS;

				total_mass	=	0.0567;
				propellant_mass	=	0.0358;
				state_init();
				return	0;
}

int	Motor::state_init	()	{

				total_impulse	=	0.0;
				for	(int	ii=1	;	ii<NUM_ELEMENTS	;	ii++)	{
								total_impulse	+=		(thrust_sample_times[ii]	-	thrust_sample_times[ii-1])	*
																										(thrust_sample_values[ii]	+	thrust_sample_values[ii-1])	/	2.0;
				}
				exhaust_speed	=	total_impulse	/	propellant_mass;
				return	0;
}

double	Motor::thrust_magnitude(double	time)	{
				//	Call	interpolator
				return	interpolate(	time,	thrust_sample_times,	thrust_sample_values,	thrust_sample_count	);
}
