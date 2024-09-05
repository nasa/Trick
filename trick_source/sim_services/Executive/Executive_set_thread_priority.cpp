
#include	"trick/Executive.hh"

int	Trick::Executive::set_thread_priority(unsigned	int	thread_id	,	unsigned	int	req_priority)	{

				int	ret	=	0	;

				/**	@par	Detailed	Design	*/
				if	(	(thread_id	+	1)	>	threads.size()	)	{
								/**	@li	If	the	thread_id	does	not	exist,	return	an	error	*/
								ret	=	-2	;
				}	else	{
								/**	@li	Call	Trick::Thread::set_thread_process_type	with	the	proces_type	if	the	thread	exists		*/
								threads[thread_id]->set_priority(req_priority)	;
				}

				return(ret)	;

}

