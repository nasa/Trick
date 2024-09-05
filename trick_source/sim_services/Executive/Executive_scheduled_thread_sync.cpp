
#include	<iostream>

#include	"trick/Executive.hh"
#include	"trick/release.h"

/**
@design
-#	Loop	through	all	threads.
			-#	Wait	for	thread	to	finish	if	the	thread	is	PROCESS_TYPE_SCHEDULED
*/
int	Trick::Executive::scheduled_thread_sync()	{

				unsigned	int	ii	;

				/*	Wait	for	synchronous	threads	to	finish	before	testing	for	adjusting	time_tics	*/
				for	(ii	=	1;	ii	<	threads.size()	;	ii++)	{
								Threads	*	curr_thread	=	threads[ii]	;
								if	(	curr_thread->enabled	and	curr_thread->process_type	==	PROCESS_TYPE_SCHEDULED)	{
												while	(curr_thread->child_complete	==	false	)	{
																if	(rt_nap	==	true)	{
																				RELEASE();
																}
												}
								}
				}

				return	0	;
}

