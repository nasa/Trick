
#include	"trick/Executive.hh"

int	Trick::Executive::set_thread_cpu_affinity(unsigned	int	thread_id	,	int	cpu_num)	{

				int	ret	=	0	;

				/**	@par	Detailed	Design	*/
				if	(	(thread_id	+1)	>	threads.size()	)	{
								/**	@li	If	the	thread_id	does	not	exist,	return	an	error	*/
								ret	=	-2	;
				}	else	{
								threads[thread_id]->cpu_set(cpu_num)	;
				}

				return(ret)	;

}

