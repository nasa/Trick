/************************TRICK	HEADER*************************
PURPOSE:	MSD	is	a	mass-spring-damper	class	for	a	trick	sim.
PROGRAMMERS:
				(((Scott	P.	Fennell)	(CACI	International	Inc.)	(January	2018)	(Trick	Learning	Project)))
*************************************************************/
#include	<math.h>
#include	"msd.hh"
#include	<float.h>

MSD::MSD(double	_m,	double	_k,	double	_b,	double	_F,	double	_v_0,	double	_x_0)	{
				m	=	_m;
				k	=	_k;
				b	=	_b;
				F	=	_F;
				v_0	=	_v_0;
				x_0	=	_x_0;
}

MSD::MSD(void)	{/*	not	used	*/}

