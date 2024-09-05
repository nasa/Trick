/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	Aircraft.	)
LIBRARY	DEPENDENCY:
				((Aircraft.o))
*******************************************************************************/
#include	"Aircraft/include/Aircraft.hh"

#include	"trick/integrator_c_intf.h"
#include	<stdlib.h>
#include	<math.h>
#include	<iostream>
#include	"Aircraft.hh"

int	Aircraft::default_data()	{
				pos[0]	=	0.0;									//	m
				pos[1]	=	0.0;
				vel[0]	=	100.0;							//	m/s
				vel[1]	=	0.0;
				
				mass	=	5000;														//	kg
				thrust_mag_max	=	45000;			//	N
				K_drag	=	0.72;
				heading_rate_gain	=	0.1;
				set_desired_compass_heading(45.0);
				desired_speed	=	200;	//	m/s
				autoPilot	=	false;

				flightPath	=	WaypointList();
				cWP	=	0;
				wpIdx	=	-1;

				return	(0);
}

int	Aircraft::state_init()	{
				heading	=	northWestToPsi(vel);
				return	(0);
}

int	Aircraft::state_deriv()	{
				double	F_total_body[2];
				calc_total_force(F_total_body);
				double	F_total_world[2];
				rotateBodyToWorld(F_total_world,	F_total_body);
				/*	Equation	#1	*/
				acc[0]	=	F_total_world[0]	/	mass;
				acc[1]	=	F_total_world[1]	/	mass;
				return(0);
}

int	Aircraft::state_integ()	{
			int	integration_step;
			load_state	(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);
			load_deriv	(	&vel[0],	&vel[1],	&acc[0],	&acc[1],	(double*)0);
			integration_step	=	integrate();
			unload_state(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);
			//	Calculate	heading	from	velocity.
			heading	=	northWestToPsi(vel);
			return(integration_step);
}

//	This	calculates	the	total	force	on	the	aircraft	in	body	coordinates.
void	Aircraft::calc_total_force(	double	(&F_total_body)[2]	)	{
				double	F_thrust_body[2];
				double	F_drag_body[2];
				double	F_turning_body[2];
				calc_thrust_force(F_thrust_body);
				calc_drag_force(F_drag_body);
				calc_turning_force(F_turning_body);
				//	Eq	#2
				F_total_body[0]	=	F_thrust_body[0]	+	F_drag_body[0]	+	F_turning_body[0];
				F_total_body[1]	=	F_thrust_body[1]	+	F_drag_body[1]	+	F_turning_body[1];
}

void	Aircraft::calc_thrust_force(double	(&F_thrust_body)[2])	{
				//	Eq	#5
				double	thrust_mag_desired	=	K_drag	*	desired_speed	*	desired_speed;
				//	Eq	#6
				double	thrust_mag;
				if	(thrust_mag_desired	>	thrust_mag_max)	{
						thrust_mag	=	thrust_mag_max;
				}	else	if	(	thrust_mag_desired	<	0.0)	{
						thrust_mag	=	0.0;
				}	else	{
						thrust_mag	=	thrust_mag_desired;
				}
				//	Eq	#7
				F_thrust_body[0]	=	thrust_mag;
				F_thrust_body[1]	=	0.0;
}

void	Aircraft::calc_drag_force(double	(	&F_drag_body)[2]	)	{
				//	Eq	#4
				double	speed	=	vector_magnitude(vel);
				//	Eq	#3
				F_drag_body[0]	=	-	K_drag	*	speed	*	speed;
				F_drag_body[1]	=	0.0;
}

void	Aircraft::calc_turning_force(double	(	&F_turning_body)[2]	)	{
				double	heading_diff;
				double	heading_error	=	0.0;
				//	Eq#8
				heading_diff	=	desired_heading	-	heading;
				//	Eq#9
				if	(	heading_diff	>	M_PI	)	{
						heading_error	=	heading_diff	-	2	*	M_PI;
				}	else	if	(heading_diff	<	-M_PI)	{
						heading_error	=	heading_diff	+	2	*	M_PI;
				}	else	{
						heading_error	=	heading_diff;
				}
				//	Eq#10
				double	heading_rate_desired	=	heading_rate_gain	*	heading_error;
				double	speed	=	vector_magnitude(vel);
				//	Eq#11
				double	Fmag	=	heading_rate_desired	*	speed	*	mass;
				if	(Fmag	>		49000.0)	Fmag	=		49000.0;
				if	(Fmag	<	-49000.0)	Fmag	=	-49000.0;
				//	Eq#3
				F_turning_body[0]	=	0.0;
				F_turning_body[1]	=	Fmag;
}

void	Aircraft::rotateBodyToWorld(	double	(&F_total_world)[2],	double	(&F_total_body)[2]	)	{
				//	Eq#12
				F_total_world[0]	=		cos(heading)	*	F_total_body[0]	-	sin(heading)	*	F_total_body[1];
				F_total_world[1]	=		sin(heading)	*	F_total_body[0]	+	cos(heading)	*	F_total_body[1];
}

int	Aircraft::control()	{
				if	(autoPilot)	{
								if	(flightPath.size()	>	0)	{
												//	Calculate	the	difference	between	where	we	want	to	be,	and	where	we	are.
												double	posDiff[2];
												double	_wp[2];
												flightPath.getPosition(cWP,	_wp);
												vector_difference(posDiff,	_wp,	pos);
												//	Calculate	bearing	to	waypoint.
												desired_heading	=	northWestToPsi(posDiff);
												//	Calculate	distance	to	waypoint.
												double	distanceToWaypoint	=	vector_magnitude(posDiff);
												//	If	we've	arrived,	that	is	we're	close	enough,	go	to	the	next	waypoint.
												if	(distanceToWaypoint	<	100.0)	{
																cWP	=	(cWP	+	1)	%	flightPath.size();
												}
								}	
				}
				return	0;
}

int	Aircraft::cycleWaypoints()	{
				if(flightPath.size()	>	0)	{
								wpIdx	=	(wpIdx	+	1)	%	flightPath.size();
								flightPath.getPosition(wpIdx,	wpPos);
								wpImg	=	flightPath.getImage(wpIdx);
				}	else	{
								wpIdx	=	-1;
				}
				return	0;
}

void	Aircraft::set_desired_compass_heading(	double	compass_heading)	{
				desired_heading	=	compassToPsi(	compass_heading	);
}

double	compassToPsi(	double	compass_heading	)	{
				compass_heading	=	fmod(	compass_heading,	360.0	);
				return	(	M_PI	*	(	2.0	-	compass_heading/180.0)	);
}

double	psiToCompass(	double	psi	)	{
				psi	=	fmod(psi	+	2*M_PI,	2*M_PI);
				return	(	180.0	*	(2.0	-	psi/M_PI));
}

double	northWestToPsi	(double	(&p)[2])	{
				double	psi;
				double	mag	=	sqrt(	p[0]*p[0]	+	p[1]*p[1]	);
				if	(mag	>	0.0)	{
								psi	=	atan2(	p[1],	p[0]	);

				}	else	{
								std::cerr	<<	"Error:	Arguments	to	northWestToPsi()	are	zero."
																		<<	std::endl;
								psi	=	0.0;
				}
				return	psi;
}

double	vector_magnitude(double	(&vector)[2])	{
				return	(	sqrt(	vector[0]	*	vector[0]	+	vector[1]	*	vector[1]	));
}

void	vector_difference(double	(&result)[2],	double	(&A)[2],	double	(&B)[2]	)	{
				result[0]	=	A[0]	-	B[0];
				result[1]	=	A[1]	-	B[1];
}

//	double	vector_dot_product(	double	(&A)[2],	double	(&B)[2]	)	{
//					return	(	A[0]	*	B[0]	+	A[1]	*	B[1]	);
//	}

//	double	vector_scale(	double	(&result)[2],	double	(&A)[2],	double	scale	)	{
//					result[0]	=	A[0]	*	scale;
//					result[1]	=	A[1]	*	scale;
//	}
