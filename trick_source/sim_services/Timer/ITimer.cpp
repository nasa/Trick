/*
PURPOSE:
				(	gettimeofday	clock	)
PROGRAMMERS:
				(((Robert	W.	Bailey)	(LinCom)	(April	1992)	(--)	(Realtime.))
					((Robert	W.	Bailey)	(LinCom)	(7/22/92)	(--)	(--))
					((Eddie	J.	Paddock)	(MDSSC)	(Oct	1992)	(--)	(Realtime.)))
					((Alex	Lin)	(NASA)	(April	2009)	(--)	(c++	port)))
*/

#include	<iostream>
#include	<sstream>

#include	<fcntl.h>
#include	<stdio.h>
#include	<string.h>
#include	<errno.h>
#include	<math.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<unistd.h>

#include	"trick/ITimer.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"

Trick::ITimer	*	the_itimer	;

Trick::ITimer::ITimer()	:	Timer()	{

				the_itimer	=	this	;

#if	!	(	__APPLE__	|	__INTERIX	)
				clock_getres(CLOCK_REALTIME,	&res);
#else
				res.tv_nsec	=	10000000;
				res.tv_sec	=	0;
#endif

				return	;
}

Trick::ITimer::~ITimer()	{
				return	;
}

void	Trick::ITimer::semaphore_post()	{
				sem_post(semaphore)	;
}

/**
@details
-#	Set	the	SIGALRM	signal	handler	to	the	ITimer	signal	handler	routine.
-#	Open	a	unique	named	semaphore
*/
int	Trick::ITimer::init()	{

				static	struct	sigaction	sigact;
				std::stringstream	sem_name_stream	;

				sigact.sa_handler	=	(void	(*)(int))	it_handler;
				if	(sigaction(SIGALRM,	&sigact,	NULL)	<	0)	{
								perror("sigaction()	failed	for	SIGALRM");
				}

				//	Initialize	the	itimer	semaphore	used	by	SIGALRM	handler	to
				//	wake	up	the	sim_exec_loop	from	its	underrun	sleepy	time.

				sem_name_stream	<<	"itimersepmaphore_"	<<	getpid()	;
				sem_name	=	sem_name_stream.str()	;

				semaphore	=	sem_open(sem_name.c_str(),	O_CREAT,	S_IRWXU	,	0);

				return	(0)	;
}

/**
@details
-#	If	the	timer	is	enabled
			-#	If	the	frame	time	is	valid
						-#	Set	the	system	itimer	to	expire	one	time	at	the	desired	frame	period.
			-#	Else	termiate	the	simulation	with	the	error	message	stating	that	the
						itimer	frame	period	is	invalid.
*/
int	Trick::ITimer::start(double	in_frame_time)	{

				struct	itimerval	ivalue;
				int	ret	;
				unsigned	long	long	frame_sec	;
				unsigned	int	frame_usec	;

				if	(	enabled	)	{

								//	clear	out	any	built	up	signals.
								while	(	(ret	=	sem_trywait(semaphore))	==	0	)	;

								frame_sec	=	(	unsigned	long	long	)in_frame_time	;
								frame_usec	=	(unsigned	int)((in_frame_time	-	frame_sec)	*	1000000)	;

								//	check	minimum	time	>	10ms
								if	(	frame_sec	==	0	&&	frame_usec	<	10000	)	{
												active	=	false	;
												return(0)	;
								}	else	if	(	frame_usec	<	2000	&&	frame_sec	>	0	)	{
												//	adjust	times	with	usec	<	2000	,	we'll	be	subtracting	2ms	from	the	frame_usec
												frame_sec		-=	1	;
												frame_usec	+=	1000000	;
								}
								frame_usec	-=	2000	;

								if	(	in_frame_time	>	0	)	{
												/*	Set	timer	interval	in	micro-seconds	*/
												ivalue.it_interval.tv_sec	=	0;
												ivalue.it_interval.tv_usec	=	0;
												ivalue.it_value.tv_sec	=	frame_sec;
												ivalue.it_value.tv_usec	=	(long)	frame_usec;

												setitimer(ITIMER_REAL,	&ivalue,	NULL);
												active	=	true	;
								}	else	{
												char	error_message[256];
												snprintf(error_message,	sizeof(error_message),	"itimer	frame_time	is	not	set\n"	)	;
												exec_terminate_with_return(-1,	__FILE__,	__LINE__	,	error_message);
								}
				}
				return	(0)	;

}

/*
@details
-#	Call	the	start	function
*/
int	Trick::ITimer::reset(double	in_frame_time)	{
				start(in_frame_time)	;
				return	(0)	;
}

/**
@details
-#	Set	the	system	itimer	with	the	expiriation	time	of	0.
*/
int	Trick::ITimer::stop()	{

				static	struct	itimerval	ivalue,	ovalue;

				/*	Turn	off	itimer	*/
				ivalue.it_interval.tv_usec	=	0;
				ivalue.it_interval.tv_sec	=	0;

				ivalue.it_value.tv_usec	=	0;
				ivalue.it_value.tv_sec	=	0;

				(void)	setitimer(ITIMER_REAL,	&ivalue,	&ovalue);

				active	=	false	;

				return	(0)	;

}

/**
@details
-#	If	the	timer	is	enabled
			-#	Call	sem_wait	to	decrement	the	semaphore.		sem_wait	will	sleep	and	wait
						for	the	semaphore	to	be	posted	from	the	signal	handler.
			-#	If	sem_wait	returns	an	error,	write	an	error	message
*/
int	Trick::ITimer::pause()	{

				int	sem_ret;

				if	(	enabled	and	active	)	{

								do	{
												sem_ret	=	sem_wait(semaphore);
												//TODO:	I'd	like	to	use	sem_timedwait
												//sem_ret	=	sem_timedwait(semaphore,	&timeout);
								}	while	((sem_ret	<	0	&&	errno	==	EINTR)	&&	(errno	!=	EDEADLK));

								if	(sem_ret	!=	0)	{
												//	Prepend	info	to	the	error	message	string	(given	by	errno)
												perror(	"ITimer	call	to	sem_wait()"	);
								}

				}

				return	(0)	;

}

/**
@details
-#	Unlink	the	semaphore
*/
int	Trick::ITimer::shutdown()	{
				sem_unlink(sem_name.c_str())	;
				return(0)	;
}

Trick::ITimer	*	get_itimer()	{
				return	(the_itimer)	;
}
