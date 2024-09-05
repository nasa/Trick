
#include	"trick/VariableServer.hh"

int	Trick::VariableServer::shutdown()	{
				
				//	Shutdown	all	listen	threads
				listen_thread.cancel_thread()	;
				for	(auto&	listen_it	:	additional_listen_threads)	{
								listen_it.second->cancel_thread();
				}

				//	Shutdown	all	session	threads
				pthread_mutex_lock(&map_mutex)	;
				for	(auto&	it	:	var_server_threads)	{
								it.second->cancel_thread()	;
				}
				pthread_mutex_unlock(&map_mutex)	;

				return	0	;
}

