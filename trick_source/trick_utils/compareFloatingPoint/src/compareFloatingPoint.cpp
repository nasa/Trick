#include	<float.h>
#include	<math.h>
#include	"trick/compareFloatingPoint.hh"

bool	Trick::dbl_is_near(	double	A,	double	B,	double	tolerance)	{

				if	(isnan(A)	||	isnan(B)	||	isinf(A)	||	isinf(B))	{	return	false;	}

				//	If	A	or	B	is	a	FP_SUBNORMAL	that	is:	less	than
				//	DBL_MIN	(2.22507e-308)	then	set	it	to	zero.
				if	(	fpclassify(A)	==	FP_SUBNORMAL	)	{	A	=	0.0;	}
				if	(	fpclassify(B)	==	FP_SUBNORMAL	)	{	B	=	0.0;	}

				//	If	A	and	B	are	identical,	then	they're	close_enough.
				if	(A==B)	{	return	true;	}

				//	The	tolerance	must	be	an	FP_NORMAL.	Neither	of	FP_INFINITE	and
				//	FP_NAN	makes	sense.	Nor	do	FP_SUBNORMAL	and	FP_ZERO,	given
				//	the	above	tests.

				//	In	order	than	we	not	generate	an	FP_underflow,	we	must	set	the	minimum
				//	allowable	tolerance	such	that	fmin(A,B)+tolerance	(or	fmax(A,B)-tolerance)
				//	cannot	be	FP_SUBNORMAL.	This	is	only	possible	if	(tolerance	>=	DBL_MIN/DBL_EPSILON),
				//	where	the	gap_size	around	tolerance	>=	DBL_MIN,	the	smallest	FP_NORMAL	number.
				//	So,	if	A	and	B	are	within	1.00208e-292	of	each	other,	they	will	always	be
				//	considered	close_enough.
				if	((	fpclassify(tolerance)	!=	FP_NORMAL	)	||	(tolerance	<	(DBL_MIN/DBL_EPSILON)	))	{
							tolerance	=	(DBL_MIN/DBL_EPSILON);
				}

				//	For	A	and	B	to	be	close	enough,	the	tolerance	must	be	greater	than	or
				//	equal	to	the	larger	of	the	gaps	around	A	and	B.
				if	(	tolerance	>=	DBL_EPSILON	*	fmax(	fabs(A),	fabs(B)))	{

								//	We	want	to	avoid	directly	computing	the	difference	between	A	and	B.
								//	We	might	otherwise	generate	an	FP_SUBNORMAL.	For	example:
								//	If	A	=	(1.5*DBL_MIN),	and	B	=	DBL_MIN,	the	difference	is	an
								//	FP_SUBNORMAL	value.
								//	When	FP_SUBNORMAL	values	are	generated,	so	are	FP_underflows.
								//	By	insisting	that	A	and	B	are	FP_NORMAL	and	that	the
								//	gap_size	around	our	tolerance	is	at	least	DBL_MIN,	then	we
								//	can	avoid	generating	FP_SUBNORMALs.
								if	((fmin(A,B)	+	tolerance)	>=	fmax(A,B))	{
												return	true	;	//	A	and	B	are	close	enough.
								}	else	{
												return	false	;
								}
				}	else	{
								return	false;
				}
}

bool	Trick::flt_is_near(	float	A,	float	B,	float	tolerance)	{
				if	(isnan(A)	||	isnan(B)	||	isinf(A)	||	isinf(B))	{	return	false;	}
				if	(	fpclassify(A)	==	FP_SUBNORMAL	)	{	A	=	0.0;	}
				if	(	fpclassify(B)	==	FP_SUBNORMAL	)	{	B	=	0.0;	}
				if	(A==B)	{	return	true;	}
				if	((	fpclassify(tolerance)	!=	FP_NORMAL	)	||	(tolerance	<	(FLT_MIN/FLT_EPSILON)	))	{
							tolerance	=	(FLT_MIN/FLT_EPSILON);
				}
				if	(	tolerance	>=	FLT_EPSILON	*	fmax(	fabs(A),	fabs(B)))	{
								if	((fmin(A,B)	+	tolerance)	>=	fmax(A,B))	{
												return	true	;
								}	else	{
												return	false	;
								}
				}	else	{
								return	false;
				}
}
