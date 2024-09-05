
#include	<iostream>
#include	"trick/VariableServer.hh"
#include	"trick/variable_server_sync_types.h"
#include	"trick/TrickConstant.hh"

int	Trick::VariableServer::get_next_sync_call_time()	{

				long	long	next_call_tics	=	TRICK_MAX_LONG_LONG	;

				pthread_mutex_lock(&map_mutex)	;
				for	(	auto	it	=	var_server_sessions.begin()	;	it	!=	var_server_sessions.end()	;	++it	)	{
								VariableServerSession	*	session	=	(*it).second	;
								if	(	session->get_next_tics()	<	next_call_tics	)	{
												next_call_tics	=	session->get_next_tics()	;
								}
				}
				pthread_mutex_unlock(&map_mutex)	;

				copy_data_job->next_tics	=	next_call_tics	;


				return(0)	;
}
