/*********************************	TRICK	HEADER	*******************************
PURPOSE:
				(Ball::state_deriv	performs	the	following:
								-	collects	all	the	external	forces	on	the	ball
								-	solves	the	EOM	F=ma	for	accelerations)
REFERENCE:
				(((Bailey,	R.W,	and	Paddock,	E.J.)
						(Trick	Simulation	Environment)	(NASA:JSC	#37943)
						(JSC/Engineering	Directorate/Automation,	Robotics	and	Simulation	Division)
						(March	1997)))
ASSUMPTIONS	AND	LIMITATIONS:
				((2	dimensional	space)
					(X-axis	is	horizontal	and	positive	to	the	right)
					(Y-axis	is	vertical	and	positive	up)
					(derivative	of	position	already	exists	as	velocity	vector)
					(collect	contains	external	forces	'collect'ed	in	the	S_define	file))
CLASS:
				(derivative)
LIBRARY	DEPENDENCY:
				((BallStateDeriv.o))
PROGRAMMERS:
				(((Robert	W.	Bailey)	(Sweet	Systems	Inc)	(March	1997)	(Tutorial	Lesson	1))
					((Edwin	Z.	Crues)(Titan	Systems	Corp.)(Jan	2002)(Crude	C++	translation)))
*******************************************************************************/

/*	System	include	files.	*/
#include	<stdio.h>
#include	<iostream>

/*	Trick	include	files.	*/
#include	"trick/collect_macros.h"

/*	Model	include	files.	*/
#include	"Ball.hh"

/*	ENTRY	POINT	*/
int	Ball::state_deriv()	/*	RETURN:	--	Always	return	zero.	*/
{

			/*	GET	SHORTHAND	NOTATION	FOR	DATA	STRUCTURES	*/
			BallStateInput		*	state_in			=	&(this->state.input);
			BallStateOutput	*	state_out		=	&(this->state.output);
			BallStateWork			*	state_work	=	&(this->state.work);

			/*	COLLECT	EXTERNAL	FORCES	ON	THE	BALL		--		TRUST	US	ON	THIS	ONE	*/
			//collected_forces	=	(double**)(state_work->external_force)	;
			state_out->external_force[0]	=	0.0	;
			state_out->external_force[1]	=	0.0	;

			unsigned	int	ii;
			for(	ii	=	0	;	ii	<	state_work->num_external_force	;	ii++	)	{
						state_out->external_force[0]	+=	state_work->external_force[ii][0];
						state_out->external_force[1]	+=	state_work->external_force[ii][1];
			}

			/*	SOLVE	FOR	THE	X	AND	Y	ACCELERATIONS	OF	THE	BALL	*/
			state_out->acceleration[0]	=	state_out->external_force[0]	/	state_in->mass;
			state_out->acceleration[1]	=	state_out->external_force[1]	/	state_in->mass;

			/*	RETURN	*/
			return(0);

}

