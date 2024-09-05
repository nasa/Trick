
#include	"trick/Executive.hh"

/*
-#	Switch	on	the	type	of	thread
			-#	Scheduled	threads	are	always	ready	to	run
			-#	AMF	thredas	are	ready	to	run	if	their	previous	frame	just	finished
			-#	ASYNC	threads	are	ready	to	run	if	their	complete	flag	is	set	and	either	they	have
						no	cycle	time	or	their	previous	frame	just	finished.
*/
bool	Trick::Executive::isThreadReadyToRun(	Trick::Threads	*	curr_thread	,	long	long	time_ticks)	{
				bool	ret	=	false	;
				if	(	curr_thread->enabled	)	{
								switch	(	curr_thread->process_type	)	{
												case	Trick::PROCESS_TYPE_SCHEDULED:
																ret	=	true	;
																break	;
												case	Trick::PROCESS_TYPE_AMF_CHILD:
																if	(	curr_thread->amf_next_tics	==	time_ticks	)	{
																				ret	=	true	;
																}
																break	;
												case	Trick::PROCESS_TYPE_ASYNC_CHILD:
																if	(	curr_thread->child_complete	==	true	)	{
																				if	(curr_thread->amf_cycle_tics	==	0	)	{
																								ret	=	true	;
																				}	else	{
																								if	(	curr_thread->amf_next_tics	==	time_ticks	)	{
																												ret	=	true	;
																								}
																				}
																}
																break	;
								}
				}
				return	ret	;
}

