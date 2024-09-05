
#include	<stdlib.h>

#include	"trick/CheckPointRestart.hh"
#include	"trick/CheckPointRestart_c_intf.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**	Global	singleton	pointer	to	the	CheckPointRestart.	*/
extern	Trick::CheckPointRestart	*	the_cpr	;

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_pre_init_checkpoint
	*/
extern	"C"	int	checkpoint_pre_init(	int	yes_no	)	{
				the_cpr->set_pre_init_checkpoint(bool(yes_no))	;
				return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_post_init_checkpoint
	*/
extern	"C"	int	checkpoint_post_init(	int	yes_no	)	{
				the_cpr->set_post_init_checkpoint(bool(yes_no))	;
				return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_end_checkpoint
	*/
extern	"C"	int	checkpoint_end(	int	yes_no	)	{
				the_cpr->set_end_checkpoint(bool(yes_no))	;
				return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_pre_init_checkpoint
	*/
extern	"C"	int	get_checkpoint_pre_init()	{
				return	(int)the_cpr->pre_init_checkpoint	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_post_init_checkpoint
	*/
extern	"C"	int	get_checkpoint_post_init()	{
				return	(int)the_cpr->post_init_checkpoint	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_end_checkpoint
	*/
extern	"C"	int	get_checkpoint_end()	{
				return	(int)the_cpr->end_checkpoint	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_safestore_enabled
	*/
extern	"C"	int	checkpoint_safestore(	int	yes_no	)	{
				the_cpr->set_safestore_enabled(bool(yes_no))	;
				return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_safestore_time
	*/
extern	"C"	int	checkpoint_safestore_period(	double	in_time	)	{
				the_cpr->set_safestore_time(in_time)	;
				return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::set_cpu_num
	*/
extern	"C"	int	checkpoint_cpu(	int	in_cpu_num	)	{
				the_cpr->set_cpu_num(in_cpu_num)	;
#if	__APPLE__
				message_publish(MSG_WARNING,	"Warning:	(Setting	checkpoint	cpu)	Trick	on	Darwin	does	not	yet	support	processor	assignment.\n");
#endif
				return(0)	;
}


/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::get_output_file
	*/
extern	"C"	const	char	*	checkpoint_get_output_file()	{

			return	the_cpr->get_output_file()	;

}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::get_load_file
	*/
extern	"C"	const	char	*	checkpoint_get_load_file()	{

			return	the_cpr->get_load_file()	;

}

/**
	*	@relates	Trick::CheckPointRestart
	@brief	@userdesc	Command	to	dump	a	checkpoint	now	to	the	specified	file.
	This	is	invoked	when	the	user	clicks	the	"Dump	ASCII	Chkpnt"	button	on	the	sim	control	panel.
	@par	Python	Usage:
	@code	trick.checkpoint("<file_name>")	@endcode
	@param	file_name	-	name	of	checkpoint	file	to	dump	(leave	blank	and	Trick	will	use	filename	"chkpnt_<simtime>"
	@return	always	0
	*/
extern	"C"	int	checkpoint(	const	char	*	file_name	)	{

				the_cpr->checkpoint(std::string(file_name))	;

				return(0)	;

}

/**
	*	@relates	Trick::CheckPointRestart
	@brief	@userdesc	Command	to	dump	a	checkpoint	now	to	the	specified	file,	only	dumping	the	specified	sim	objects.
	This	is	invoked	when	the	user	clicks	the	"Dump	ASCII	Chkpnt"	button	on	the	sim	control	panel	after	having	set	"Checkpoint	Objects"	under	the	"Actions"	menu.
	@par	Python	Usage:
	@code	trick.checkpoint_objects("<file_name>",	"<object>[,<object>,...]")	@endcode
	@param	file_name	-	name	of	checkpoint	file	to	dump	(leave	blank	and	Trick	will	use	filename	"chkpnt_<simtime>"
	@param	objects	-	string	of	comma	separated	sim	object	names	to	dump
	@return	always	0
	*/
extern	"C"	int	checkpoint_objects(	const	char	*	file_name	,	const	char	*	objects	)	{

	the_cpr->checkpoint(std::string(file_name),	true,	std::string(objects))	;

	return(0)	;
}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::load_checkpoint
	*/
extern	"C"	int	load_checkpoint(	const	char	*	file_name	)	{

				the_cpr->load_checkpoint(std::string(file_name))	;

				return(0)	;

}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::load_checkpoint
	*/
extern	"C"	int	load_checkpoint_stls(	const	char	*	file_name,	int	on_off	)	{

				the_cpr->load_checkpoint(std::string(file_name),	(bool)	on_off)	;

				return(0)	;

}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::load_checkpoint
	*/
extern	"C"	int	load_checkpoint_objects(	const	char	*	file_name	)	{

				the_cpr->load_checkpoint(std::string(file_name))	;

				return(0)	;

}

/**
	*	@relates	Trick::CheckPointRestart
	*	@copydoc	Trick::CheckPointRestart::load_checkpoint_job
	*/
extern	"C"	int	load_checkpoint_job()	{

				the_cpr->load_checkpoint_job()	;

				return(0)	;

}

/**
	*	@relates	Trick::CheckPointRestart
	*	This	get_address	has	C	bindings	and	is	callable	from	regular	C	code.
	*	@param	var_name	-	name	of	variable
	*/
extern	"C"	void	*	get_address(	const	char	*	var_name	)	{

				REF2	*	new_ref	;
				new_ref	=	ref_attributes(var_name)	;
				void	*	address	;

				if	(	new_ref	!=	NULL	)	{
								address	=	new_ref->address	;
								ref_free(new_ref)	;
								free(new_ref)	;
								return(address)	;
				}

				return(NULL)	;

}
