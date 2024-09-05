/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	submarine.	)
LIBRARY	DEPENDENCY:
				((Submarine.o))
*******************************************************************************/
#include	"submarine/include/Submarine.hh"

#include	"trick/integrator_c_intf.h"
#include	<stdlib.h>
#include	<math.h>
#include	<iostream>

int	Submarine::default_data()	{

				//	Parameters
				outer_hull_outer_radius	=	1.30;	//	meters
				outer_hull_inner_radius	=	1.25;	//	meters
				inner_hull_outer_radius	=	1.10;	//	meters
				inner_hull_inner_radius	=	1.00;	//	meters
				hull_length	=	3.0;														//	meters
				ballast_tank_volume	=	1.0;						//	cubic	meter
				hard_ballast_mass=	0.0;									//	kg
				hull_mass	=	10000.0;												//	kg
				payload_mass	=	1500.0;										//	kg
				Cd	=	0.5;

				//	State
				pos[0]	=	0.0;														//	meters
				pos[1]	=	0.0;
				vel[0]	=	0.0;														//	meters/second
				vel[1]	=	0.0;
				ballast_energy	=	80000.0;		//	Joules
				pump_power	=	0.0;										//	Watts

				//	Control	Variable
				ballast_air_ratio	=	0.0;
				water_pressure_pascals	=	0.0;
				water_pressure_PSI	=	0.0;
				pump_power_command	=	0;

				return	(0);
}

int	Submarine::state_init()	{
				return	(0);
}

int	Submarine::state_deriv()	{
				double	total_mass	=	calc_total_mass();
				double	F_gravity		=	calc_gravity_force();
				double	F_buoyancy	=	calc_buoyancy_force();
				double	F_drag					=	calc_drag_force();
				acc[0]	=	0.0;
				acc[1]	=	(F_gravity	+	F_buoyancy	+	F_drag)	/	total_mass;
				return(0);
}

int	Submarine::state_integ()	{
			int	integration_step;
			load_state	(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	&ballast_energy,	(double*)0);
			load_deriv	(	&vel[0],	&vel[1],	&acc[0],	&acc[1],	&pump_power,	(double*)0);
			integration_step	=	integrate();
			unload_state(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	&ballast_energy,	(double*)0);
			if	(ballast_energy	<	0.0)	{
							ballast_energy	=	0.0;
			}
			return(integration_step);
}

//	Calculate	Water	Pressure	at	depth	in	Pascals
void	Submarine::calc_water_pressure()	{
				water_pressure_pascals	=	DENSITY_SALT_WATER	*	GRAVITY	*	-pos[1]	+	14.7/CONVERSION_PASCALS_TO_PSI;
				water_pressure_PSI	=	water_pressure_pascals	*	CONVERSION_PASCALS_TO_PSI;
}
//	Calculate	volume	of	water	displaced	from	the	ballast	water	tank	by	compressed	air.
double	Submarine::calc_ballast_air_volume()	{
				calc_water_pressure();
				double	ballast_air_volume	=	ballast_energy	/	water_pressure_pascals;

				//	Ballast_air_volume	cant'	exceed	to	volume	of	the	ballast	tank.
				if	(	ballast_air_volume	>	ballast_tank_volume	)	{
								ballast_air_volume	=	ballast_tank_volume;
								ballast_energy	=	ballast_air_volume	*	water_pressure_pascals;
				}
				ballast_air_ratio	=	ballast_air_volume	/	ballast_tank_volume;
				return	ballast_air_volume;
}
//	Calculate	the	volume	of	water	currently	displaced	by	the	pressure	hull.
//	This	is	just	multiplying	the	hull	volume	times	a	sigmoid.
double	Submarine::calc_pressure_hull_displaced_volume()	{
				double	C	=	5.5;
				return	(	calc_pressure_hull_volume()	/	(1	+	exp(	C	*	pos[1]	)));
}

double	Submarine::calc_total_displaced_volume()	{
			return	calc_ballast_air_volume()	+	calc_pressure_hull_displaced_volume();
}

double	Submarine::calc_gravity_force	()	{
		return	-calc_total_mass()	*	GRAVITY;
}

double	Submarine::calc_total_mass	()	{
				double	total_mass	=	hard_ballast_mass	+	calc_fixed_mass()	+	calc_ballast_water_mass();
				return	total_mass;
}

double	Submarine::calc_fixed_mass()	{
				double	fixed_mass	=	(hull_mass	+	payload_mass);
				return	fixed_mass;
}

double	Submarine::calc_buoyancy_force(	)	{
			return	(	calc_total_displaced_volume()	*	DENSITY_SALT_WATER	*	GRAVITY	);
}

double	Submarine::calc_ballast_water_mass()	{
				double	ballast_mass	=	calc_ballast_water_volume()	*	DENSITY_SALT_WATER	;
				return	ballast_mass;
}

double	Submarine::calc_ballast_water_volume	()	{
				double	ballast_water_volume	=	ballast_tank_volume	-	calc_ballast_air_volume();
				return	ballast_water_volume;
}

double	Submarine::calc_drag_force()	{
				double	A	=	outer_hull_outer_radius	*	2	*	hull_length;
				return	-0.5	*	DENSITY_SALT_WATER	*	vel[1]	*	fabs(vel[1])	*	Cd	*	A;
}

double	Submarine::calc_pressure_hull_volume()	{
				return	(	M_PI	*	inner_hull_outer_radius	*	inner_hull_outer_radius	*	hull_length);
}

int	Submarine::control()	{
				pump_power	=	pump_power_command;
				if	(pump_power	>		5000.0)	pump_power	=		5000.0;
				if	(pump_power	<	-5000.0)	pump_power	=	-5000.0;
				return(0);
}

int	Submarine::state_post_integ()	{
				return(0);
}
