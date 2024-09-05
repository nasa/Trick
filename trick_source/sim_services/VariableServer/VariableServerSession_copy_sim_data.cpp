
#include	<iostream>
#include	<string.h>

#include	"trick/VariableServerSession.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.h"


//	These	actually	do	the	copying

int	Trick::VariableServerSession::copy_sim_data()	{
				return	copy_sim_data(_session_variables,	true);
}

int	Trick::VariableServerSession::copy_sim_data(std::vector<VariableReference	*>&	given_vars,	bool	cyclical)	{

				if	(given_vars.size()	==	0)	{
								return	0;
				}

				if	(	pthread_mutex_trylock(&_copy_mutex)	==	0	)	{
								//	Get	the	simulation	time	we	start	this	copy
								_time	=	(double)exec_get_time_tics()	/	exec_get_time_tic_value()	;
								

								for	(auto	curr_var	:	given_vars	)	{
												curr_var->stageValue();
								}

								pthread_mutex_unlock(&_copy_mutex)	;
				}

				return	0;
}
