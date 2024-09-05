
#include	<iostream>

#include	"trick/Executive.hh"

double	Trick::Executive::get_sim_time()	{

				unsigned	int	ii	;
				unsigned	int	num_threads	;
				pthread_t	curr_pthread_id	;

				num_threads	=	get_num_threads()	;

				if	(	num_threads	==	1	)	{
								return((double)threads[0]->curr_time_tics	/	time_tic_value)	;
				}	else	if	(	num_threads	>	1	)	{
								curr_pthread_id	=	pthread_self()	;
								for	(ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
												if	(	pthread_equal(curr_pthread_id,threads[ii]->get_pthread_id())	)	{
																return((double)threads[ii]->curr_time_tics	/	time_tic_value)	;
												}
								}
				}

				return	((double)time_tics	/	time_tic_value)	;

}

long	long	Trick::Executive::get_time_tics()	{

				unsigned	int	ii	;
				unsigned	int	num_threads	;
				pthread_t	curr_pthread_id	;

				num_threads	=	get_num_threads()	;

				if	(	num_threads	==	1	)	{
								return(threads[0]->curr_time_tics)	;
				}	else	if	(	num_threads	>	1	)	{
								curr_pthread_id	=	pthread_self()	;
								for	(ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
												if	(	pthread_equal(curr_pthread_id,threads[ii]->get_pthread_id())	)	{
																return(threads[ii]->curr_time_tics)	;
												}
								}
				}

				return	(time_tics)	;

}
