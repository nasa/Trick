/************************TRICK	HEADER*************************
PURPOSE:	MSD	default	data	job
PROGRAMMERS:
				(((Scott	P.	Fennell)	(CACI	International	Inc.)	(January	2018)	(Trick	Learning	Project)))
*************************************************************/
#include	"msd.hh"

/*	default	data	job	*/
int	MSD::default_data(MSD	&M)	{
				M	=	MSD(
												1.0,	/*	m	*/
												2.0,	/*	k	*/
												0.5,	/*	b	*/
												5.0,	/*	F	*/
												0.0,	/*	v_0	*/
												5.0		/*	x_0	*/
				);

				return	0;
}