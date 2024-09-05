/*********************************	TRICK	HEADER	*******************************
PURPOSE:
				(Ball::Ball	ball	object	constructor.)
REFERENCE:
				(((Bailey,	R.W,	and	Paddock,	E.J.)
						(Trick	Simulation	Environment)	(NASA:JSC	#37943)
						(JSC/Engineering	Directorate/Automation,	Robotics	and	Simulation	Division)
						(March	1997)))
ASSUMPTIONS	AND	LIMITATIONS:
				((2	dimensional	space)
					(X-axis	is	horizontal	and	positive	to	the	right)
					(Y-axis	is	vertical	and	positive	up)
					(resulting	force	is	'collect'ed	in	the	S_define	file))
CLASS:
				(N/A)
LIBRARY	DEPENDENCY:
				((Ball.o))
PROGRAMMERS:
				(((Robert	W.	Bailey)	(Sweet	Systems	Inc)	(March	1997)	(Tutorial	Lesson	1))
					((Edwin	Z.	Crues)(Titan	Systems	Corp.)(Jan	2002)(Crude	C++	translation)))
*******************************************************************************/

/*	System	include	files.	*/
#include	<iostream>

/*	Model	include	files.	*/
#include	"Ball.hh"

//	Default	consructor.
Ball::Ball()	/*	RETURN:	--	None.	*/
{

			//	Print	out	constructor	message.
			//cout	<<	"In	Ball	constructor."	<<	endl;

}

//	Destructor.
Ball::~Ball()	/*	RETURN:	--	None.	*/
{

			//	Print	out	constructor	message.
			//cout	<<	"In	Ball	destructor."	<<	endl;

}

