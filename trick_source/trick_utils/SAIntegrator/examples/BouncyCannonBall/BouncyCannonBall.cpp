#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

struct	Cannon	{
				double	pos[2];
				double	vel[2];
				Cannon(double	px,	double	py,	double	vx,	double	vy);
};
Cannon::Cannon(double	px,	double	py,	double	vx,	double	vy)	{
				pos[0]	=	px;		pos[1]	=	py;
				vel[0]	=	vx;		vel[1]	=	vy;
}
void	CSV_header()	{
				printf	("t,	cannon.pos[0],	cannon.pos[1],	cannon.vel[0],	cannon.vel[1]\n");
}
void	CSV_state(	double	t,	Cannon&	cannon)	{
				printf	("%5.3f,	%5.10f,	%5.10f,	%5.10f,	%5.10f\n",
				t,	cannon.pos[0],	cannon.pos[1],	cannon.vel[0],	cannon.vel[1]);
}
void	calc_derivs(	double	t,	double	state[],	double	derivs[],	void*	udata)	{
				derivs[0]	=	state[2];
				derivs[1]	=	state[3];
				derivs[2]	=			0.0;
				derivs[3]	=	-9.81;
}
double	impact(	double	t,	double	state[],	RootFinder*	root_finder,	void*	udata)	{
				double	root_error	=	root_finder->find_roots(t,	state[1]);
				if	(root_error	==	0.0)	{
								root_finder->init();
								state[2]	=		0.9	*	state[2];
								state[3]	=	-0.9	*	state[3];
				}
				return	(root_error);
}
int	main	(	int	argc,	char*	argv[])	{
				const	double	muzzle_speed	=	50;	//	m/s
				const	double	muzzle_angle	=	30;	//	degrees
				double	vx0	=	muzzle_speed	*	cos(muzzle_angle	*	M_PI	/	180.0);
				double	vy0	=	muzzle_speed	*	sin(muzzle_angle	*	M_PI	/	180.0);
				Cannon	cannon(0.0,	0.0,	vx0,	vy0);

				double*	state_var_p[4]	=	{	&(cannon.pos[0]),	&(cannon.pos[1]),
																															&(cannon.vel[0]),	&(cannon.vel[1])	};
				double	dt	=	0.01;
				SA::RK2Integrator	integ(dt,	4,	state_var_p,	calc_derivs,	NULL);
				integ.add_Rootfinder(1.0e-10,	Negative,	&impact);

				double	t		=	0.0;
				CSV_header();
				CSV_state(	t,	cannon);
				while	(t	<	20.0)	{
								integ.integrate();
								t	=	integ.getIndyVar();
								CSV_state(	t,	cannon);
				}
}
