
#include	"trick/JITEvent.hh"
#include	"trick/jit_input_file_proto.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"

//	Default	constructor	used	by	Trick	for	checkpoints.
Trick::JITEvent::JITEvent()	:
	func_ptr(NULL)	{
				active	=	false	;
}

Trick::JITEvent::JITEvent(std::string	in_func_name	,	std::string	in_name	,	double	in_cycle	)	:
	Trick::Event(in_name,	in_cycle)	,
	func_name(in_func_name)	{
				//	Set	active	to	true	by	default	for	all	JITEvents
				active	=	true	;
				get_func_ptr_from_name()	;
}

/**
@details
-#	call	the	function	and	return	it's	return	value.
*/
int	Trick::JITEvent::process(	long	long	curr_time	__attribute__	((unused))	)	{
				if	(	func_ptr	!=	NULL	)	{
								return	(*func_ptr)()	;
				}
				return	1	;
}

/**
@details
-#	Get	the	function	pointer	associated	with	this	event.
*/
void	Trick::JITEvent::restart()	{
				get_func_ptr_from_name()	;
}

/**
@details
-#	If	func_name	is	not	empty
	-#	Get	the	function	pointer	associated	with	the	func_name
	-#	If	the	function	is	not	found
		-#	Print	an	error	message	saying	we	can't	find	the	function
*/
void	Trick::JITEvent::get_func_ptr_from_name()	{
				if	(	!	func_name.empty()	)	{
								func_ptr	=	(int	(*)(void))jit_find_symbol(func_name)	;
								if	(	func_ptr	==	NULL	)	{
												message_publish(	MSG_WARNING,	"JITEvent	could	not	find	function	named	%s",	func_name.c_str()	)	;
								}
				}
}
