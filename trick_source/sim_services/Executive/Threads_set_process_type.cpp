
#include	"trick/Threads.hh"

int	Trick::Threads::set_process_type(int	in_process_type)	{

				/**	@par	Detailed	Design:	*/
				/**	@li	After	all	jobs	have	completed,	set	the	child_complete	flag	to	true.	Design	[@ref	d_exec_child_3]	*/
				if	(	in_process_type	>	PROCESS_TYPE_AMF_CHILD	||	in_process_type	<	PROCESS_TYPE_SCHEDULED	)	{
								return(-1)	;
				}	else	{
								process_type	=	(Trick::ProcessType)in_process_type	;
				}

				return(0)	;
}
