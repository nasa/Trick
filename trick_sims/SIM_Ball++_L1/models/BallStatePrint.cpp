/*********************************	TRICK	HEADER	*******************************
PURPOSE:
				(Ball::state_init	performs	the	following:
								-	passes	the	input	position	vector	to	the	output	vector
								-	xforms	the	input	ball	speed	and	trajectory	elevation	into	a
										velocity	vector.)
REFERENCE:
				(((Bailey,	R.W,	and	Paddock,	E.J.)
						(Trick	Simulation	Environment)	(NASA:JSC	#37943)
						(JSC/Engineering	Directorate/Automation,	Robotics	and	Simulation	Division)
						(March	1997)))
ASSUMPTIONS	AND	LIMITATIONS:
				((2	dimensional	space)
					(Positive	X	is	horizontal	to	the	right)
					(Positive	Y	is	vertical	and	up))
CLASS:
				(initialization)
LIBRARY	DEPENDENCY:
				((BallStateInit.o))
PROGRAMMERS:
				(((Robert	W.	Bailey)	(Sweet	Systems	Inc)	(March	1997)	(Tutorial	Lesson	1))
					((Edwin	Z.	Crues)(Titan	Systems	Corp.)(Jan	2002)(Crude	C++	translation)))
*******************************************************************************/

/*	System	include	files.	*/
#include	<math.h>
#include	<iostream>
#include	<unistd.h>

/*	Model	include	files.	*/
#include	"Ball.hh"
#include	"trick/exec_proto.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"

					/*	ENTRY	POINT	*/
int	Ball::state_print()	{

			message_publish(MSG_NORMAL,	"time	=	%8.2f	,	position	=	%12.6f	,	%12.6f\n",
				exec_get_sim_time()	,	state.output.position[0]	,	state.output.position[1])	;

			/*	RETURN	*/
			return(	0	);

}

