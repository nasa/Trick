
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"

/**
@design
-#	If	the	thread_id	matches	the	main	thread,	that	is	an	error
-#	If	the	thread_id	does	not	exist,	return	an	error
-#	Call	Trick::Thread::set_enabled(bool)	with	the	enabled	flag
*/
int	Trick::Executive::set_thread_enabled(unsigned	int	thread_id	,	bool	in_enabled)	{

				if	(	thread_id	==	0	)	{
								return	-2	;
				}	else	if	(	(thread_id	+1)	>	threads.size()	)	{
								return	-2	;
				}

				threads[thread_id]->set_enabled(	in_enabled	)	;
				return	0	;

}
