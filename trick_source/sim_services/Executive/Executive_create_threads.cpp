
#include	<iostream>
#include	<sstream>
#include	<sys/syscall.h>

#include	"trick/Executive.hh"

int	Trick::Executive::create_threads()	{

				unsigned	int	kk	;

				/**	@par	Detailed	Design	*/

				/**	@li	Start	up	enough	threads	to	handle	the	jobs	specified	in	the	S_define	*/
				for	(	kk	=	1	;	kk	<	threads.size()	;	kk++	)	{
								//	if	this	is	a	restart,	thread	may	have	already	been	created
								if	(threads[kk]->get_pthread_id()	==	0)	{
												threads[kk]->create_thread()	;
								}
								//	set	the	initial	time	of	each	thread	to	the	main	thread	time.
								threads[kk]->curr_time_tics	=	time_tics	;
				}

				/**	@li	Set	the	priority	and	CPU	affinity	for	the	main	thread.	*/
				threads[0]->set_pthread_id(pthread_self());
				threads[0]->set_pid();
				threads[0]->execute_priority()	;
				threads[0]->execute_cpu_affinity()	;

				return(0)	;

}
