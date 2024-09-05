#include	"trick/MonteCarlo.hh"
#include	"trick/tc_proto.h"

Trick::MonteCarlo	*	the_mc	;

Trick::MonteCarlo::MonteCarlo()	:
				enabled(false),
				dry_run(false),
				localhost_as_remote(false),
				custom_slave_dispatch(false),
				timeout(120),
				max_tries(2),
				verbosity(MC_INFORMATIONAL),
				num_runs(0),
				actual_num_runs(0),
				num_results(0),
				slave_id(0),
				except_return(0)
{
				the_mc	=	this;

				slaves_head	=	NULL;

				char	hostname[_POSIX_HOST_NAME_MAX	+	1];
				gethostname(hostname,	_POSIX_HOST_NAME_MAX);
				machine_name	=	hostname;

				memset(&listen_device,	0,	sizeof(TCDevice))	;
				memset(&connection_device,	0,	sizeof(TCDevice))	;

				listen_device.port	=	0;
				connection_device.port	=	0;

				listen_device.disable_handshaking	=	TC_COMM_TRUE;
				connection_device.disable_handshaking	=	TC_COMM_TRUE;

				tc_error(&listen_device,	0);
				tc_error(&connection_device,	0);

				int	num_classes	=	0;
				class_map["monte_master_init"]	=	num_classes;
				class_to_queue[num_classes++]	=	&master_init_queue;

				class_map["monte_master_pre"]	=	num_classes;
				class_to_queue[num_classes++]	=	&master_pre_queue;

				class_map["monte_master_post"]	=	num_classes;
				class_to_queue[num_classes++]	=	&master_post_queue;

				class_map["monte_master_shutdown"]	=	num_classes;
				class_to_queue[num_classes++]	=	&master_shutdown_queue;

				class_map["monte_slave_init"]	=	num_classes;
				class_to_queue[num_classes++]	=	&slave_init_queue;

				class_map["monte_slave_pre"]	=	num_classes;
				class_to_queue[num_classes++]	=	&slave_pre_queue;

				class_map["monte_slave_post"]	=	num_classes;
				class_to_queue[num_classes++]	=	&slave_post_queue;

				class_map["monte_slave_shutdown"]	=	num_classes;
				class_to_queue[num_classes++]	=	&slave_shutdown_queue;

}

Trick::MonteCarlo::~MonteCarlo()	{
				/*	tc_error	allocates	memory	in	the	constructor	*/
				free(listen_device.error_handler)	;
				free(connection_device.error_handler)	;
				listen_device.error_handler	=	NULL	;
				connection_device.error_handler	=	NULL	;
}

const	char*	Trick::MonteRun::exit_status_string()	const	{
				return	MC_exit_status_string[exit_status];
}


