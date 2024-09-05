
#include	<iostream>

#include	"trick/Executive.hh"

unsigned	int	Trick::Executive::get_process_id()	{

				unsigned	int	ii	;
				pthread_t	curr_pthread_id	;

				if	(	get_num_threads()	>	1	)	{
								curr_pthread_id	=	pthread_self()	;
								for	(ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
												if	(	pthread_equal(curr_pthread_id,threads[ii]->get_pthread_id())	)	{
																return(ii)	;
												}
								}
				}

				return(0)	;

}

