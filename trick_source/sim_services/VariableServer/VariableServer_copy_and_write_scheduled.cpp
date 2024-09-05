
#include	<iostream>
#include	<sys/time.h>

#include	"trick/VariableServer.hh"
#include	"trick/TrickConstant.hh"

int	Trick::VariableServer::copy_and_write_scheduled()	{

				long	long	next_call_tics	=	TRICK_MAX_LONG_LONG;

				pthread_mutex_lock(&map_mutex)	;
				for	(	auto	it	=	var_server_sessions.begin()	;	it	!=	var_server_sessions.end()	;	++it	)	{
								VariableServerSession	*		session	=	(*it).second	;
								session->copy_and_write_scheduled(copy_data_job->next_tics)	;
								if	(	session->get_next_tics()	<	next_call_tics	)	{
												next_call_tics	=	session->get_next_tics()	;
								}
				}
				pthread_mutex_unlock(&map_mutex)	;

				//reschedule	the	current	job.	TODO:	a	call	needs	to	be	created	to	do	this	the	OO	way
				if	(	copy_data_job	!=	NULL	)	{
								copy_data_job->next_tics	=	next_call_tics	;
				}

				return(0)	;
}
