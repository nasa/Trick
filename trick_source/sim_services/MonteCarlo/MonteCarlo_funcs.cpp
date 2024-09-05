
#include	<sys/time.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/command_line_protos.h"
#include	"trick/tc_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/exec_proto.h"

extern	Trick::Executive	*	the_exec	;

void	Trick::MonteCarlo::set_enabled(bool	in_enabled)	{
				this->enabled	=	in_enabled;
}

bool	Trick::MonteCarlo::get_enabled()	{
				return	enabled;
}

void	Trick::MonteCarlo::set_dry_run(bool	in_dry_run)	{
				this->dry_run	=	in_dry_run;
}

bool	Trick::MonteCarlo::get_dry_run()	{
				return	dry_run;
}

bool	Trick::MonteCarlo::is_slave()	{
				if	(slave_id	==	0)	{
							return	false;
				}
				return	true;
}

bool	Trick::MonteCarlo::is_master()	{
				if	(slave_id	==	0)	{
							return	true;
				}
				return	false;
}

void	Trick::MonteCarlo::set_localhost_as_remote(bool	in_localhost_as_remote)	{
				this->localhost_as_remote	=	in_localhost_as_remote;
}

bool	Trick::MonteCarlo::get_localhost_as_remote()	{
				return	localhost_as_remote;
}

void	Trick::MonteCarlo::set_custom_slave_dispatch(bool	in_custom_slave_dispatch)	{
				this->custom_slave_dispatch	=	in_custom_slave_dispatch;
}

bool	Trick::MonteCarlo::get_custom_slave_dispatch()	{
				return	custom_slave_dispatch;
}

void	Trick::MonteCarlo::set_timeout(double	in_timeout)	{
				this->timeout	=	in_timeout;
}

double	Trick::MonteCarlo::get_timeout()	{
				return	timeout;
}

void	Trick::MonteCarlo::set_max_tries(unsigned	int	in_max_tries)	{
				this->max_tries	=	in_max_tries;
}

unsigned	int	Trick::MonteCarlo::get_max_tries()	{
				return	max_tries;
}

void	Trick::MonteCarlo::set_user_cmd_string(std::string	in_user_cmd_string)	{
				this->user_cmd_string	=	in_user_cmd_string;
}

std::string	Trick::MonteCarlo::get_user_cmd_string()	{
				return	user_cmd_string;
}

const	char*	Trick::MonteCarlo::get_user_cmd_string_c_str()	{
				return	user_cmd_string.c_str();
}

void	Trick::MonteCarlo::set_custom_pre_text(std::string	in_custom_pre_text)	{
				this->custom_pre_text	=	in_custom_pre_text;
}

std::string	Trick::MonteCarlo::get_custom_pre_text()	{
				return	custom_pre_text;
}

const	char*	Trick::MonteCarlo::get_custom_pre_text_c_str()	{
				return	custom_pre_text.c_str();
}

void	Trick::MonteCarlo::set_custom_post_text(std::string	in_custom_post_text)	{
				this->custom_post_text	=	in_custom_post_text;
}

std::string	Trick::MonteCarlo::get_custom_post_text()	{
				return	custom_post_text;
}

const	char*	Trick::MonteCarlo::get_custom_post_text_c_str()	{
				return	custom_post_text.c_str();
}

void	Trick::MonteCarlo::set_verbosity(Trick::MonteCarlo::Verbosity	in_verbosity)	{
				this->verbosity	=	in_verbosity;
}

Trick::MonteCarlo::Verbosity	Trick::MonteCarlo::get_verbosity()	{
				return	verbosity;
}

void	Trick::MonteCarlo::set_num_runs(unsigned	int	in_num_runs)	{
				while	(this->num_runs	<	in_num_runs)	{
								runs.push_back(new	Trick::MonteRun(this->num_runs++));
				}
				while	(	(this->num_runs	>	in_num_runs)	&&	!runs.empty()	)	{
								delete	runs.back();
								runs.pop_back();
								--this->num_runs;
				}
				update_actual_num_runs();
}

unsigned	int	Trick::MonteCarlo::get_num_runs()	{
				return	num_runs;
}

unsigned	int	Trick::MonteCarlo::get_num_results()	{
				return	num_results;
}

unsigned	int	Trick::MonteCarlo::get_slave_id()	{
				return	slave_id;
}

void	Trick::MonteCarlo::add_range(unsigned	int	start,	unsigned	int	end)	{
				run_ranges.push_back(new	MonteRange(start,	end));
				update_actual_num_runs();
}

/**
	*	@par	Detailed	Design:
	*	If	there	are	no	ranges	specified,	all	runs	are	in	range.
	*/
bool	Trick::MonteCarlo::in_range(Trick::MonteRun	*in_run)	{
					if	(run_ranges.size()	==	0)	{
									return	true;
					}
					for	(std::vector<Trick::MonteRange>::size_type	i	=	0;	i	<	run_ranges.size();	++i)	{
									if	(in_run->id	>=	run_ranges[i]->get_start()	&&	in_run->id	<=	run_ranges[i]->get_end())	{
													return	true;
									}
					}
					return	false;
}

void	Trick::MonteCarlo::get_ranges(std::vector<MonteRange	*>	&ranges)	{
				ranges	=	run_ranges;
}

void	Trick::MonteCarlo::add_variable(Trick::MonteVar	*variable)	{
				for	(std::vector<Trick::MonteVar	*>::const_iterator	i	=	variables.begin();	i	!=	variables.end();	++i)	{
								if	(	(*i)->name.compare(variable->name)	==	0	)	{
												message_publish(MSG_WARNING,	"Monte	WARNING:	Cannot	add	new	MonteVar	\"%s\",	variable	of	that	name	already	exists.\n",
																				variable->name.c_str()	);
												return;
								}
				}
				variables.push_back(variable);
}

/**
	*	@par	Detailed	Design:
	*	Get	a	pointer	to	a	MonteVar	by	name.	Note	this	is	used	in	conjunction	with
	*	%factory	so	that	swig	produces	methods	returning	all	derived	types
	*/
Trick::MonteVar	*	Trick::MonteCarlo::get_variable(std::string	variable_name)	{

				for	(std::vector<Trick::MonteVar	*>::const_iterator	i	=	variables.begin();	i	!=	variables.end();	++i)	{
								if	(	(*i)	and	(*i)->name.compare(variable_name)	==	0	)	{
												return	(*i);
								}
				}
				return	(NULL);
}

const	std::vector<Trick::MonteVar*>&	Trick::MonteCarlo::get_variables()	{
				return	variables;
}

void	Trick::MonteCarlo::add_slave(std::string	in_machine_name)	{
				add_slave(new	MonteSlave(in_machine_name));
}

/**
	*	@par	Detailed	Design:
	*	The	first	slave	added	is	assigned	an	id	of	one.	Additional	slaves	receive	ids	of	one	plus	the	id	of	the	last	slave	on
	*	the	list,	ensuring	that	every	slave	has	a	unique	id.	This	function	has	no	effect	if	the	slave	has	been	previously	added.
	*/
void	Trick::MonteCarlo::add_slave(Trick::MonteSlave	*in_slave)	{
				if	(get_slave_index(in_slave->id)	==	-1)	{
								if	(slaves.empty())	{
												in_slave->id	=	1;
								}	else	{
												in_slave->id	=	slaves.back()->id	+	1;
								}
								slaves.push_back(in_slave);
								sync_slaves_head();
				}
}

/**
	*	@par	Detailed	Design:
	*	This	function	has	an	effect	only	if	the	slave	exists	and	is	in	the	MC_STOPPING,	MC_UNRESPONSIVE_STOPPING,	or	MC_STOPPED	state.
	*/
void	Trick::MonteCarlo::start_slave(unsigned	int	id)	{
				if	(MonteSlave	*slave	=	get_slave(id))	{
								if	(verbosity	>=	MC_ALL)	{
												message_publish(MSG_INFO,	"Monte	[Master]	Starting	%s:%d.\n",	slave->machine_name.c_str(),	slave->id)	;
								}
								if	(slave->state	==	Trick::MonteSlave::MC_STOPPING)	{
												slave->state	=	Trick::MonteSlave::MC_RUNNING;
								}	else	if	(slave->state	==	Trick::MonteSlave::MC_UNRESPONSIVE_STOPPING)	{
												slave->state	=	Trick::MonteSlave::MC_UNRESPONSIVE_RUNNING;
								}	else	if	(slave->state	==	Trick::MonteSlave::MC_STOPPED)	{
												slave->state	=	Trick::MonteSlave::MC_READY;
								}
				}
}

/**
	*	@par	Detailed	Design:
	*	This	function	has	an	effect	only	if	the	slave	exists	and	is	in	the	MC_READY,	MC_RUNNING,	or	MC_UNRESPONSIVE_RUNNING	state.
	*/
void	Trick::MonteCarlo::stop_slave(unsigned	int	id)	{
				if	(MonteSlave	*slave	=	get_slave(id))	{
								if	(verbosity	>=	MC_ALL)	{
												message_publish(MSG_INFO,	"Monte	[Master]	Stopping	%s:%d.\n",	slave->machine_name.c_str(),	slave->id)	;
								}
								if	(slave->state	==	Trick::MonteSlave::MC_READY)	{
												slave->state	=	Trick::MonteSlave::MC_STOPPED;
								}	else	if	(slave->state	==	Trick::MonteSlave::MC_RUNNING)	{
												slave->state	=	Trick::MonteSlave::MC_STOPPING;
								}	else	if	(slave->state	==	Trick::MonteSlave::MC_UNRESPONSIVE_RUNNING)	{
												slave->state	=	Trick::MonteSlave::MC_UNRESPONSIVE_STOPPING;
								}
				}
}

/**
	*	@par	Detailed	Design:
	*	This	function	will	only	disable	the	slave	when	called	from	the	input	file	at	initialization.
	*/
void	Trick::MonteCarlo::disable_slave(std::string	name,	bool	disabled){
				for	(std::vector<MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								if	(equals_ignore_case(slaves[i]->machine_name,	name))	{
												if	(disabled)	{
																slaves[i]->state	=	Trick::MonteSlave::MC_STOPPED;
												}	else	{
																slaves[i]->state	=	Trick::MonteSlave::MC_UNINITIALIZED;
												}
												return;
								}
				}
}

int	Trick::MonteCarlo::process_sim_args()	{
				int	argc	=	command_line_args_get_argc()	;
				char	**argv	=	command_line_args_get_argv()	;

				if	(argc	>	2)	{
								for	(int	i	=	2;	i	<	argc;	++i)	{
												if	(!strncmp("--monte_host",	argv[i],	12))	{
																connection_device.hostname	=	strdup(argv[++i]);
												}	else	if	(!strncmp("--monte_sync_port",	argv[i],	17))	{
																sscanf(argv[++i],	"%d",	&master_port);
												}	else	if	(!strncmp("--monte_client_id",	argv[i],	12))	{
																sscanf(argv[++i],	"%d",	&slave_id);
												}
								}
				}
				return	0;
}

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::shutdown()	{
				/**	<ul><li>	If	this	is	a	slave,	run	the	shutdown	jobs.	*/
				if	(enabled	&&	is_slave())	{
								connection_device.port	=	master_port;
								if	(tc_connect(&connection_device)	==	TC_SUCCESS)	{
												int	exit_status	=	the_exec->get_except_return()	?	MonteRun::MC_RUN_FAILED	:	MonteRun::MC_RUN_COMPLETE;
												if	(verbosity	>=	MC_ALL)	{
																message_publish(MSG_INFO,	"Monte	[%s:%d]	Sending	run	exit	status	to	master:	%d\n",
																																machine_name.c_str(),	slave_id,	exit_status)	;
												}
												int	id	=	htonl(slave_id);
												tc_write(&connection_device,	(char*)&id,	(int)sizeof(id));
												exit_status	=	htonl(exit_status);
												tc_write(&connection_device,	(char*)&exit_status,	(int)sizeof(exit_status));
												run_queue(&slave_post_queue,	"in	slave_post	queue");
												tc_disconnect(&connection_device);
								}	else	{
												if	(verbosity	>=	MC_ERROR)
																message_publish(
																		MSG_ERROR,
																		"Monte	[%s:%d]	Failed	to	connect	to	master.\n",
																		machine_name.c_str(),	slave_id);
								}
				}
				return	0;
}

void	Trick::MonteCarlo::handle_retry(MonteSlave&	slave,	MonteRun::ExitStatus	exit_status)	{
				if	(max_tries	<=	0	||	slave.current_run->num_tries	<	max_tries)	{
								//	Add	the	run	to	the	retry	queue.
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[Master]	Queueing	run	%d	for	retry.\n",	slave.current_run->id)	;
								}
								runs.push_back(slave.current_run);
				}	else	{
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[Master]	Run	%d	has	reached	its	maximum	allowed	tries	and	has	been	skipped.\n",
																												slave.current_run->id)	;
								}
								resolve_run(slave,	exit_status);
				}
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::resolve_run(MonteSlave&	slave,	MonteRun::ExitStatus	exit_status)	{
				if	(exit_status	==	MonteRun::MC_RUN_FAILED)	{
								failed_runs.push_back(slave.current_run);
				}
				else	if	(exit_status	!=	MonteRun::MC_RUN_COMPLETE)	{
								error_runs.push_back(slave.current_run);
				}

				/**	<li>	Update	the	bookkeeping.	*/
				struct	timeval	time_val;
				gettimeofday(&time_val,	NULL);
				slave.current_run->end_time	=	time_val.tv_sec	+	(double)time_val.tv_usec	/	1000000;
				slave.current_run->exit_status	=	exit_status;

				++slave.num_results;
				slave.cpu_time	+=	slave.current_run->end_time	-	slave.current_run->start_time;

				++num_results;

				if	(verbosity	>=	MC_ALL)	{
								message_publish(MSG_INFO,	"Monte	[Master]	Run	%d	has	been	resolved	as:	%d.\n",slave.current_run->id,	exit_status)	;
				}
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::check_timeouts()	{
				struct	timeval	time_val;
				gettimeofday(&time_val,	NULL);
				/**	<ul><li>	For	every	slave:	*/
				for	(std::vector<Trick::MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								/**	<ul><li>	If	the	slave	has	timed	out:	*/
								if	((slaves[i]->state	==	MonteSlave::MC_RUNNING	||	slaves[i]->state	==	MonteSlave::MC_STOPPING)	&&
										(time_val.tv_sec	+	(double)time_val.tv_usec	/	1000000	-	slaves[i]->current_run->start_time)	*
										slaves[i]->multiplier	>	timeout)	{
												/**
													*	<ul><li>	This	run	might	have	been	redispatched	due	to	a	previous	timeout	for	which	the	slave	actually	returned
													*	data	later.	Only	process	this	timeout	if	the	run	hasn't	been	resolved	yet.
													*/
												if	(slaves[i]->current_run->exit_status	==	MonteRun::MC_RUN_INCOMPLETE)	{
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(MSG_ERROR,	"Monte	[Master]	%s:%d	has	not	responded	for	run	%d.\n",
																																				slaves[i]->machine_name.c_str(),	slaves[i]->id,	slaves[i]->current_run->id)	;
																}
																handle_retry(*slaves[i],	MonteRun::MC_RUN_TIMED_OUT);
												}
												/**	</ul><li>	Update	the	slave's	state.	*/
												slaves[i]->state	=	slaves[i]->state	==	MonteSlave::MC_RUNNING	?
															MonteSlave::MC_UNRESPONSIVE_RUNNING	:	MonteSlave::MC_UNRESPONSIVE_STOPPING;
								}
				}
}

Trick::MonteSlave	*	Trick::MonteCarlo::get_ready_slave()	{
				for	(std::vector<Trick::MonteSlave>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								if	(slaves[i]->state	==	Trick::MonteSlave::MC_READY)	{
												return	slaves[i];
								}
				}
				return	NULL;
}

Trick::MonteSlave*	Trick::MonteCarlo::get_slave(unsigned	int	id)	{
				int	i	=	get_slave_index(id);
				if	(i	>	-1)	{
								return	slaves[i];
				}
				return	NULL;
}

int	Trick::MonteCarlo::get_slave_index(unsigned	int	id)	{
				for	(std::vector<std::string>::size_type	i	=	0;	i	<	slaves.size();	i++)	{
								if	(slaves[i]->id	==	id)	{
												return	i;
								}
				}
				return	-1;
}

/**	@par	Detailed	Design:	*/
Trick::MonteRun	*Trick::MonteCarlo::get_next_dispatch()	{
				/**	<ul><li>	While	there	are	remaining	runs:	*/
				while	(!runs.empty())	{
								MonteRun	*curr_run	=	runs.front();
								/**	<li>	If	it	is	in	range,	return	it.	</ul>*/
								if	(in_range(curr_run))	{
												return	curr_run;
								/**	<li>	Otherwise,	run	the	pre	run	jobs	and	dequeue	it.	*/
								}	else	{
												if	(verbosity	>=	MC_ALL)	{
																message_publish(MSG_WARNING,	"Monte	[Master]	Run	%d	is	out	of	range	and	has	been	skipped.\n",	curr_run->id)	;
												}
												prepare_run(curr_run);
								}
				}
				return	NULL;
}

/**	@par	Detailed	Design:	*/
int		Trick::MonteCarlo::prepare_run(MonteRun	*curr_run)	{
				current_run	=	curr_run->id;
				/**	<ul><li>	If	this	run	has	never	been	dispatched:	*/
				if	(curr_run->num_tries	==	0)	{
								/**	<ul><li>	Run	the	pre	run	jobs.	*/
								run_queue(&master_pre_queue,	"in	master_pre	queue")	;
								/**	<li>	Add	the	variables	to	the	curr_run	and	check	for	end	of	file	and	value	generation	failures.	*/
								for	(std::vector<std::string>::size_type	i	=	0;	i	<	variables.size();	++i)	{
												curr_run->variables.push_back(variables[i]->get_next_value());
												if	(curr_run->variables.back()	==	"EOF")	{
																if	(verbosity	>=	MC_ALL)	{
																				message_publish(MSG_WARNING,	"Monte	[Master]	File	variable	'%s'	reached	end-of-file.	Reducing	number	of	runs	to	%d.\n",
																																				variables[i]->name.c_str(),	curr_run->id)	;
																}
																set_num_runs(curr_run->id);
																return	-1;
												}
								}
								/**	<li>	Create	the	data	file	</ul>*/
								fprintf(run_data_file,	"%05u\t",	curr_run->id);
								for	(std::vector<std::string>::size_type	i	=	0;	i	<	variables.size();	++i)	{
												if	(i>0)	{
																fprintf(run_data_file,	"\t");
												}
												fprintf(run_data_file,	"%s",	variables[i]->value.c_str());
								}
								fprintf(run_data_file,	"\n");
				}
				/**	<li>	Dequeue	the	run.	*/
				dequeue_run(curr_run);
				return	0;
}

void	Trick::MonteCarlo::dequeue_run(MonteRun	*curr_run)	{
				for	(std::deque<MonteRun	*>::size_type	i	=	0;	i	<	runs.size();	++i)	{
								if	(curr_run	==	runs[i])	{
												runs.erase(runs.begin()	+	i);
								}
				}
}

/**
	*	@par	Detailed	Design:
	*	Since	the	Variable	Server	is	unable	to	access	elements	within	the	standard	template	objects,	the	data	contained	in
	*	#slaves	must	be	duplicated	here	for	applications	that	wish	to	query	the	Variable	Server	for	information	on	slaves,
	*	such	as	@ref	MonteMonitor	and	@ref	TrickView.	This	function	ensures	that	the	slave	pointers	within	#slaves_head	point
	*	to	the	same	memory	addresses	that	the	slave	pointers	in	#slaves	do.
	*/
void	Trick::MonteCarlo::sync_slaves_head()	{
				num_slaves	=	slaves.size();
				if	(!slaves_head)	{
								slaves_head	=	(MonteSlave**)TMM_declare_var_s("Trick::MonteSlave*[1]");
				}	else	{
								slaves_head	=	(MonteSlave**)TMM_resize_array_1d_a(slaves_head,	num_slaves);
				}
				for	(std::vector<MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								slaves_head[i]	=	slaves[i];
				}
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::update_actual_num_runs()	{
				/**	<ul><li>	Start	with	the	number	of	results	obtained	so	far.	*/
				actual_num_runs	=	num_results;
				/**	<li>	Add	the	number	of	runs	still	on	the	queue	that	are	in	range.	*/
				for	(std::vector<MonteRun	*>::size_type	i	=	0;	i	<	runs.size();	++i)	{
								if	(in_range(runs[i]))	{
												++actual_num_runs;
								}
				}
				/**	<li>	Add	one	for	every	currently	dispatched	run.	*/
				for	(std::vector<MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								if	(slaves[i]->state	==	MonteSlave::MC_RUNNING	||	slaves[i]->state	==	MonteSlave::MC_STOPPING)	{
												++actual_num_runs;
								}
				}
}

bool	Trick::MonteCarlo::equals_ignore_case(std::string	string1,	std::string	string2)	{
				if	(string1.length()	!=	string2.length())	{
								return	false;
				}
				for	(size_t	i	=	0;	i	<	string1.length();	++i)	{
								if	(tolower(string1[i])	!=	tolower(string2[i]))	{
												return	false;
								}
				}
				return	true;
}

unsigned	int	Trick::MonteCarlo::get_current_run()	{
				return	(current_run)	;
}

void	Trick::MonteCarlo::set_current_run(int	run_num)	{
				current_run	=	run_num	;
}

void	Trick::MonteCarlo::set_listen_device_port(int	port_number)	{
								listen_device.port	=	port_number	;
}

void	Trick::MonteCarlo::set_connection_device_port(int	port_number)	{
				//	This	port	is	passed	to	slave	as	an	argument,	do	not	override
				if	(is_master())	{
								connection_device.port	=	port_number	;
				}
}

int	Trick::MonteCarlo::get_listen_device_port()	{
				return	listen_device.port	;
}

int	Trick::MonteCarlo::get_connection_device_port()	{
				return	connection_device.port	;
}

int	Trick::MonteCarlo::instrument_job_before(	Trick::JobData*	instrument_job)	{

				int	count	=	0	;

				count	+=	master_init_queue.instrument_before(instrument_job)	;
				count	+=	master_pre_queue.instrument_before(instrument_job)	;
				count	+=	master_post_queue.instrument_before(instrument_job)	;
				count	+=	master_shutdown_queue.instrument_before(instrument_job)	;
				count	+=	slave_init_queue.instrument_before(instrument_job)	;
				count	+=	slave_pre_queue.instrument_before(instrument_job)	;
				count	+=	slave_post_queue.instrument_before(instrument_job)	;
				count	+=	slave_shutdown_queue.instrument_before(instrument_job)	;

				/**	@li	Return	how	many	insertions	were	done.	*/
				return(count)	;
}

int	Trick::MonteCarlo::instrument_job_after(	Trick::JobData*	instrument_job)	{

				unsigned	int	count	;

				count	=	0	;

				count	+=	master_init_queue.instrument_after(instrument_job)	;
				count	+=	master_pre_queue.instrument_after(instrument_job)	;
				count	+=	master_post_queue.instrument_after(instrument_job)	;
				count	+=	master_shutdown_queue.instrument_after(instrument_job)	;
				count	+=	slave_init_queue.instrument_after(instrument_job)	;
				count	+=	slave_pre_queue.instrument_after(instrument_job)	;
				count	+=	slave_post_queue.instrument_after(instrument_job)	;
				count	+=	slave_shutdown_queue.instrument_after(instrument_job)	;

				/**	@li	Return	how	many	insertions	were	done.	*/
				return(count)	;

}


int	Trick::MonteCarlo::instrument_job_remove(	std::string	in_job)	{

				master_init_queue.instrument_remove(in_job)	;
				master_pre_queue.instrument_remove(in_job)	;
				master_post_queue.instrument_remove(in_job)	;
				master_shutdown_queue.instrument_remove(in_job)	;
				slave_init_queue.instrument_remove(in_job)	;
				slave_pre_queue.instrument_remove(in_job)	;
				slave_post_queue.instrument_remove(in_job)	;
				slave_shutdown_queue.instrument_remove(in_job)	;

				return(0)	;
}

int	Trick::MonteCarlo::write_s_job_execution(FILE	*fp)	{
				if	(fp	==	NULL)	{
								return(0);
				}

				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"MonteCarlo	Loop:\n\n")	;
				master_init_queue.write_sched_queue(fp)	;
				master_pre_queue.write_sched_queue(fp)	;
				master_post_queue.write_sched_queue(fp)	;
				master_shutdown_queue.write_sched_queue(fp)	;
				slave_init_queue.write_sched_queue(fp)	;
				slave_pre_queue.write_sched_queue(fp)	;
				slave_post_queue.write_sched_queue(fp)	;
				slave_shutdown_queue.write_sched_queue(fp)	;

				return	0;
}

int	Trick::MonteCarlo::write(char*	data,	int	size)	{
				return	tc_write(&connection_device,	data,	size);
}

int	Trick::MonteCarlo::read(char*	data,	int	size)	{
				return	tc_read(&connection_device,	data,	size);
}
