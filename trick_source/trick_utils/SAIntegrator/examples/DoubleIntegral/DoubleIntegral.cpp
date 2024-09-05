#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

struct	DefiniteIntegrator	{
				double	ivar;
				double	start;
				double	end;
				double	result;
				SA::Integrator*	integrator;
				double	evaluate	();
};
double	DefiniteIntegrator::evaluate	()	{
				result	=	0.0;
				integrator->setIndyVar(start);
				ivar	=	start;
				while	(ivar	<	end)	{
								integrator->integrate();
								ivar	=	integrator->getIndyVar();
				}
				return	result;
}
struct	IntegContext	{
				DefiniteIntegrator	XDI;
				DefiniteIntegrator	YDI;
};
void	deriv_y(	double	y,	double	state_y[],	double	derivs_y[],	void*	udata)	{
				IntegContext&	IC	=	*(IntegContext*)udata;
				derivs_y[0]	=	cos(IC.XDI.ivar)	*	cos(y);
}
void	deriv_x(	double	x,	double	state_x[],	double	derivs_x[],	void*	udata)	{
				IntegContext&	IC	=	*(IntegContext*)udata;
				derivs_x[0]	=	IC.YDI.evaluate();
}
double	doubleIntegral(	double	x_start,	double	x_end,	double	y_start,	double	y_end	)	{
				IntegContext	IC;
				double	dx	=	(x_end	-	x_start)	/	20.0;
				double	dy	=	(y_end	-	y_start)	/	20.0;
				double*	state_x[1]	=	{	&IC.XDI.result	};
				double*	state_y[1]	=	{	&IC.YDI.result	};

				IC.XDI.start	=	x_start;
				IC.XDI.end			=	x_end;
				IC.XDI.integrator	=	new	SA::RK4Integrator(dx,	1,	state_x,	deriv_x,	&IC);

				IC.YDI.start	=	y_start;
				IC.YDI.end			=	y_end;
				IC.YDI.integrator	=	new	SA::RK4Integrator(dy,	1,	state_y,	deriv_y,	&IC);

				return	(IC.XDI.evaluate());
}
int	main	(	int	argc,	char*	argv[])	{
				double	vol	=	doubleIntegral(	-M_PI/2.0,	M_PI/2.0,	-M_PI/2.0,	M_PI/2.0	);
				printf("Volume	=	%g.\n",	vol);
}
