#include	"Rootfinder.hh"
#include	<math.h>
#include	<float.h>
#include	<iostream>

void	RootFinder::init(	double	tolerance,	SlopeConstraint	constraint)	{
				iterations	=	0;
				prev_f_error	=	DBL_MAX;
				lower_set	=	false;
				upper_set	=	false;
				f_error_tol	=	tolerance;
				slope_constraint	=	constraint;
}
void	RootFinder::init()	{
				init(0.00000000001,	Unconstrained);
}
//	Default	Constructor
RootFinder::RootFinder	()	{
				init();
}
//	Constructor
RootFinder::RootFinder	(double	tolerance,	SlopeConstraint	constraint)	{
				init(tolerance,	constraint);
}
//	Copy	Constructor:	The	default	copy_constructor	should	suffice.
//	Assignment	Operator:	The	default	assignment	operator	should	suffice.
//	Destructor:	The	default	destructor	should	suffice.

//	Given	the	values	of	the	independent	variable,	and	the	function,
//	estimate	the	distance	of	the	independent	variable	from	functions	root.
double	RootFinder::find_roots(	double	x,	double	f_error	)	{
				if	(		(	fabs(f_error)	<	f_error_tol)	||
										((	iterations	>	0)	&&	(fabs(prev_f_error	-	f_error)	<	f_error_tol))
							)	{
								if	(slope_constraint	==	Unconstrained)	{
												return	(0.0);
								}	else	if	(slope_constraint	==	Positive	&&	lower_set)	{
												return	(0.0);
								}	else	if	(slope_constraint	==	Negative	&&	upper_set)	{
												return	(0.0);
								}
				}
				if	(f_error	<	0.0)	{
								f_lower	=	f_error;
								x_lower	=	x;
								lower_set	=	true;
				}	else	if	(f_error	>	0.0)	{
								f_upper	=	f_error;
								x_upper	=	x;
								upper_set	=	true;
				}
				iterations++;
				if	(upper_set	&&	lower_set)	{
								double	x_correction_estimate;
								if	(fabs(f_error)	<	f_error_tol)
												x_correction_estimate	=	0.0;
								else	{
												x_correction_estimate	=	-f_error/((f_upper-f_lower)/(x_upper-x_lower));
												if	(iterations	>	20)
																x_correction_estimate	=	0.0;
								}
								/*	Check	for	Positive	or	Negative	constraint	*/
								switch	(slope_constraint)	{
												case	Unconstrained:
																prev_f_error	=	f_error;
																return	(x_correction_estimate);
												case	Positive:
																if	(f_slope	==	Positive)	{
																				prev_f_error	=	f_error;
																				return	(x_correction_estimate);
																}	else	{
																				lower_set	=	false;
																				f_slope	=	Positive;
																}
																break;
												case	Negative:
																if	(f_slope	==	Negative)	{
																				prev_f_error	=	f_error;
																				return	(x_correction_estimate);
																}	else	{
																				upper_set	=	false;
																				f_slope	=	Negative;
																}
																break;
								}
								f_slope	=	Unconstrained;
				}	else	if	(lower_set	==	1)	{
								f_slope	=	Positive;
				}	else	if	(upper_set	==	1)	{
								f_slope	=	Negative;
				}
				iterations	=	0;
				return	(DBL_MAX);
}

std::ostream&	operator<<(std::ostream&	os,	const	RootFinder&	rf)	{
				os	<<	"\n---	RootFinder	---";
				os	<<	"\nupper:	{"	<<	rf.f_upper	<<	','	<<	rf.x_upper	<<	','	<<	rf.upper_set	<<	"}";
				os	<<	"\nlower:	{"	<<	rf.f_lower	<<	','	<<	rf.x_lower	<<	','	<<	rf.lower_set	<<	"}";
				os	<<	"\nprev_f_error:	"	<<	rf.prev_f_error;
				os	<<	"\nf_error_tol:"	<<	rf.prev_f_error;
				os	<<	"\niterations:"	<<	rf.prev_f_error;
				os	<<	"\nslope_constraint:"	<<	rf.slope_constraint;
				os	<<	"\nf_slope:"	<<	rf.f_slope;
				return	os;
}
