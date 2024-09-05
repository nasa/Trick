
#include	<iostream>
#include	<iomanip>
#include	<sstream>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<sys/syscall.h>
#include	<sys/stat.h>
#include	<string.h>

#include	"trick/CheckPointRestart.hh"
#include	"trick/MemoryManager.hh"
#include	"trick/SimObject.hh"
#include	"trick/Executive.hh"
#include	"trick/exec_proto.hh"
#include	"trick/exec_proto.h"
#include	"trick/command_line_protos.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/TrickConstant.hh"

Trick::CheckPointRestart	*	the_cpr	;

Trick::CheckPointRestart::CheckPointRestart()	{

				int	num_classes	=	0	;

				pre_init_checkpoint	=	false	;
				post_init_checkpoint	=	false	;
				end_checkpoint	=	false	;
				safestore_enabled	=	false	;
				cpu_num	=	-1	;
				safestore_time	=	TRICK_MAX_LONG_LONG	;
				load_checkpoint_file_name.clear()	;

				write_checkpoint_job	=	NULL	;
				safestore_checkpoint_job	=	NULL	;

				class_map["checkpoint"]	=	num_classes	;
				class_to_queue[num_classes++]	=	&checkpoint_queue	;

				class_map["post_checkpoint"]	=	num_classes	;
				class_to_queue[num_classes++]	=	&post_checkpoint_queue	;

				class_map["preload_checkpoint"]	=	num_classes	;
				class_to_queue[num_classes++]	=	&preload_checkpoint_queue	;

				class_map["restart"]	=	num_classes	;
				class_to_queue[num_classes++]	=	&restart_queue	;

				the_cpr	=	this	;
}

int	Trick::CheckPointRestart::set_pre_init_checkpoint(bool	yes_no)	{
				pre_init_checkpoint	=	yes_no	;
				return(0)	;
}

int	Trick::CheckPointRestart::set_post_init_checkpoint(bool	yes_no)	{
				post_init_checkpoint	=	yes_no	;
				return(0)	;
}

int	Trick::CheckPointRestart::set_end_checkpoint(bool	yes_no)	{
				end_checkpoint	=	yes_no	;
				return(0)	;
}

int	Trick::CheckPointRestart::set_safestore_enabled(bool	yes_no)	{
				safestore_enabled	=	yes_no	;
				return(0)	;
}

int	Trick::CheckPointRestart::set_cpu_num(int	in_cpu_num)	{
				if	(	in_cpu_num	<=	0	)	{
								cpu_num	=	-1	;
				}	else	{
								cpu_num	=	in_cpu_num	;
				}
				return(0)	;
}


const	char	*	Trick::CheckPointRestart::get_output_file()	{
				return	output_file.c_str()	;
}

const	char	*	Trick::CheckPointRestart::get_load_file()	{
				return	load_checkpoint_file_name.c_str()	;
}

int	Trick::CheckPointRestart::find_write_checkpoint_jobs(std::string	sim_object_name)	{

				write_checkpoint_job	=	exec_get_job(std::string(sim_object_name	+	".write_checkpoint").c_str())	;
				if	(	write_checkpoint_job	==	NULL	)	{
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	"CheckPointRestart	could	not	find	write_checkpoint	job"	)	;
				}	else	{
								write_checkpoint_job->next_tics	=	TRICK_MAX_LONG_LONG	;
				}

				safestore_checkpoint_job	=	exec_get_job(std::string(sim_object_name	+	".safestore_checkpoint").c_str())	;
				if	(	safestore_checkpoint_job	==	NULL	)	{
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	"CheckPointRestart	could	not	find	safestore_checkpoint	job"	)	;
				}	else	{
								safestore_checkpoint_job->next_tics	=	TRICK_MAX_LONG_LONG	;
				}

				return(0)	;
}

int	Trick::CheckPointRestart::checkpoint(double	in_time)	{

				long	long	curr_time	=	exec_get_time_tics()	;
				long	long	new_time	;

				new_time	=	(long	long)(in_time	*	exec_get_time_tic_value())	;

				if	(new_time	>=	curr_time	)	{
								checkpoint_times.push(new_time)	;
								if	(	new_time	<	write_checkpoint_job->next_tics	)	{
												write_checkpoint_job->next_tics	=	new_time	;
								}
								//std::cout	<<	"\033[33mSET	CHECKPOINT	TIME	"	<<	in_time	<<	"	"	<<	new_time	<<	"\033[0m"	<<	std::endl	;
				}	else	{
								message_publish(MSG_ERROR,	"Checkpoint	time	specified	in	the	past.	specified	%f,	current_time	%f\n",
									in_time	,	exec_get_sim_time())	;
				}

				return(0)	;
}

int	Trick::CheckPointRestart::set_safestore_time(double	in_time)	{

				long	long	software_frame_tics	;

				safestore_period	=	(long	long)(in_time	*	exec_get_time_tic_value())	;
				software_frame_tics	=	exec_get_software_frame_tics()	;

				if	(	safestore_period	%	software_frame_tics	)	{
								safestore_period	=	((safestore_time	/	software_frame_tics)	+	1	)	*	software_frame_tics	;
				}
				safestore_time	=	safestore_period	;

				if	(	safestore_checkpoint_job	!=	NULL	)	{
								safestore_checkpoint_job->next_tics	=	safestore_time	;
				}

				return(0)	;
}

int	Trick::CheckPointRestart::checkpoint(std::string	file_name,	bool	print_status,	std::string	obj_list_str	)	{

				//	first,	empty	the	sim	obj	list	to	make	sure	there	is	nothing	left	from	last	time
				obj_list.clear();

				if	(obj_list_str.compare(""))	{
								const	char	*	tok	=	strtok((char*)obj_list_str.c_str(),	",");
								while	(tok	!=	NULL)	{
												obj_list.push_back(tok);
												tok	=	strtok(NULL,	",");
								}
				}

				do_checkpoint(file_name,	print_status)	;

				return(0)	;
}

int	Trick::CheckPointRestart::do_checkpoint(std::string	file_name,	bool	print_status)	{

				JobData	*	curr_job	;
				pid_t	pid;

				if	(	!	file_name.compare("")	)	{
								std::stringstream	file_name_stream	;
								file_name_stream	<<	"chkpnt_"	<<	std::fixed	<<	std::setprecision(6)	<<	exec_get_sim_time()	;
								file_name	=	file_name_stream.str()	;
				}
				output_file	=	std::string(command_line_args_get_output_dir())	+	"/"	+	file_name	;

				checkpoint_queue.reset_curr_index()	;
				while	(	(curr_job	=	checkpoint_queue.get_next_job())	!=	NULL	)	{
								curr_job->parent_object->call_function(curr_job)	;
				}

				if	(	cpu_num	!=	-1	)	{
				//	if	the	user	specified	a	cpu	number	for	the	checkpoint,	fork	a	process	to	write	the	checkpoint
								if	((pid	=	fork())	==	0)	{
#if	__linux
												if	(	cpu_num	>=	0	)	{
																unsigned	long	mask;
																mask	=	1	<<	cpu_num	;
																syscall((long)	__NR_sched_setaffinity,	0,	sizeof(mask),	&mask);
												}
#endif
#if	__APPLE__
												if	(	cpu_num	>=	0	)	{
												}
#endif
												if	(obj_list.empty())	{
																trick_MM->write_checkpoint(output_file.c_str())	;
												}	else	{
																trick_MM->write_checkpoint(output_file.c_str(),	obj_list);
												}
												_Exit(0)	;
								}
				}
				else	{
				//	no	fork
								if	(obj_list.empty())	{
												trick_MM->write_checkpoint(output_file.c_str())	;
								}	else	{
												trick_MM->write_checkpoint(output_file.c_str(),	obj_list);
								}
				}

				post_checkpoint_queue.reset_curr_index()	;
				while	(	(curr_job	=	post_checkpoint_queue.get_next_job())	!=	NULL	)	{
								curr_job->parent_object->call_function(curr_job)	;
				}

				if	(	print_status	)	{
								message_publish(MSG_INFO,	"Dumped	ASCII	Checkpoint	%s.\n",	file_name.c_str())	;
				}

				return	0	;
}

int	Trick::CheckPointRestart::write_checkpoint()	{

				long	long	curr_time	=	exec_get_time_tics()	;

				//	checkpoint	time	is	set	in	a	read	event	that	occurs	at	top	of	frame
				if	(	curr_time	==	checkpoint_times.top()	)	{

								//	remove	all	times	at	the	top	of	the	queue	that	match	the	current	time.
								while	(	!checkpoint_times.empty()	and	(checkpoint_times.top()	==	curr_time)	)	{
												checkpoint_times.pop()	;
								}

								if	(	!checkpoint_times.empty()	)	{
												write_checkpoint_job->next_tics	=	checkpoint_times.top()	;
								}	else	{
												write_checkpoint_job->next_tics	=	TRICK_MAX_LONG_LONG	;
								}

								double	sim_time	=	exec_get_sim_time()	;
								std::stringstream	chk_name_stream	;

								chk_name_stream	<<	"chkpnt_"	<<	std::fixed	<<	std::setprecision(6)	<<	sim_time	;

								checkpoint(	chk_name_stream.str()	);

				}

				return(0)	;
}

int	Trick::CheckPointRestart::write_pre_init_checkpoint()	{
				if	(	pre_init_checkpoint	)	{
								checkpoint(std::string("chkpnt_pre_init"))	;
				}
				return	0		;
}

int	Trick::CheckPointRestart::write_post_init_checkpoint()	{
				if	(	post_init_checkpoint	)	{
								checkpoint(std::string("chkpnt_post_init"))	;
				}
				return	0		;
}

int	Trick::CheckPointRestart::write_end_checkpoint()	{
				if	(	end_checkpoint	)	{
								checkpoint(std::string("chkpnt_end"))	;
				}
				return	0		;
}

int	Trick::CheckPointRestart::safestore_checkpoint()	{

				if	(	safestore_enabled)	{
								checkpoint(std::string("chkpnt_safestore"),	false)	;
								safestore_time	+=	safestore_period	;
				}

				if	(	safestore_checkpoint_job	!=	NULL	)	{
								safestore_checkpoint_job->next_tics	=	safestore_time	;
				}

				return(0)	;
}

void	Trick::CheckPointRestart::load_checkpoint(std::string	file_name)	{
				load_checkpoint_file_name	=	file_name	;
}

void	Trick::CheckPointRestart::load_checkpoint(std::string	file_name,	bool	stls_on)	{
				trick_MM->set_restore_stls_default(stls_on);
				load_checkpoint(file_name);
}

int	Trick::CheckPointRestart::load_checkpoint_job()	{

				JobData	*	curr_job	;
				struct	stat	temp_buf	;

				if	(	!	load_checkpoint_file_name.empty()	)	{

								if	(	stat(	load_checkpoint_file_name.c_str()	,	&temp_buf)	==	0	)	{
												preload_checkpoint_queue.reset_curr_index()	;
												while	(	(curr_job	=	preload_checkpoint_queue.get_next_job())	!=	NULL	)	{
																curr_job->call()	;
												}

												//	clear	the	queues!		They	will	be	rebuilt	when	the	executive	calls	its	restart	job.
												checkpoint_queue.clear()	;
												post_checkpoint_queue.clear()	;
												preload_checkpoint_queue.clear()	;
												restart_queue.clear()	;

												message_publish(MSG_INFO,	"Load	checkpoint	file	%s.\n",	load_checkpoint_file_name.c_str())	;
												trick_MM->init_from_checkpoint(load_checkpoint_file_name.c_str())	;

												message_publish(MSG_INFO,	"Finished	loading	checkpoint	file.		Calling	restart	jobs.\n")	;

												//	bootstrap	the	sim_objects	back	into	the	executive!
												//	TODO:	MAKE	AN	exec_restart()	CALL	THAT	DOES	NOT	REQUIRE	WE	USE	the_exec.
												the_exec->restart()	;

												//	the	restart	queue	will	be	rebuilt	by	the	executive.
												restart_queue.reset_curr_index()	;
												while	(	(curr_job	=	restart_queue.get_next_job())	!=	NULL	)	{
																curr_job->call()	;
												}
								}	else	{
												message_publish(MSG_INFO,	"Could	not	find	checkpoint	file	%s.\n",	load_checkpoint_file_name.c_str())	;
								}
								load_checkpoint_file_name.clear()	;
				}

				return(0)	;
}

int	Trick::CheckPointRestart::write_s_job_execution(FILE	*fp)	{

				if	(	fp	==	NULL	)	{
								return(0)	;
				}

				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"CheckPointRestart	:\n\n")	;
				checkpoint_queue.write_non_sched_queue(fp)	;
				post_checkpoint_queue.write_non_sched_queue(fp)	;
				preload_checkpoint_queue.write_non_sched_queue(fp)	;
				restart_queue.write_non_sched_queue(fp)	;
				return	0	;
}

int	Trick::CheckPointRestart::instrument_job_before(	Trick::JobData	*	instrument_job	)	{

				int	count	=	0	;

				/**	@par	Detailed	Design:	*/

				/**	@li	if	target_job	specified,	instrument	job	will	only	be	inserted	before	it
								note:	when	more	than	one	target	job	with	same	name,	this	will	insert	instrument	job	for	1st	occurrence	of	target_job	in	each	queue
					*/
				count	+=	checkpoint_queue.instrument_before(instrument_job)	;
				count	+=	preload_checkpoint_queue.instrument_before(instrument_job)	;
				count	+=	restart_queue.instrument_before(instrument_job)	;

				/**	@li	Return	how	many	insertions	were	done.	*/
				return(count)	;

}

int	Trick::CheckPointRestart::instrument_job_after(	Trick::JobData	*	instrument_job	)	{

				int	count	=	0	;

				/**	@par	Detailed	Design	*/
				//	if	target_job	specified,	instrument	job	will	only	be	inserted	after	it
				//	note:	when	more	than	one	target	job	with	same	name,	this	will	insert	instrument	job	for	1st	occurrence	of	target_job	in	each	queue

				count	+=	checkpoint_queue.instrument_after(instrument_job)	;
				count	+=	preload_checkpoint_queue.instrument_after(instrument_job)	;
				count	+=	restart_queue.instrument_after(instrument_job)	;

				/**	@li	Return	how	many	insertions	were	done.	*/
				return(count)	;

}

int	Trick::CheckPointRestart::instrument_job_remove(std::string	in_job)	{

				checkpoint_queue.instrument_remove(in_job)	;
				preload_checkpoint_queue.instrument_remove(in_job)	;
				restart_queue.instrument_remove(in_job)	;

				return(0)	;
}

