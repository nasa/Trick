
#include	<iostream>
#include	<stdio.h>
#include	<stdarg.h>

#include	"trick/MessagePublisher.hh"
#include	"trick/MessageCout.hh"
#include	"trick/MessageFile.hh"
#include	"trick/MessageCustomFile.hh"
#include	"trick/MessageTCDevice.hh"
#include	"trick/message_proto.h"

#define	MAX_MSG_HEADER_SIZE	256
#define	MAX_MSG_SIZE	20480

/**	Global	pointer	to	the	MessagePubliser.	*/
extern	Trick::MessagePublisher	*	the_message_publisher	;

/**
	@relates	Trick::MessagePublisher
	@copydoc	Trick::MessagePublisher::subscribe
	*/
int	message_subscribe(	Trick::MessageSubscriber	*	in_ms	)	{
				the_message_publisher->subscribe(in_ms)	;
				return(0)	;
}
	
/**
	@relates	Trick::MessagePublisher
	@copydoc	Trick::MessagePublisher::unsubscribe
	*/
int	message_unsubscribe(	Trick::MessageSubscriber	*	in_ms	)	{
				the_message_publisher->unsubscribe(in_ms)	;
				return(0)	;
}


/**
	@relates	Trick::MessagePublisher
	@copydoc	Trick::MessagePublisher::subscribe
	*/
extern	"C"	int	message_add_subscriber(	void	*	in_ms	)	{
				the_message_publisher->subscribe((Trick::MessageSubscriber	*)in_ms)	;
				return(0)	;
}
	
/**
	@relates	Trick::MessagePublisher
	@copydoc	Trick::MessagePublisher::unsubscribe
	*/
extern	"C"	int	message_remove_subscriber(	void	*	in_ms	)	{
				the_message_publisher->unsubscribe((Trick::MessageSubscriber	*)in_ms)	;
				return(0)	;
}

/**
	@relates	Trick::MessagePublisher
	@copydoc	Trick::MessagePublisher::getSubscriber
	*/
extern	"C"	void	*	message_get_subscriber(	const	char	*	sub_name	)	{
				return	(void	*)the_message_publisher->getSubscriber(sub_name)	;
}

/**
	@relates	Trick::MessagePublisher
	@userdesc	Command	to	publish	a	message,	which	sends	the	message	to	all	subscribers.
	Creates	a	header	that	is	prepended	to	the	message,	so	that	the	output	looks	like	this:
	@code	|L	<level>|<date>|<hostname>|<Trick::MessagePublisher::sim_name>|T	<process	id>|<sim	time>|	<format_msg>	@endcode
	Calls	Trick::MessagePublisher::publish.
	@par	Python	Usage:
	@code	trick.message_publish(<level>,	"<format_msg>")	@endcode
	@param	level	-	the	published	message	level
	@param	format_msg	-	the	published	message	C	format	message	string
	@return	always	0
	*/
extern	"C"	int	message_publish(int	level,	const	char	*	format_msg,	...)	{

				char	msg_buf[MAX_MSG_SIZE];
				va_list	args;

				va_start(args,	format_msg);
				(void)	vsnprintf(msg_buf,	MAX_MSG_SIZE,	format_msg,	args);
				va_end(args);

				if	(the_message_publisher	!=	NULL)	{
								the_message_publisher->publish(level,	msg_buf)	;
				}	else	{
								//	if	we	don't	have	a	message	publisher,	simply	print	message	to	terminal
								message_publish_standalone(level,	msg_buf)	;
				}

				return	(0);
}

extern	"C"	int	vmessage_publish(int	level,	const	char	*	format_msg,	va_list	args)	{

				char	msg_buf[MAX_MSG_SIZE];

				(void)	vsnprintf(msg_buf,	MAX_MSG_SIZE,	format_msg,	args);

				if	(the_message_publisher	!=	NULL)	{
								the_message_publisher->publish(level,	msg_buf)	;
				}	else	{
								//	if	we	don't	have	a	message	publisher,	simply	print	message	to	terminal
								message_publish_standalone(level,	msg_buf)	;
				}

				return	(0);
}
/**
	@relates	Trick::MessagePublisher
	@userdesc	A	send	health	&	status	routine	provided	for	backwards	compatibility;	simply	calls	message_publish	with	level	0.
	@par	Python	Usage:
	@code	trick.send_hs(None,	"<format_msg>")	@endcode
	@param	fp	-	the	file	which	the	published	message	goes	to	(this	is	ignored,	message	goes	to	all	subscribers)
	@param	format_msg	-	the	published	message	C	format	message	string
	@return	always	0
	*/
extern	"C"	int	send_hs(FILE	*	fp	__attribute__	((unused)),	const	char	*	format_msg,	...)	{

				char	msg_buf[MAX_MSG_SIZE];
				va_list	args;

				va_start(args,	format_msg);
				(void)	vsnprintf(msg_buf,	MAX_MSG_SIZE,	format_msg,	args);
				va_end(args);

				message_publish(0,	msg_buf)	;

				return	(0);
}

extern	"C"	int	vsend_hs(FILE	*	fp	__attribute__	((unused)),	const	char	*	format_msg,	va_list	args)	{

				vmessage_publish(0,	format_msg,	args)	;
				return	(0);
}

