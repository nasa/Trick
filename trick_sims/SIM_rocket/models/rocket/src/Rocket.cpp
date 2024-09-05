/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	model	rocket.	)
LIBRARY	DEPENDENCY:
				((Rocket.o))
*******************************************************************************/
#include	"../include/Rocket.hh"
#include	<math.h>
#include	<iostream>

int	Rocket::default_data()	{

				env.default_data();
				motor.default_data();
				mission_time	=	0.0;
				mission_time_rate	=	1.0;
				cross_sectional_area	=	0.0015;
				coefficient_of_drag	=	0.75;
				empty_mass	=	0.1162;	//	116.2	g
				total_mass	=	empty_mass;
				position[0]	=	0.0;
				position[1]	=	0.0;
				velocity[0]	=	0.0;
				velocity[1]	=	0.001;
				return	(0);
}

int	Rocket::state_init()	{
				motor.state_init();
				total_mass	=	empty_mass	+	motor.total_mass;
				return	(0);
}

int	Rocket::state_deriv()	{

				mission_time_rate	=	1.0;

				double	thrust_mag	=	motor.thrust_magnitude(	mission_time	);

				mass_rate	=	-	thrust_mag	/	motor.exhaust_speed;

				double	vel_magnitude	=	sqrt(	velocity[0]	*	velocity[0]	+	velocity[1]	*	velocity[1]	);

				if	(	position[1]	<	1.0	)	{
								vel_unit[0]	=	0.0;
								vel_unit[1]	=	1.0;
				}	else	{
								vel_unit[0]	=	velocity[0]	/	vel_magnitude;
								vel_unit[1]	=	velocity[1]	/	vel_magnitude;
				}

				thrust_force[0]	=	vel_unit[0]	*	thrust_mag;
				thrust_force[1]	=	vel_unit[1]	*	thrust_mag;

				drag_force[0]	=	-	0.5	*	coefficient_of_drag	*	env.air_density	*	cross_sectional_area	*	vel_magnitude	*	velocity[0];
				drag_force[1]	=	-	0.5	*	coefficient_of_drag	*	env.air_density	*	cross_sectional_area	*	vel_magnitude	*	velocity[1];

				gravity_force[0]	=	env.gravity[0]	*	total_mass;
				gravity_force[1]	=	env.gravity[1]	*	total_mass;

				total_force[0]	=	thrust_force[0]	+	drag_force[0]	+	gravity_force[0];
				total_force[1]	=	thrust_force[1]	+	drag_force[1]	+	gravity_force[1];

				acc[0]	=	total_force[0]	/	total_mass;
				acc[1]	=	total_force[1]	/	total_mass;

			return(0);
}

#include	"sim_services/Integrator/include/integrator_c_intf.h"

int	Rocket::state_integ()	{

			int	integration_step;

			load_state	(	&mission_time,	&total_mass,	&position[0],	&position[1],	&velocity[0],	&velocity[1],	(double*)0);
			load_deriv	(	&mission_time_rate,	&mass_rate,	&velocity[0],	&velocity[1],	&acc[0],	&acc[1],	(double*)0);

			integration_step	=	integrate();
			unload_state	(	&mission_time,
																		&total_mass,
																		&position[0],	&position[1],
																		&velocity[0],	&velocity[1],
																		(double*)0);

			return(integration_step);
}
