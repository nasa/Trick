
#include	<iostream>

#include	"trick/Executive.hh"

Trick::JobData	*	Trick::Executive::get_curr_job()	{

				unsigned	int	proc_id	=	get_process_id()	;

				if	(proc_id	==	0)	{
								return(	curr_job	)	;
				}	else	{
								return(	threads.at(proc_id)->curr_job	)	;
				}

}

