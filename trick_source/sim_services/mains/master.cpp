/*
PURPOSE:																				(The	Trick	simulation	executive	MAIN	program.)
REFERENCE:																		((Bailey,	R.W,	and	Paddock,	E.J.)
																													(Trick	Simulation	Environment)
																													(JSC	/	Automation	and	Robotics	Division)
																													(June	1994))
ASSUMPTIONS	AND	LIMITATIONS:	(It	all	starts	here)
PROGRAMMERS:																	(	(Robert	W.	Bailey)	(LinCom)	(April	1992)	)
*/

#include	<iostream>
#include	<locale.h>

#include	"trick/Executive.hh"
#include	"trick/command_line_protos.h"
#include	"trick/exec_proto.hh"

//TODO	move	to	proto
void	memory_init(void);

int	master(	int	nargs,	char	**args)	{

				int	ret	;

				/*	Set	the	locale	*/
				if		(setlocale(LC_CTYPE,	"")	==	NULL)	{
								fprintf(stderr,	"Unable	to	set	the	locale.\n");
								fprintf(stderr,	"Please	check	LANG,	LC_CTYPE	and	LC_ALL.\n");
								return	1;
				}

				/*	adds	sim	objects	including	the	executive	defined	at	CP	time	*/
				memory_init()	;

				/*	process	generic	command	line	arguments	*/
				//Trick::CommandLineArguments	*	cmd_args	=	exec_get_cmd_args()	;
				command_line_args_process_sim_args(nargs,	args)	;

				/*	get	the	exec	pointer	*/
				Trick::Executive	*	exec	=	exec_get_exec_cpp()	;

				ret	=	exec->init()	;

				if	(	ret	==	0	)	{
								exec->loop()	;
				}
				ret	=	exec->shutdown()	;

				//TODO:	add	call	to	free	all	memory	from	memory	manager

				return	ret	;
}

