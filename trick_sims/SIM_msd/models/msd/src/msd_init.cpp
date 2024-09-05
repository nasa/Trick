/************************TRICK	HEADER*************************
PURPOSE:	Set	the	initial	data	values	of	the	MSD
PROGRAMMERS:
				(((Scott	P.	Fennell)	(CACI	International	Inc.)	(January	2018)	(Trick	Learning	Project)))
*************************************************************/

/*	Model	Include	files	*/
#include	"msd.hh"

/*	initialization	job	*/
int	MSD::init()	{
				x	=	x_0;
				v	=	v_0;
				a	=	(F	-	b*v	-	k*x)/m;
				return	0;
}
