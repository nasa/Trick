
#include	<iostream>
#include	<sstream>
#include	<stdarg.h>
#include	"trick/Interpolator.hh"

double	Trick::Interpolator::eval	(double	param[],	unsigned	int	param_index,	unsigned	int	offset)
{

				double	x,	x_lower,	x_upper;
				double	f_x,	f_x_lower,	f_x_upper;
				double	*breakPoint;
				unsigned	int	breakPointArraySize;
				unsigned	int	ii;

				x	=	param[param_index];

				breakPoint	=	breakPointArrays[param_index];
				breakPointArraySize	=	breakPointArraySizes[param_index];

				for	(	ii=0	;	((ii+2	<	breakPointArraySize)	&&	(x	>	breakPoint[ii+1]))	;	ii++	)	;

				x_lower	=	breakPoint[ii];
				x_upper	=	breakPoint[ii+1];

				if	((x	<	x_lower)	||		(x	>	x_upper))	{
								std::stringstream	ss;

								ss	<<	"Interpolation	parameter["	<<	param_index	<<	"]	is	outside	of	its	specified	breakpoint	range."	;
								throw	std::logic_error(	ss.str()	);
				}

				if	(param_index	==	nParams-1)	{
								f_x_lower	=	table[offset	*	breakPointArraySize	+	ii];
								f_x_upper	=	table[offset	*	breakPointArraySize	+	ii	+	1];
				}	else	{
								f_x_lower	=	eval(	param,	param_index+1,	offset	*	breakPointArraySize	+	ii);
								f_x_upper	=	eval(	param,	param_index+1,	offset	*	breakPointArraySize	+	ii	+	1);
				}

				f_x	=	(x_upper	-	x)/(x_upper	-	x_lower)	*	f_x_lower	+
										(x	-	x_lower)/(x_upper	-	x_lower)	*	f_x_upper	;

				return(f_x);

}

double	Trick::Interpolator::eval	(double	params[])
{

				return	(	eval(	params,	0,0));
}

double	Trick::Interpolator::eval	(double	param1,	...)
{

				double	params[256];
				va_list	ap;
				unsigned	int	i=0;

				va_start(ap,	param1);
				params[i]	=	param1;
				i++;
				while	(i	<	nParams)	{
								params[i]	=	va_arg(ap,	double);
								i++;
				}
				va_end(ap);

				if	(i	==	nParams	)	{
							return	(	eval(	params,	0,0));
				}	else	{
							return(0);
				}
}

