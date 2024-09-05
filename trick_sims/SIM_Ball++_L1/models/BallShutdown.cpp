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

#include	"trick/trick_tests.h"

/*	Model	include	files.	*/
#include	"Ball.hh"

					/*	ENTRY	POINT	*/
int	Ball::shutdown()	{

			TRICK_EXPECT_NEAR(	state.output.position[0]	,	-7.627512	,	0.000001	,	"BallSim"	,	"FinalXPositionCheck"	)
			TRICK_EXPECT_NEAR(	state.output.position[1]	,	3.131932	,	0.000001	,	"BallSim"	,	"FinalYPositionCheck"	)


			/*	RETURN	*/
			return(	0	);

}

