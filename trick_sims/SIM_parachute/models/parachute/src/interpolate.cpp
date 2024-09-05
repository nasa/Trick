#include	"../include/interpolate.h"
#include	<stdio.h>

double	interpolate(	double	x,	const	double	xa[],	const	double	fa[],	int	N_elems	)	{
				int	ii;
				for	(ii=0	;	((ii+2	<	N_elems	)	&&	(x	>	xa[ii+1]))	;	ii++	)	;
				double	x_lower	=	xa[ii];
				double	x_upper	=	xa[ii+1];
				double	f_lower	=	fa[ii];
				double	f_upper	=	fa[ii+1];
				if	(x	<	x_lower)	{
								fprintf(stderr,	"Interpolation	x	is	out	of	range	low.\n");
								return(f_lower);
				}
				if	(x	>	x_upper)	{
								fprintf(stderr,	"Interpolation	x	is	out	of	range	high.\n");
								return(f_upper);
				}
				double	f	=	(x_upper	-	x)/(x_upper	-	x_lower)	*	f_lower	+
															(x	-	x_lower)/(x_upper	-	x_lower)	*	f_upper	;
				return(f);
}
