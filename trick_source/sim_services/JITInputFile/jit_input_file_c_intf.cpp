#include	<cmath>

#include	"trick/jit_input_file_proto.hh"
#include	"trick/JITInputFile.hh"
#include	"trick/exec_proto.h"
#include	"trick/EventManager_c_intf.hh"

extern	Trick::JITInputFile	*	the_jit	;

int	jit_compile(std::string	file_name)	{
				if	(	the_jit	!=	NULL	)	{
								return	the_jit->compile(file_name)	;
				}
				return	-1	;
}

int	jit_run(std::string	library_name	,	std::string	run_function	)	{
				if	(	the_jit	!=	NULL	)	{
								return	the_jit->run(library_name,	run_function)	;
				}
				return	-1	;
}

int	jit_compile_and_run(std::string	file_name,	std::string	run_function	)	{
				if	(	the_jit	!=	NULL	)	{
								return	the_jit->compile_and_run(file_name,	run_function)	;
				}
				return	-1	;
}

int	jit_add_library(std::string	lib_name	)	{
				if	(	the_jit	!=	NULL	)	{
								return	the_jit->add_library(lib_name)	;
				}
				return	-1	;
}

void	*	jit_find_symbol(std::string	sym	)	{
				if	(	the_jit	!=	NULL	)	{
								return	the_jit->find_symbol(sym)	;
				}
				return	NULL	;
}

int	jit_add_read(	unsigned	int	thread_id	,	double	in_time	,	std::string	func_name	)	{
				Trick::JITEvent	*	event	=	(Trick::JITEvent	*)TMM_declare_var_1d("Trick::JITEvent",	1)	;
				new	(event)	Trick::JITEvent(func_name,	"jit_read_event"	,	0)	;
				event->set_thread(thread_id)	;
				event->set_next_tics((long	long)round(in_time	*	exec_get_time_tic_value()))	;
				return	event_manager_add_event(event)	;
}

int	jit_add_read(	double	in_time	,	std::string	func_name)	{
				return	jit_add_read(	0	,	in_time	,	func_name)	;
}

int	jit_add_event(	std::string	func_name,	std::string	name	,	double	cycle	)	{
				Trick::JITEvent	*	event	=	(Trick::JITEvent	*)TMM_declare_var_1d("Trick::JITEvent",	1)	;
				new	(event)	Trick::JITEvent(func_name	,	name	,	cycle	)	;
				return	event_manager_add_event(event)	;
}

int	jit_add_event_before(	std::string	func_name	,	std::string	name	,	std::string	target_name	,	unsigned	int	target_inst	)	{
				Trick::JITEvent	*	event	=	(Trick::JITEvent	*)TMM_declare_var_1d("Trick::JITEvent",	1)	;
				new	(event)	Trick::JITEvent(func_name	,	name	)	;
				return	event_manager_add_event_before(event,	target_name	,	target_inst)	;
}

int	jit_add_event_after(	std::string	func_name,	std::string	name	,	std::string	target_name	,	unsigned	int	target_inst	)	{
				Trick::JITEvent	*	event	=	(Trick::JITEvent	*)TMM_declare_var_1d("Trick::JITEvent",	1)	;
				new	(event)	Trick::JITEvent(func_name	,	name	)	;
				return	event_manager_add_event_after(event,	target_name	,	target_inst)	;
}

