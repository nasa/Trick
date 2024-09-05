/************************TRICK	HEADER*************************
PURPOSE:	(Print	the	final	msd	state.)
PROGRAMMERS:
				(((Scott	P.	Fennell)	(CACI	International	Inc.)	(January	2018)	(Trick	Learning	Project)))
*************************************************************/
#include	"msd.hh"
#include	"trick/exec_proto.h"
#include	<stdio.h>

int	MSD::shutdown()	{
				printf("========================================\n");
				printf("									MSD	State	at	Shutdown					\n");
				printf("pos	=	[%g],	vel	=	[%g],	acc	=	[%g]\n",
													x,	v,	a);
				printf("========================================\n");
				return	0;
}