/*
			PURPOSE:	(This	function	switches	on	signal	type	to	act	on	specific	signal.	This	function	performs	the	signal
			handling	for	both	master	and	child	processes.)

			REFERENCE:	(((Bailey,	R.W,	and	Paddock,	E.J.)	(Trick	Simulation	Environment)	(NASA:JSC	#37943)	(JSC	/	Engineering
			Directorate	/	Automation	and	Robotics	Division)	(June	1994)	(--)))

			ASSUMPTIONS	AND	LIMITATIONS:	((None))

			CLASS:	(N/A)

			LIBRARY	DEPENDENCY:	((none))

			PROGRAMMERS:	(((Eddie	J.	Paddock)	(MDSSC)	(April	1992)	(--)	(Realtime))	((Scott	Killingsworth)	(LinCom)	(September
			1997)	(--)	(Issue	#1016)))	*/

#include	<iostream>
#include	<sstream>

#include	<stdlib.h>
#include	<string.h>
#include	<signal.h>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.hh"

/**
	*	@relates	Trick::Executive
	*	C	binded	function	to	handle	UNIX	signals	for	the	simulation.
	*	@return	void
	*/
void	sig_hand(int	sig)	{
				Trick::Executive	*	E	=	exec_get_exec_cpp();
				E->signal_handler(sig)	;
				return;
}

void	ctrl_c_hand(int	sig	__attribute__	((unused)))	{
				Trick::Executive	*	E	=	exec_get_exec_cpp();
				E->ctrl_c_handler()	;
				return;
}

void	term_hand(int	sig	__attribute__	((unused)))	{
				Trick::Executive	*	E	=	exec_get_exec_cpp();
				E->term_handler()	;
				return;
}

void	usr1_hand(int	sig	__attribute__	((unused)))	{
				Trick::Executive	*	E	=	exec_get_exec_cpp();
				E->usr1_handler()	;
				return;
}
