/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Example	of	an	"automatic"	(self-scheduling)	job.	)
LIBRARY	DEPENDENCY:
				((Fibonacci_time.o))
*******************************************************************************/
#include	"../include/Fibonacci_time.hh"
#include	<math.h>
#include	<iostream>

#include	"trick/exec_proto.hh"
#include	"trick/exec_proto.h"

const	char	*	ISO_6429_Restore_Default			=	"\x1b[00m";
const	char	*	ISO_6429_Red_Foreground				=	"\x1b[31m";
const	char	*	ISO_6429_Green_Foreground		=	"\x1b[32m";

int	Fibonacci(int	n)	{
				if	(n<=1)	{
								return	n;
				}	else	{
								return	Fibonacci(n-1)	+	Fibonacci(n-2)	;
				}
}

int	Fibonacci_time::default_data()	{
				N	=	1;
				tics_per_second	=	exec_get_time_tic_value();
				return	(0);
}

int	Fibonacci_time::state_init()	{
				return	(0);
}

int	Fibonacci_time::scheduled_job_1()	{

				double	time_in_seconds	=	(double)exec_get_time_tics()	/	tics_per_second;
				std::cout	<<	ISO_6429_Green_Foreground
														<<	"Scheduled	Job	Time	=	"	<<	time_in_seconds
														<<	ISO_6429_Restore_Default	<<	std::endl;	

				return(0);
}

int	Fibonacci_time::automatic_job_1()	{

				double	time_in_seconds	=	(double)exec_get_time_tics()	/	tics_per_second;
				std::cout	<<	ISO_6429_Red_Foreground
														<<	"Automatic	Job	Time	=	"	<<	time_in_seconds
														<<	ISO_6429_Restore_Default	<<	std::endl;	

				N++;
				double	next_time	=	Fibonacci(N);

				Trick::JobData	*	thisJob	=	exec_get_curr_job()	;
				thisJob->set_next_call_time(next_time	*	tics_per_second);

				return(0);
}
