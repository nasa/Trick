/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	satellite	orbiting	the	Earth.	)
LIBRARY	DEPENDENCY:
				((Satellite.o))
*******************************************************************************/
#include	"../include/Satellite.hh"
#include	<math.h>
#include	<iostream>

int	Satellite::default_data()	{

				pos[0]	=	0.0;
				pos[1]	=	6578000.0;
				vel[0]	=	7905.0;
				vel[1]	=	0.0;
				return	(0);
}

int	Satellite::state_init()	{
				return	(0);
}

int	Satellite::state_deriv()	{

			double	d	=	sqrt(	pos[0]*pos[0]	+	pos[1]*pos[1]);
			acc[0]	=	-pos[0]	*	GRAVITATIONAL_CONSTANT	*	EARTH_MASS	/	(d*d*d);
			acc[1]	=	-pos[1]	*	GRAVITATIONAL_CONSTANT	*	EARTH_MASS	/	(d*d*d);	
			return(0);
}

#include	"sim_services/Integrator/include/integrator_c_intf.h"

int	Satellite::state_integ()	{

			int	integration_step;

			load_state	(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);
			load_deriv	(	&vel[0],	&vel[1],	&acc[0],	&acc[1],	(double*)0);

			integration_step	=	integrate();

			unload_state(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	(double*)0);

			return(integration_step);
}

