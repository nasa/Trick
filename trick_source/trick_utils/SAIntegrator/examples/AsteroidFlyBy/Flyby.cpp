#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

#define	GRAVITATIONAL_CONSTANT	6.674e-11
#define	EARTH_MASS	5.9723e24
#define	EARTH_RADIUS	6367500.0

struct	Flyby	{
				double	pos[2];
				double	vel[2];
				double	planet_mass;
				Flyby(double	px,	double	py,	double	vx,	double	vy,	double	m);
};
Flyby::Flyby(double	px,	double	py,	double	vx,	double	vy,	double	m)	{
				pos[0]	=	px;
				pos[1]	=	py;
				vel[0]	=	vx;
				vel[1]	=	vy;
				planet_mass	=	m;
}
void	print_header()	{
				printf	("time,	dt,	flyby.pos[0],	flyby.pos[1],	flyby.vel[0],	flyby.vel[1]\n");
}
void	print_state(	double	t,	double	dt,	Flyby&	flyby	)	{
				printf	("%10.10f,	%10.10f,	%10.10f,	%10.10f,	%10.10f,	%10.10f\n",
													t,	dt,	flyby.pos[0],	flyby.pos[1],	flyby.vel[0],	flyby.vel[1]);
}
void	gravity(	double	t,	double*	state,	double	derivs[],	void*	udata)	{
				Flyby*	flyby	=	(Flyby*)udata;
				double	d	=	sqrt(	state[0]*state[0]	+	state[1]*state[1]);
				derivs[0]	=	state[2];
				derivs[1]	=	state[3];
				derivs[2]	=	-state[0]	*	GRAVITATIONAL_CONSTANT	*	flyby->planet_mass	/	(d*d*d);
				derivs[3]	=	-state[1]	*	GRAVITATIONAL_CONSTANT	*	flyby->planet_mass	/	(d*d*d);
}
int	main	(	int	argc,	char*	argv[])	{

				double	sim_duration	=	25000.0;	//	s
				double	dt	=	60.0;	//	s
				double	epsilon	=	0.000000001;
				Flyby	flyby(-20.0	*	EARTH_RADIUS,	2.0	*	EARTH_RADIUS,	10000.0	,	0.0,	EARTH_MASS);
				double*	state_p[4]	=	{	&flyby.pos[0],	&flyby.pos[1],	&flyby.vel[0],	&flyby.vel[1]	};

				double	time	=	0.0;	//	s
				print_header();
				print_state(time,	dt,	flyby);

				SA::RKF45Integrator	integ(epsilon,	dt,	4,	state_p,	gravity,	&flyby);
				while	(time	<	sim_duration)	{
								integ.integrate();
								double	last_h	=	integ.getLastStepSize();
								time	=	integ.getIndyVar();

								double	r	=	sqrt(	flyby.pos[0]*flyby.pos[0]	+	flyby.pos[1]*flyby.pos[1]);
								if	(r	<	EARTH_RADIUS)	{	printf("Collision\n");	}
								print_state(time,	last_h,	flyby);
				}
}
