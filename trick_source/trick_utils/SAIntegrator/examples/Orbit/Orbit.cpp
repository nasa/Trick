#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

#define	GRAVITATIONAL_CONSTANT	6.674e-11
#define	EARTH_MASS	5.9723e24
#define	EARTH_RADIUS	6367500.0

struct	Orbit	{
				double	pos[2];
				double	vel[2];
				double	planet_mass;
				Orbit(double	px,	double	py,	double	vx,	double	vy,	double	m);
};
Orbit::Orbit(double	px,	double	py,	double	vx,	double	vy,	double	m)	{
				pos[0]	=	px;
				pos[1]	=	py;
				vel[0]	=	vx;
				vel[1]	=	vy;
				planet_mass	=	m;
}
void	print_header()	{
				printf	("time,	orbit.pos[0],	orbit.pos[1],	orbit.vel[0],	orbit.vel[1]\n");
}
void	print_state(	double	t,	Orbit&	orbit	)	{
				printf	("%10.10f,	%10.10f,	%10.10f,	%10.10f,	%10.10f\n",
													t,	orbit.pos[0],	orbit.pos[1],	orbit.vel[0],	orbit.vel[1]);
}
void	G(	double	t,	double	pos[],	double	vel[],	double	acc_out[],	void*	udata)	{
				Orbit*	orbit	=	(Orbit*)udata;
				double	d	=	sqrt(	pos[0]*pos[0]	+	pos[1]*pos[1]);
				acc_out[0]	=	-pos[0]	*	GRAVITATIONAL_CONSTANT	*	orbit->planet_mass	/	(d*d*d);
				acc_out[1]	=	-pos[1]	*	GRAVITATIONAL_CONSTANT	*	orbit->planet_mass	/	(d*d*d);
}
int	main	(	int	argc,	char*	argv[])	{

				const	double	altitude_AGL	=	408000.0;												//	m

				double	mu	=	GRAVITATIONAL_CONSTANT	*	EARTH_MASS;	//	m^3s^2
				double	R_bar_mag	=	EARTH_RADIUS	+	altitude_AGL;		//	m
				double	V_bar_mag	=	sqrt(mu	/	R_bar_mag);									//	m/s
				double	Orbital_period	=	2	*	M_PI	*	sqrt((R_bar_mag	*	R_bar_mag	*	R_bar_mag)	/	mu);	//	s

				double	dt	=	0.1;	//	s
				Orbit	orbit(0.0,	R_bar_mag,	V_bar_mag,	0.0,	EARTH_MASS);
				double*	x_p[2]	=	{	&orbit.pos[0],	&orbit.pos[1]	};
				double*	v_p[2]	=	{	&orbit.vel[0],	&orbit.vel[1]	};

				double	time	=	0.0;	//	s

				print_header();
				print_state(time,	orbit);
				SA::EulerCromerIntegrator	integ(dt,	2,	x_p,	v_p,	G,	&orbit);
				while	(time	<	Orbital_period)	{
								integ.integrate();
								time	=	integ.getIndyVar();
								print_state(time,	orbit);
				}
}
