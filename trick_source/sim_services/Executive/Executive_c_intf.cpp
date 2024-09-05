
#include	<iostream>
#include	<vector>
#include	<string>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"

/*	Global	singleton	pointer	to	the	executive	*/
extern	Trick::Executive	*	the_exec	;

/**
	*	@relates	Trick::Executive
	*	returns	a	pointer	to	the	simulation	executive
	*	@return	Executive
	*/
Trick::Executive	*	exec_get_exec_cpp()	{
				return	(	the_exec	)	;
}

/*********************************************************/
/*	These	routines	are	the	"C"	interface	to	the	executive	*/
/*********************************************************/

//	--------------------------	GET	------------------------

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_attach_debugger
	*	C	wrapper	for	Trick::Executive::get_attach_debugger
	*/
extern	"C"	int	exec_get_attach_debugger()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_attach_debugger()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_debugger_command
	*	C	wrapper	for	Trick::Executive::get_debugger_command
	*/
extern	"C"	const	char	*	exec_get_debugger_command()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_debugger_command().c_str()	;
				}
				return	NULL	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_enable_freeze
	*	C	wrapper	for	Trick::Executive::get_enable_freeze
	*/
extern	"C"	int	exec_get_enable_freeze()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_enable_freeze()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_exec_command
	*	C	wrapper	for	Trick::Executive::get_exec_command
	*/
extern	"C"	SIM_COMMAND	exec_get_exec_command()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_exec_command()	;
				}
				return	NoCmd	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_frame
	*	C	wrapper	for	Trick::Executive::get_freeze_frame
	*/
extern	"C"	double	exec_get_freeze_frame()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_freeze_frame()	;
				}
				return	0.0	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_command
	*	C	wrapper	for	Trick::Executive::get_freeze_command
	*/
extern	"C"	int	exec_get_freeze_command()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_freeze_command()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_on_frame_boundary
	*	C	wrapper	for	Trick::Executive::get_freeze_on_frame_boundary
	*/
extern	"C"	int	exec_get_freeze_on_frame_boundary()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_freeze_on_frame_boundary()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_frame_tics
	*	C	wrapper	for	Trick::Executive::get_freeze_frame_tics
	*/
extern	"C"	long	long	exec_get_freeze_frame_tics()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_freeze_frame_tics()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_frame_count
	*	C	wrapper	for	Trick::Executive::get_freeze_frame_count
	*/
extern	"C"	long	long	exec_get_freeze_frame_count()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_freeze_frame_count()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_job_cycle
	*	C	wrapper	for	Trick::Executive::get_job_cycle
	*/
extern	"C"	double	exec_get_job_cycle(const	char	*	job_name)	{

				if	(	the_exec	!=	NULL	)	{

								std::string	job_string	;

								//	Add	handling	for	NULL	string.
								if	(	job_name	==	NULL	)	{
												job_string.clear()	;
								}	else	{
												job_string	=	std::string(job_name)	;
								}
								return	the_exec->get_job_cycle(job_string)	;
				}
				return	-1.0	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_mode
	*	C	wrapper	for	Trick::Executive::get_mode
	*/
extern	"C"	SIM_MODE	exec_get_mode()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_mode()	;
				}
				return	Initialization	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_num_threads
	*	C	wrapper	for	Trick::Executive::get_num_threads
	*/
extern	"C"	unsigned	int	exec_get_num_threads()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_num_threads()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_time_tic_value
	*	C	wrapper	for	Trick::Executive::get_time_tic_value
	*/
extern	"C"	int	exec_get_old_time_tic_value()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_old_time_tic_value()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_process_id
	*	C	wrapper	for	Trick::Executive::get_process_id
	*/
extern	"C"	unsigned	int	exec_get_process_id()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_process_id()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_rt_nap
	*	C	wrapper	for	Trick::Executive::get_rt_nap
	*/
extern	"C"	int	exec_get_rt_nap()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_rt_nap()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_scheduled_start_index
	*	C	wrapper	for	Trick::Executive::get_scheduled_start_index
	*/
extern	"C"	int	exec_get_scheduled_start_index()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_scheduled_start_index()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_sim_time
	*	C	wrapper	for	Trick::Executive::get_sim_time
	*/
extern	"C"	double	exec_get_sim_time()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_sim_time()	;
				}
				return	-1.0	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_software_frame
	*	C	wrapper	for	Trick::Executive::get_software_frame
	*/
extern	"C"	double	exec_get_software_frame()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_software_frame()	;
				}
				return	-1.0	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_software_frame_tics
	*	C	wrapper	for	Trick::Executive::get_software_frame_tics
	*/
extern	"C"	long	long	exec_get_software_frame_tics()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_software_frame_tics()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_frame_count
	*	C	wrapper	for	Trick::Executive::get_frame_count
	*/
extern	"C"	long	long	exec_get_frame_count()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_frame_count()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_stack_trace
	*	C	wrapper	for	Trick::Executive::get_stack_trace
	*/
extern	"C"	int	exec_get_stack_trace()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_stack_trace()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_terminate_time
	*	C	wrapper	for	Trick::Executive::get_terminate_time
	*/
extern	"C"	double	exec_get_terminate_time()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_terminate_time()	;
				}
				return	-1.0	;
}

extern	"C"	double	exec_get_thread_amf_cycle_time(unsigned	int	thread_id)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_thread_amf_cycle_time(thread_id)	;
				}
				return	-1.0	;
}
/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_time_tic_value
	*	C	wrapper	for	Trick::Executive::get_time_tic_value
	*/
extern	"C"	int	exec_get_time_tic_value()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_time_tic_value()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_time_tics
	*	C	wrapper	for	Trick::Executive::get_time_tics
	*/
extern	"C"	long	long	exec_get_time_tics()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_time_tics()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_freeze_time_tics
	*	C	wrapper	for	Trick::Executive::get_freeze_time_tics
	*/
extern	"C"	long	long	exec_get_freeze_time_tics()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_freeze_time_tics()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_trap_sigbus
	*	C	wrapper	for	Trick::Executive::get_trap_sigbus
	*/
extern	"C"	int	exec_get_trap_sigbus()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_trap_sigbus()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_trap_sigfpe
	*	C	wrapper	for	Trick::Executive::get_trap_sigfpe
	*/
extern	"C"	int	exec_get_trap_sigfpe()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_trap_sigfpe()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_trap_sigsegv
	*	C	wrapper	for	Trick::Executive::get_trap_sigsegv
	*/
extern	"C"	int	exec_get_trap_sigsegv()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_trap_sigsegv()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_trap_sigabrt
	*	C	wrapper	for	Trick::Executive::get_trap_sigabrt
	*/
extern	"C"	int	exec_get_trap_sigabrt()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_trap_sigabrt()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_trap_sigchld
	*	C	wrapper	for	Trick::Executive::get_trap_sigchld
	*/
extern	"C"	int	exec_get_trap_sigchld()	{
				if	(	the_exec	!=	NULL	)	{
								return	(int)the_exec->get_trap_sigchld()	;
				}
				return	-1	;
}

//	--------------------------	SET	------------------------

/**
@relates	Trick::Executive
@copybrief	Trick::Executive::set_attach_debugger

@par	Python	Usage:
@code	trick.exec_set_attach_debugger(<on_off>)	@endcode

C	wrapper	for	Trick::Executive::set_attach_debugger
	*/
extern	"C"	int	exec_set_attach_debugger(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_attach_debugger((bool)on_off)	;
				}
				return	-1	;
}

/**
@relates	Trick::Executive
@copydoc	Trick::Executive::set_debugger_command

@par	Python	Usage:
@code	trick.exec_set_debugger_command("<command>")	@endcode

C	wrapper	for	Trick::Executive::set_debugger_command
*/
extern	"C"	int	exec_set_debugger_command(	const	char	*	command	)	{
				if	(	the_exec	!=	NULL	)	{
								return	(	the_exec->set_debugger_command(std::string(command))	)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_enable_freeze
	*	C	wrapper	for	Trick::Executive::set_enable_freeze
	*/
extern	"C"	int	exec_set_enable_freeze(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_enable_freeze((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_exec_command
	*	C	wrapper	for	Trick::Executive::set_exec_command
	*/
//	TODO:		I	don't	think	we	need	this	function,	too	dangerous.	(Alex	3/2/12)
extern	"C"	int	exec_set_exec_command(SIM_COMMAND	in_command)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_exec_command(in_command)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_freeze_command
	*	C	wrapper	for	Trick::Executive::set_freeze_command
	*/
extern	"C"	int	exec_set_freeze_command(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_freeze_command((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_freeze_on_frame_boundary
	*	C	wrapper	for	Trick::Executive::set_freeze_on_frame_boundary
	*/
extern	"C"	int	exec_set_freeze_on_frame_boundary(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_freeze_on_frame_boundary((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_freeze_frame
	*	C	wrapper	for	Trick::Executive::set_freeze_frame
	*/
extern	"C"	int	exec_set_freeze_frame(double	in_frame)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_freeze_frame(in_frame)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_rt_nap
	*	C	wrapper	for	Trick::Executive::set_rt_nap
	*/
extern	"C"	int	exec_set_rt_nap(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_rt_nap((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_software_frame
	*	C	wrapper	for	Trick::Executive::set_software_frame
	*/
extern	"C"	int	exec_set_software_frame(double	in_frame)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_software_frame(in_frame)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_stack_trace
	*	C	wrapper	for	Trick::Executive::set_stack_trace
	*/
extern	"C"	int	exec_set_stack_trace(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_stack_trace((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_terminate_time
	*	C	wrapper	for	Trick::Executive::set_terminate_time
	*/
extern	"C"	int	exec_set_terminate_time(double	time_value)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_terminate_time(time_value)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_time
	*	C	wrapper	for	Trick::Executive::set_time
	*/
extern	"C"	int	exec_set_time(	double	in_time	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_time(in_time)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_time_tics
	*	C	wrapper	for	Trick::Executive::set_time_tics
	*/
extern	"C"	int	exec_set_time_tics(	long	long	in_time_tics	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_time_tics(in_time_tics)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_time_tic_value
	*	C	wrapper	for	Trick::Executive::set_time_tic_value
	*/
extern	"C"	int	exec_set_time_tic_value(	int	in_time_tics	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_time_tic_value(in_time_tics)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_trap_sigbus
	*	C	wrapper	for	Trick::Executive::set_trap_sigbus
	*/
extern	"C"	int	exec_set_trap_sigbus(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_trap_sigbus((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_trap_sigfpe
	*	C	wrapper	for	Trick::Executive::set_trap_sigfpe
	*/
extern	"C"	int	exec_set_trap_sigfpe(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_trap_sigfpe((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_trap_sigsegv
	*	C	wrapper	for	Trick::Executive::set_trap_sigsegv
	*/
extern	"C"	int	exec_set_trap_sigsegv(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_trap_sigsegv((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_trap_sigsegv
	*	C	wrapper	for	Trick::Executive::set_trap_sigsegv
	*/
extern	"C"	int	exec_set_trap_sigabrt(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_trap_sigabrt((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_trap_sigsegv
	*	C	wrapper	for	Trick::Executive::set_trap_sigsegv
	*/
extern	"C"	int	exec_set_trap_sigchld(	int	on_off	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_trap_sigchld((bool)on_off)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_job_onoff
	*	C	wrapper	for	Trick::Executive::set_job_onoff
	*/
extern	"C"	int	exec_set_job_onoff(const	char	*	job_name	,	int	instance	,	int	on)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_job_onoff(	job_name	,	instance	,	on)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_sim_object_onoff
	*	C	wrapper	for	Trick::Executive::set_sim_object_onoff
	*/
extern	"C"	int	exec_set_sim_object_onoff(const	char	*	sim_object_name	,	int	on)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_sim_object_onoff(	sim_object_name	,	on)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_sim_object_onoff
	*	C	wrapper	for	Trick::Executive::get_sim_object_onoff
	*/
extern	"C"	int	exec_get_sim_object_onoff(const	char	*	sim_object_name	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_sim_object_onoff(	sim_object_name	)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_sim_object_jobs_onoff
	*	C	wrapper	for	Trick::Executive::set_sim_object_jobs_onoff
	*/
extern	"C"	int	exec_set_sim_object_jobs_onoff(const	char	*	sim_object_name	,	int	on)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_sim_object_jobs_onoff(	sim_object_name	,	on)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_version_date_tag
	*	C	wrapper	for	Trick::Executive::set_version_date_tag
	*/
extern	"C"	int	exec_set_version_date_tag(const	char	*	tag)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_version_date_tag(std::string(tag))	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_build_date
	*	C	wrapper	for	Trick::Executive::set_build_date
	*/
extern	"C"	int	exec_set_build_date(const	char	*	date)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_build_date(std::string(date))	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_current_version
	*	C	wrapper	for	Trick::Executive::set_current_version
	*/
extern	"C"	int	exec_set_current_version(const	char	*	version)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_current_version(std::string(version))	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_current_version
	*	C	wrapper	for	Trick::Executive::set_current_version
	*/
extern	"C"	const	char	*	exec_get_current_version()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_current_version().c_str()	;
				}
				return	NULL	;
}

/**
	*	@relates	Trick::Executive
	*	@userdesc	Raise	a	signal	on	the	main	thread	to	terminate	the	simuation	immediately.
	*	This	is	not	typically	used	by	users.		Use	exec_terminate	instead.
	*/
extern	"C"	void	exec_signal_terminate()	{
				//	ret_code	=	0	for	this	call.
				if	(	the_exec	!=	NULL	)	{
								pthread_kill(	the_exec->get_pthread_id(0)	,	SIGUSR1	)	;
				}
}

/**
	*	@relates	Trick::Executive
	*	@userdesc	C	wrapper	to	transition	to	Shutdown	mode.		Throws	Trick::ExecutiveException	exception.		This	version
	*	@	is	backwards	compatible
	*	@param	file_name	-	file	name	of	the	caller	to	exec_terminate
	*	@param	error	-	error	message	to	print
	*/
extern	"C"	int	exec_terminate(const	char	*file_name	,	const	char	*error	)	{
				throw	Trick::ExecutiveException(0	,	file_name	,	0	,	error)	;
				//	never	reached
				return	0	;
}

/**
	*	@relates	Trick::Executive
	*	@userdesc	C	wrapper	to	transition	to	Shutdown	mode.		Throws	Trick::ExecutiveException	exception
	*	@param	ret_code	-	return	code	to	pass	back	to	caller	of	init()	or	loop()	;
	*	@param	file_name	-	file	name	of	the	caller	to	exec_terminate
	*	@param	line	-	line	number	where	exception	occurred
	*	@param	error	-	error	message	to	print
	*	@return	ret_code	(but	never	reached)
	*/
extern	"C"	int	exec_terminate_with_return(int	ret_code	,	const	char	*file_name	,	int	line	,	const	char	*error	)	{
				throw	Trick::ExecutiveException(ret_code	,	file_name	,	line	,	error)	;
				//	never	reached
				return	ret_code	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::freeze
	*	C	wrapper	for	Trick::Executive::freeze
	*/
extern	"C"	int	exec_freeze()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->freeze()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::run
	*	C	wrapper	for	Trick::Executive::run
	*/
extern	"C"	int	exec_run()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->run()	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
			@userdesc	C	wrapper	to	add	an	instrumentation	job	before	all	initialization,	integration,
				scheduled,	shutdown,	top_of_frame	&	end_of_frame	jobs.
	*	Calls	Trick::Executive::instrument_job_before(job_name,	"",	NULL)
	*	@param	job_name	-	name	of	the	job	to	instrument.		Name	follows	convention	in	RUN_<dir>/S_job_execution
	*	@return	number	of	times	the	job	was	inserted
	*/
extern	"C"	int	exec_instrument_before(const	char	*	job_name)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->instrument_job_before(job_name)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
			@userdesc	C	wrapper	to	add	an	instrumentation	job	after	all	initialization,	integration,
				scheduled,	shutdown,	top_of_frame	&	end_of_frame	jobs.
	*	Calls	Trick::Executive::instrument_job_after(job_name,	"",	NULL)
	*	@param	job_name	-	name	of	the	job	to	instrument.		Name	follows	convention	in	RUN_<dir>/S_job_execution
	*	@return	number	of	times	the	job	was	inserted
	*/
extern	"C"	int	exec_instrument_after(const	char	*	job_name)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->instrument_job_after(job_name)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
			@userdesc	C	wrapper	to	remove	all	jobs	with	name	job_name	from	all	job	queues.
	*	Calls	Trick::Executive::instrument_job_remove(job_name,	NULL)
	*	@param	job_name	-	name	of	the	job	to	remove.		Name	follows	convention	in	RUN_<dir>/S_job_execution
	*	@return	always	0
	*/
extern	"C"	int	exec_instrument_remove(const	char	*	job_name)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->instrument_job_remove(job_name)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_enabled
	*	C	wrapper	for	Trick::Executive::set_thread_enabled
	*/
extern	"C"	int	exec_set_thread_enabled(	unsigned	int	thread_id	,	int	yes_no	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_enabled(thread_id	,	yes_no)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_amf_cycle_time
	*	C	wrapper	for	Trick::Executive::set_thread_amf_cycle_time
	*/
extern	"C"	int	exec_set_thread_amf_cycle_time(	unsigned	int	thread_id	,	double	cycle_time	)	{
				return	exec_set_thread_async_cycle_time(thread_id	,	cycle_time)	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_async_cycle_time
	*	C	wrapper	for	Trick::Executive::set_thread_async_cycle_time
	*/
extern	"C"	int	exec_set_thread_async_cycle_time(	unsigned	int	thread_id	,	double	cycle_time	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_amf_cycle_time(thread_id	,	cycle_time)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_async_wait
	*	C	wrapper	for	Trick::Executive::set_thread_async_wait
	*/
extern	"C"	int	exec_set_thread_async_wait(	unsigned	int	thread_id	,	int	yes_no	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_async_wait(thread_id	,	(bool)yes_no)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_rt_semaphores
	*	C	wrapper	for	Trick::Executive::set_thread_rt_semaphores
	*/
extern	"C"	int	exec_set_thread_rt_semaphores(	unsigned	int	thread_id	,	int	yes_no	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_rt_semaphores(thread_id	,	(bool)yes_no)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_cpu_affinity
	*	C	wrapper	for	Trick::Executive::set_thread_cpu_affinity
	*/
extern	"C"	int	exec_set_thread_cpu_affinity(unsigned	int	thread_id	,	int	cpu_num)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_cpu_affinity(thread_id,	cpu_num)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_priority
	*	C	wrapper	for	Trick::Executive::set_thread_priority
	*/
extern	"C"	int	exec_set_thread_priority(unsigned	int	thread_id	,	unsigned	int	req_priority)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_priority(thread_id,	req_priority)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_thread_process_type
	*	C	wrapper	for	Trick::Executive::set_thread_process_type
	*/
extern	"C"	int	exec_set_thread_process_type(	unsigned	int	thread_id	,	int	process_type	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->set_thread_process_type(thread_id	,	process_type)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::set_job_cycle
	*	C	wrapper	for	Trick::Executive::set_job_cycle
	*/
extern	"C"	int	exec_set_job_cycle(const	char	*	job_name,	int	instance,	double	in_cycle)	{

				if	(	the_exec	!=	NULL	)	{
								std::string	job_string	;

								//	Add	handling	for	NULL	string.
								if	(	job_name	==	NULL	)	{
												job_string.clear()	;
								}	else	{
												job_string	=	std::string(job_name)	;
								}
								return	the_exec->set_job_cycle(job_string,	instance,	in_cycle)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::add_depends_on_job
	*	C	wrapper	for	Trick::Executive::add_depends_on_job
	*/
extern	"C"	int	exec_add_scheduled_job_class(const	char	*	in_name)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->add_scheduled_job_class(std::string(in_name))	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::add_depends_on_job
	*	C	wrapper	for	Trick::Executive::add_depends_on_job
	*/
extern	"C"	int	exec_add_depends_on_job(const	char	*	target_job_string	,	unsigned	int	t_instance	,	const	char	*	depend_job_string	,	unsigned	int	d_instance	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->add_depends_on_job(std::string(target_job_string)	,	t_instance	,	std::string(depend_job_string)	,	d_instance	)	;
				}
				return	-1	;
}

/*********************************************************/
/*	These	are	executive	access	functions	that	stay	C++				*/

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::add_sim_object
	*	C	wrapper	for	Trick::Executive::add_sim_object
	*/
int	exec_add_sim_object(	Trick::SimObject	*	in_object	,	const	char	*	in_name	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->add_sim_object(in_object	,	in_name)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::add_sim_object
	*	C	wrapper	for	Trick::Executive::add_sim_object
	*/
int	exec_remove_sim_object(	Trick::SimObject	*	in_object	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->delete_sim_object(in_object)	;
				}
				return	-1	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_sim_objects
	*	Wrapper	for	Trick::Executive::get_sim_objects
	*/
int	exec_get_sim_objects(std::vector<Trick::SimObject	*>&	in_sim_objects	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_sim_objects(in_sim_objects)	;
				}
				return	-1	;
}


/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_sim_objects
	*	Wrapper	for	Trick::Executive::get_sim_objects
	*/
Trick::SimObject	*	exec_get_sim_object_by_name(std::string	sim_object_name)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_sim_object_by_name(sim_object_name)	;
				}
				return	NULL	;
}


/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_job
	*	Wrapper	for	Trick::Executive::get_job
	*/
Trick::JobData	*	exec_get_job(const	char	*	job_name	,	unsigned	int	j_instance)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_job(std::string(job_name),	j_instance)	;
				}
				return	NULL	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_curr_job
	*	Wrapper	for	Trick::Executive::get_curr_job
	*/
Trick::JobData	*	exec_get_curr_job()	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_curr_job()	;
				}
				return	NULL	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_thread
	*	C	wrapper	for	Trick::Executive::get_thread
	*/
Trick::Threads	*	exec_get_thread(	unsigned	int	thread_id	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_thread(thread_id)	;
				}
				return	NULL	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::get_all_jobs_vector
	*	Wrapper	for	Trick::Executive::get_all_jobs_vector
	*/
int	exec_get_all_jobs_vector(std::vector<Trick::JobData	*>	&	in_jobs_vector)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->get_all_jobs_vector(in_jobs_vector)	;
				}
				return	0	;
}

/**
	*	@relates	Trick::Executive
	*	@copydoc	Trick::Executive::register_scheduler
	*	Wrapper	for	Trick::Executive::register_scheduler
	*/
int	exec_register_scheduler(	Trick::Scheduler	*	scheduler	)	{
				if	(	the_exec	!=	NULL	)	{
								return	the_exec->register_scheduler(scheduler)	;
				}
				return	-1	;
}

