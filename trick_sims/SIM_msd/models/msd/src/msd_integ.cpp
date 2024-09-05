/************************TRICK	HEADER*************************
		PURPOSE:	(	Trick	integration	)
		PROGRAMMERS:
				(((Scott	P.	Fennell)	(CACI	International	Inc.)	(January	2018)	(Trick	Learning	Project)))
*************************************************************/
#include	"msd.hh"
#include	"trick/Integrator.hh"
#include	"trick/integrator_c_intf.h"
#include	<math.h>

int	MSD::state_integ(void)	{
				int	ipass;
					load_state(
						&x,
						&v,
						NULL);

					load_deriv(
						&v,
						&a,
						NULL);
					
					ipass	=	integrate();

					unload_state(
						&x,
						&v,	
			NULL);

				return(ipass);
}