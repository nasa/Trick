
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"

int	Trick::Executive::set_thread_process_type(unsigned	int	thread_id	,	int	process_type)	{

				int	ret	;

				/**	@par	Detailed	Design	*/
				if	(	thread_id	==	0	)	{
								/**	@li	If	the	thread_id	matches	the	main	thread,	that	is	an	error	*/
								ret	=	-2	;
				}	else	if	(	(thread_id	+1)	>	threads.size()	)	{
								/**	@li	If	the	thread_id	does	not	exist,	return	an	error	*/
								ret	=	-2	;
				}	else	{
								/**	@li	Call	Trick::Thread::set_thread_process_type	with	the	proces_type	if	the	thread	exists		*/
								ret	=	threads[thread_id]->set_process_type(process_type)	;
				}

				return(ret)	;

}
