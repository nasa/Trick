/*********************************	TRICK	HEADER	*******************************
PURPOSE:
				(Ball::state_integ	performs	the	following:
								-	loads	the	position	states	into	the	INTEGRATOR	state	ws	arrays
								-	loads	the	velocity	states	into	the	INTEGRATOR	state	derivative	ws
								-	loads	the	velocity	states	into	the	INTEGRATOR	state	ws	arrays
								-	loads	the	acceleration	states	into	the	INTEGRATOR	state	derivative	ws
								-	calls	the	TRICK	state	integration	service
								-	unloads	the	new	pos	and	vel	states	from	the	INTEGRATOR	ws	arrays)
REFERENCE:
				(((Bailey,	R.W,	and	Paddock,	E.J.)
						(Trick	Simulation	Environment)	(NASA:JSC	#37943)
						(JSC/Engineering	Directorate/Automation,	Robotics	and	Simulation	Division)
						(March	1997)))
ASSUMPTIONS	AND	LIMITATIONS:
				((2	dimensional	space)
					(integrate	accel	to	pos	as	two	first	order	diffEQs))
CLASS:
				(integration)
LIBRARY	DEPENDENCY:
				((BallStateInteg.o))
PROGRAMMERS:
				(((Robert	W.	Bailey)	(Sweet	Systems	Inc)	(March	1997)	(Tutorial	Lesson	1))
					((Edwin	Z.	Crues)(Titan	Systems	Corp.)(Jan	2002)(Crude	C++	translation)))
*******************************************************************************/

/*	Model	include	files.	*/
#include	<stdio.h>

#include	"trick/integrator_c_intf.h"
#include	"Ball.hh"
#include	<iostream>

/*	ENTRY	POINT	*/
int	Ball::state_integ()	{

			int	ipass;

			/*	GET	SHORTHAND	NOTATION	FOR	DATA	STRUCTURES	*/
			BallStateOutput	*	state_out	=	&(this->state.output);

			/*	LOAD	THE	POSITION	AND	VELOCITY	STATES	*/
			load_state(
							&state_out->position[0]	,
							&state_out->position[1]	,
							&state_out->velocity[0]	,
							&state_out->velocity[1]	,
							NULL
			);

			/*	LOAD	THE	POSITION	AND	VELOCITY	STATE	DERIVATIVES	*/
			load_deriv(
							&state_out->velocity[0]	,
							&state_out->velocity[1]	,
							&state_out->acceleration[0]	,
							&state_out->acceleration[1]	,
							NULL
			);

			/*	CALL	THE	TRICK	INTEGRATION	SERVICE	*/
			ipass	=	integrate();

			/*	UNLOAD	THE	NEW	POSITION	AND	VELOCITY	STATES	*/
			unload_state(
							&state_out->position[0]	,
							&state_out->position[1]	,
							&state_out->velocity[0]	,
							&state_out->velocity[1]	,
							NULL
			);

			/*	RETURN	*/
			return(	ipass	);

}
