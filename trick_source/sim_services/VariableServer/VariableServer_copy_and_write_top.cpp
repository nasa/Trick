#include	"trick/exec_proto.h"
#include	"trick/VariableServer.hh"

int	Trick::VariableServer::copy_and_write_top()	{

				pthread_mutex_lock(&map_mutex)	;
				for	(	auto	it	=	var_server_sessions.begin()	;	it	!=	var_server_sessions.end()	;	++it	)	{
								(*it).second->copy_and_write_top(exec_get_frame_count())	;
				}
				pthread_mutex_unlock(&map_mutex)	;

				return	0	;
}
