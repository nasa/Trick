
#include	"trick/MessageThreadedCout.hh"
#include	"trick/message_type.h"
#include	"trick/release.h"

#include	<sstream>

Trick::MessageThreadedCout::MessageThreadedCout()	:
	max_buffer_items(400),
	max_buffer_size(4000)	,
	print_immediate(false)	,
	copy_ptr(NULL),
	write_ptr(NULL),
	SysThread("threadedcout")	{
				/**	By	default,	this	subscriber	is	enabled	when	it	is	created.	*/
				Trick::MessageSubscriber::name	=	"threadedcout"	;
				color_code.reserve(6)	;
				StringNode	*	temp	=	new	StringNode(max_buffer_size)	;
				write_ptr	=	copy_ptr	=	temp	;
				for	(	unsigned	int	ii	=	1	;	ii	<	max_buffer_items	;	ii++	)	{
								temp	=	new	StringNode(max_buffer_size)	;
								copy_ptr->next	=	temp	;
								copy_ptr	=	temp	;
				}
				copy_ptr->next	=	write_ptr	;
				copy_ptr	=	write_ptr	;

				pthread_mutex_init(&write_mutex,	NULL);
}

int	Trick::MessageThreadedCout::init()	{
				create_thread()	;
				return	0	;
}

void	Trick::MessageThreadedCout::update(	unsigned	int	level	,	std::string	header	,	std::string	message	)	{

				/**	@li	Prints	the	received	message	to	the	standard	output	stream.	*/
				if	(enabled	&&	level	<	100	)	{
								switch	(level)	{
												case	MSG_NORMAL	:
																color_code	=	"\033[00m"	;	//	normal
																break	;
												case	MSG_INFO	:
																color_code	=	"\033[32m"	;	//	green
																break	;
												case	MSG_WARNING	:
																color_code	=	"\033[33m"	;	//	yellow
																break	;
												case	MSG_ERROR	:
																color_code	=	"\033[31m"	;	//	red
																break	;
												case	MSG_DEBUG	:
																color_code	=	"\033[36m"	;	//	cyan
																break	;
												default	:
																color_code	=	"\033[00m"	;	//	normal
																break	;
								}

								if	(	copy_ptr->next	!=	write_ptr	)	{
												copy_ptr->copy(header,	color_code,	message)	;
												copy_ptr	=	copy_ptr->next	;
												//	After	shutdown	the	sim	sends	a	termination	message.	print	it	now.
												if	(	print_immediate	)	{
																write_pending_messages()	;
												}
								}
				}
}

void	*	Trick::MessageThreadedCout::thread_body()	{
				while(1)	{
								write_pending_messages()	;
								RELEASE()	;
				}
				return	(void*)0;
}

void	Trick::MessageThreadedCout::write_pending_messages()	{
				pthread_mutex_lock(&write_mutex)	;
				while	(	write_ptr	!=	copy_ptr	)	{
								std::cout	<<	write_ptr->buffer	<<	std::flush	;
								write_ptr	=	write_ptr->next	;
				}
				pthread_mutex_unlock(&write_mutex)	;
}

int	Trick::MessageThreadedCout::shutdown()	{

				write_pending_messages()	;
				//	After	shutdown	print	any	late	incoming	messages	immediately.
				print_immediate	=	true	;

				//TODO:	delete	memory

				return	0	;
}

void	Trick::MessageThreadedCout::dump(	std::ostream	&	oss	)	{
				Trick::ThreadBase::dump(oss)	;
}

