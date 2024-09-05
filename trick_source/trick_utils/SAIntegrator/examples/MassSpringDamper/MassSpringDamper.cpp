#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

struct	MassSpringDamper	{
				double	pos;
				double	vel;
				double	k;					//	Spring	constant
				double	c;					//	Damping	constant
				double	mass;
				double	extForce;
				MassSpringDamper(double	k,	double	c,	double	m);
};
MassSpringDamper::MassSpringDamper(	double	k,	double	c,	double	m)
:	pos(0.0),	vel(0.0),	k(k),	c(c),	mass(m),	extForce(0.0)	{}

struct	ImpulseGenerator	{
				double	period;
				double	duration;
				double	force;
				ImpulseGenerator(double	period,	double	duration,	double	force);
				double	impulse(double	time);
};
ImpulseGenerator::ImpulseGenerator(double	period,	double	duration,	double	force)
:	period(period),	duration(duration),	force(force)	{}
double	ImpulseGenerator::impulse(double	time)	{
				double	tt	=	fmod(time,	period);
				if	((tt	>=	0.0)	&&	(tt	<	duration))	return	force;
				return	0.0;
}
void	print_header()	{
				printf	("time,	msd.pos,	msd.vel\n");
}
void	print_state(	double	t,	MassSpringDamper&	msd	)	{
				printf	("%10.10f,	%10.10f,	%10.10f\n",	t,	msd.pos,	msd.vel);
}
void	acceleration(	double	t,	double	x[],	double	v[],	double	g_out[],	void*	udata)	{
				MassSpringDamper*	msd	=	(MassSpringDamper*)udata;
				g_out[0]	=	-(msd->k/msd->mass)	*	x[0]
															-(msd->c/msd->mass)	*	v[0]
															+	msd->extForce/msd->mass;
}
#define	N_DIMENSIONS	1
int	main	(	int	argc,	char*	argv[])	{
				const	double	mass	=	1.0;												//	kg
				const	double	frequency	=	2;									//	Hz
				const	double	dampingConstant	=	5.0;	//	kg/s	=	Ns/m
				double	springConstant	=	4.0	*	M_PI	*	M_PI	*	frequency	*	frequency	*	mass;	//	kg/s^2	=	N/m
				MassSpringDamper	msd(springConstant,	dampingConstant,	mass);
				ImpulseGenerator	ImpGen(5.0,	0.005,	500.0);	//	500	Newtons	for	0.005	seconds	every	5	seconds.
				double	time	=	0.0;
				double*	x_p[N_DIMENSIONS]	=	{	&msd.pos	};
				double*	v_p[N_DIMENSIONS]	=	{	&msd.vel	};
				double	dt	=	0.001;
				print_header();
				print_state(time,	msd);
				SA::EulerCromerIntegrator	integ(dt,	N_DIMENSIONS,	x_p,	v_p,	acceleration,	&msd);
				while	(time	<	10)	{
								msd.extForce	=	ImpGen.impulse(time);
								integ.integrate();
								time	=	integ.getIndyVar();
								print_state(time,	msd);
				}
}
