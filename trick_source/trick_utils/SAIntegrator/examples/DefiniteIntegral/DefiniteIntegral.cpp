#include	<math.h>
#include	<stdio.h>
#include	"SAIntegrator.hh"

struct	DefiniteIntegral	{
				double	lower_limit;
				double	upper_limit;
				double	result;
				double	Coeff[4];
				SA::Integrator*	integrator;
				double	evaluate	();
};
double	DefiniteIntegral::evaluate	()	{
				result	=	0.0;
				integrator->setIndyVar(lower_limit);
				while(integrator->getIndyVar()	<	upper_limit){
								integrator->integrate();
				}
				return	result;
}
void	Order3Polynomial(	double	x,	double	state[],	double	derivs[],	void*	udata)	{
				DefiniteIntegral&	DI	=	*(DefiniteIntegral*)udata;
				double	x2	=	x*x;
				double	x3	=	x*x2;
				derivs[0]	=	DI.Coeff[3]	*	x3	+
																DI.Coeff[2]	*	x2	+
																DI.Coeff[1]	*	x	+
																DI.Coeff[0];
}
int	main	(int	argc,	char*	argv[])	{
				unsigned	int	NSteps	=	20;
				DefiniteIntegral	DI;
				DI.Coeff[0]	=		7.0;
				DI.Coeff[1]	=	-5.0;
				DI.Coeff[2]	=		1.0;
				DI.Coeff[3]	=		2.0;
				DI.lower_limit	=		0.0;
				DI.upper_limit	=		2.0;
				double	dx	=	(DI.upper_limit	-	DI.lower_limit)	/	NSteps;
				double*	state[1]	=	{	&DI.result	};
				DI.integrator	=	new	SA::RK4Integrator(dx,	1,	state,	Order3Polynomial,	&DI);
				double	result	=	DI.evaluate();
				printf("Integral	=	%g.\n",	result);
}
