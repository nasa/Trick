/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	hot-air	balloon.	)
LIBRARY	DEPENDENCY:
				((Balloon.o))
*******************************************************************************/
#include	"balloon/include/Balloon.hh"
#include	"atmosphere/include/atmosphere.h"

#include	"trick/integrator_c_intf.h"
#include	<stdlib.h>
#include	<math.h>
#include	<iostream>

int	Balloon::default_data()	{

				pos[0]	=	0.0;
				pos[1]	=	2.0;
				vel[0]	=	0.0;
				vel[1]	=	0.0;

				envelope_mass						=	113.4;
				basket_mass								=		63.5;
				burner_system_mass	=	206.4;
				payload_mass							=	300.0;

				envelope_air_temperature	=	80;
				envelope_radius										=	8.5;
				envelope_theta											=	45	*	(M_PI/180.0);

				Cd[0]	=	0.5;
				Cd[1]	=	0.5;

				return	(0);
}

int	Balloon::state_init()	{
				envelope_volume	=	calc_envelope_volume();
				fixed_mass	=	calc_fixed_mass();
				return	(0);
}

int	Balloon::state_deriv()	{
				double	total_mass	=	calc_total_mass();
				double	F_gravity		=	total_mass	*	(-US_STD_gravity(	pos[1]));	/*	Equation	#2	*/
				double	F_buoyancy	=	calc_buoyancy_force();
				double	F_drag[2];
				calc_drag_force(F_drag);
				acc[0]	=	0.0;

				acc[0]	=	(F_drag[0])	/	total_mass;																										/*	Equation	#1	*/
				acc[1]	=	(F_gravity	+	F_buoyancy	+	F_drag[1])	/	total_mass;	/*	Equation	#1	*/

				return(0);
}

int	Balloon::state_integ()	{
			int	integration_step;
			load_state	(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);
			load_deriv	(	&vel[0],	&vel[1],	&acc[0],	&acc[1],	(double*)0);
			integration_step	=	integrate();
			unload_state(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);
			return(integration_step);
}

double	Balloon::calc_total_mass()	{
				return	(fixed_mass	+	calc_envelope_air_mass());
}

double	Balloon::calc_envelope_air_mass()	{
			double	heated_air_density	=	calc_heated_air_density();
			return	(	heated_air_density	*	envelope_volume	);	/*	Equation	#4	with	density	of	heated	air.	*/
}

double	Balloon::calc_envelope_volume()	{
			double	h;

			h	=	envelope_radius	*	(1.0	+	sin(	envelope_theta));
			double	v_dome	=	volume_of_a_spherical_dome(envelope_radius,	h);

			double	r	=	envelope_radius	*	cos(envelope_theta);
			h	=	r	/	tan(envelope_theta);
			double	v_cone	=	volume_of_a_cone(r,	h);

			return	(v_dome	+	v_cone);	/*	Equation	#7	*/
}

double	Balloon::volume_of_a_spherical_dome(	double	r,	double	h)	{
			return	(M_PI*((r*h*h)-((h*h*h)/3.0)));	/*	Equation	#5	*/
}

double	Balloon::volume_of_a_cone(	double	r,	double	h)	{
			return	((M_PI*r*r*h)/3.0);	/*	Equation	#6	*/
}

double	Balloon::calc_heated_air_density()	{
				double	t_k	=	envelope_air_temperature	+	273.15;
				double	rho	=	US_STD_pressure(pos[1])	/	(287.055	*	t_k);	/*	Equation	#8	*/
				return	rho;
}

double	Balloon::calc_buoyancy_force()	{
				return(	calc_displaced_air_mass()	*	US_STD_gravity(	pos[1]));	/*	Equation	9	*/
}

double	Balloon::calc_displaced_air_mass()	{
				return(	US_STD_density(pos[1])	*	envelope_volume);	/*	Equation	#4	with	standard	air	density.	*/
}

double	Balloon::calc_fixed_mass()	{
				return	(envelope_mass	+	basket_mass	+	burner_system_mass	+	payload_mass);
}

void	Balloon::calc_drag_force(double	*F_drag)	{
				double	altitude	=	pos[1];

				/*	Equation	11	*/
				double	v_tas[2];
				v_tas[0]	=	vel[0]	-	wind_speed;
				v_tas[1]	=	vel[1];

				double	A[2];
				A[0]	=	(0.75	*	M_PI	+	1.0)	*	envelope_radius	*	envelope_radius;	/*	Equation	13	*/
				A[1]	=	M_PI	*	envelope_radius	*	envelope_radius;	/*	Equation	12	*/

				/*	Equation	10	*/
				F_drag[0]	=	-(	0.5	*	US_STD_density(altitude)	*	abs(v_tas[0])	*	v_tas[0]	*	Cd[0]	*	A[0]	);
				F_drag[1]	=	-(	0.5	*	US_STD_density(altitude)	*	abs(v_tas[1])	*	v_tas[1]	*	Cd[1]	*	A[1]	);

				return;
}

int	Balloon::control()	{
				if	(	temperature_change_command	!=	0)	{
								envelope_air_temperature	+=	temperature_change_command;
								temperature_change_command	=	0;
				}
				if	(envelope_air_temperature	>	120.0)	{
								envelope_air_temperature	=	120.0;
				}
				if	(envelope_air_temperature	<	80.0)	{
								envelope_air_temperature	=	80.0;
				}
				if	(	wind_change_command	!=	0)	{
								wind_speed	+=	wind_change_command;
								wind_change_command	=	0;
				}
				if	(wind_speed	>	10.0)	{
								wind_speed	=	10.0;
				}
				if	(wind_speed	<	-10.0)	{
								wind_speed	=	-10.0;
				}
				return(0);
}

int	Balloon::check_ground_contact()	{
				if	(pos[1]	<	2.0)	{
								pos[1]	=	2.0;
								vel[0]	=	0.0;
								vel[1]	=	0.0;
				}
				return(0);
}
