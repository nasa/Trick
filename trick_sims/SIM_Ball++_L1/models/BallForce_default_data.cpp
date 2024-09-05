/*********************************	TRICK	HEADER	*******************************
PURPOSE:
				(Ball::force_field	performs	the	following:
								-	computes	a	relative	vector	from	the	ball	to	the	force	field	origin
								-	computes	the	unit	vector	in	the	direction	of	this	relative	vector
								-	scales	the	unit	vector	by	the	magnitude	of	the	constant	force	field)
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
				(derivative)
LIBRARY	DEPENDENCY:
				((BallForce_default_data.o))
PROGRAMMERS:
				(((Robert	W.	Bailey)	(Sweet	Systems	Inc)	(March	1997)	(Tutorial	Lesson	1))
					((Edwin	Z.	Crues)(Titan	Systems	Corp.)(Jan	2002)(Crude	C++	translation)))
*******************************************************************************/

/*	System	include	files.	*/
#include	<stdexcept>
#include	<math.h>

/*	Model	include	files.	*/
#include	"BallForce.hh"

/*	ENTRY	POINT	*/
int	BallForce::default_data()	{

				//	This	data	is	the	same	as	in	the	constructor.
				input.origin[0]	=	0.0	;
				input.origin[1]	=	2.0	;
				input.force	=	8.0	;

				return(0)	;
}

