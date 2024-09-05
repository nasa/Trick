#include	"starter.h"
#include	<iostream>
#include	"trick/exec_proto.hh"
#include	"trick/exec_proto.h"

void	Starter::sched	()	{
				sched_calls++;
}

void	Starter::custom	()	{
				custom_calls++;
}

void	Starter::top	()	{
				top_calls++;
}

void	Starter::automatic	()	{

				Trick::JobData	*	thisJob	=	exec_get_curr_job()	;
				thisJob->set_next_call_time(exec_get_time_tics()	+	exec_get_time_tic_value());
				automatic_calls++;
}

void	Starter::automatic_last	()	{

				Trick::JobData	*	thisJob	=	exec_get_curr_job()	;
				thisJob->set_next_call_time(exec_get_time_tics()	+	exec_get_time_tic_value());

				automatic_last_calls++;
}

void	Starter::end()	{
				end_calls++;
}

void	Starter::deriv()	{				
				deriv_calls++;
}

int	Starter::integ()	{				
				integ_calls++;

				return	0;
}

void	Starter::test_calls()	{
				if	(top_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"top_of_frame	job	never	called");
				}

				if	(custom_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"custom	job	never	called");
				}

				if	(sched_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"scheduled	job	never	called");
				}

				if	(automatic_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"automatic	job	never	called");
				}

				if	(automatic_last_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"automatic_last	job	never	called");
				}

				if	(deriv_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"derivative	job	never	called");
				}

				if	(integ_calls	==	0)	{
								exec_terminate_with_return(1,	__FILE__,	__LINE__,	"integration	job	never	called");
				}

}
