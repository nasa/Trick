
#include	<iostream>
#include	<sstream>
#include	<stdlib.h>
#include	<string.h>

#include	"trick/JSONVariableServerThread.hh"
#include	"trick/MemoryManager.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.h"
#include	"trick/PythonPrint.hh"
#include	"trick/tc_proto.h"
#include	"trick/TrickConstant.hh"

Trick::JSONVariableServerThread::JSONVariableServerThread(TCDevice	*	listen_dev)	:
	Trick::ThreadBase("JSONVarServer")	{

				connection.disable_handshaking	=	TC_COMM_TRUE	;
				connection.blockio_type	=	TC_COMM_BLOCKIO	;

				tc_accept(listen_dev,	&connection);

				//	Save	the	hostname	and	port	of	the	listen	server
				hostname	=	listen_dev->hostname	;
				port	=	listen_dev->port	;

				incoming_msg	=	new	char[MAX_CMD_LEN]	;
}

/*
@details
-#	delete	the	space	for	incominig_msg.
-#	These	classes	are	always	allocated...	free	the	memory	on	the	way	out.
*/
Trick::JSONVariableServerThread::~JSONVariableServerThread()	{
				delete	[]	incoming_msg	;
}

/*
@details
-#	Loop	until	the	connection	is	broken	or	timed_out
	-#	Call	select	to	wait	up	to	15	seconds	for	a	message.
	-#	If	a	message	is	found,	read	it	into	the	incoming_msg	buffer
	-#	Call	parse_request	on	the	incoming_msg	buffer
-#	Disconnect	the	socket	connection
*/
void	*	Trick::JSONVariableServerThread::thread_body()	{

				int	nbytes	=	-1	;
				socklen_t	sock_size	=	sizeof(connection.remoteServAddr)	;
				fd_set	rfds;
				struct	timeval	timeout_time	=	{	15,	0	};
				bool	timed_out	=	false	;

				while	(!	timed_out	and	nbytes	!=	0	)	{
								FD_ZERO(&rfds);
								FD_SET(connection.socket,	&rfds);
								timeout_time.tv_sec	=	15	;
								select(connection.socket	+	1,	&rfds,	NULL,	NULL,	&timeout_time);

								if	(	FD_ISSET(connection.socket,	&rfds))	{
												nbytes	=	recvfrom(	connection.socket,	incoming_msg,	MAX_CMD_LEN,	0	,
																					(struct	sockaddr	*)&connection.remoteServAddr,	&sock_size	)	;
												//std::cout	<<	"nbytes	=	"	<<	nbytes	<<	std::endl	;
												if	(nbytes	!=	0	)	{
																//std::cout	<<	incoming_msg	<<	std::endl	;
																parse_request()	;
												}
								}	else	{
												timed_out	=	true	;
								}
				}
				tc_disconnect(&connection)	;
				//std::cout	<<	"closed	variable	server	connection"	<<	std::endl	;

				return	NULL	;
}

/*
@details
-#	scan	the	incoming	message	for	the	command	and	requested	path
-#	start	the	return	message	with	a	"{"
-#	if	both	a	command	and	request	is	present
	-#	if	the	command	is	"GET"
		-#	if	the	path	is	"/"	call	get_top_page
		-#	else	if	the	path	starts	with	"/vars"	call	get_vars
		-#	else	if	the	path	starts	with	"/commands"	call	get_commands
		-#	else	create	an	error	message	reply
	-#	else	if	the	command	is	"POST"	do	something	at	some	point.
	-#	else	create	an	error	message	reply
-#	else	create	an	error	message	reply
-#	close	the	return	message	with	a	"}"
-#	create	the	http	header	for	the	reply	message
-#	call	tc_write	to	send	the	reply	message
*/
void	Trick::JSONVariableServerThread::parse_request()	{
				//	Need	to	protect	against	buffer	overflow
				char	command[32]	;
				char	path[512]	;
				std::stringstream	ss	;
				std::stringstream	body	;
				int	ret	;

				ret	=	sscanf(incoming_msg,	"%s	%s",	command	,	path)	;

				body	<<	"{"	<<	std::endl	;
				if	(	ret	==	2	)	{
								//std::cout	<<	"command	=	"	<<	command	<<	std::endl	;
								//std::cout	<<	"path	=	"	<<	path	<<	std::endl	;

								if	(	!	strcmp(command,	"GET")	)	{
												if	(	!	strcmp(path,	"/")	)	{
																//	send	the	top	page
																ret	=	get_top_page(body)	;
												}	else	if	(	!	strncmp(path,	"/vars",	5))	{
																//	get	list	of	variables	or	value
																ret	=	get_vars(body,	&path[5])	;
												}	else	if	(	!	strncmp(path,	"/commands",	9))	{
																//	get	list	of	commands
																ret	=	get_commands(body,	&path[9])	;
												}	else	{
																ret	=	404	;
																body	<<	"				\"name\"	:	\""	<<	path	<<	"\"	,"	<<	std::endl	;
																body	<<	"				\"message\"	:	\"Not	Found\""	<<	std::endl	;
												}
								}	else	if	(	!	strcmp(	command,	"POST")	)	{
												//TODO:	Allow	setting	variable	values	with	POST.
												ret	=	405	;
												body	<<	"				\"verb\"	:	\""	<<	command	<<	"\"	,"	<<	std::endl	;
												body	<<	"				\"message\"	:	\"Method	not	allowed\""	<<	std::endl	;
								}	else	{
												ret	=	405	;
												body	<<	"				\"verb\"	:	\""	<<	command	<<	"\"	,"	<<	std::endl	;
												body	<<	"				\"message\"	:	\"Method	not	allowed\""	<<	std::endl	;
								}

				}	else	{
								ret	=	400	;
								body	<<	"				\"message\"	:	\"Bad	Request\""	<<	std::endl	;
				}
				body	<<	"}"	<<	std::endl	;

				ss	<<	"HTTP/1.1	"	<<	ret	;
				switch	(ret)	{
								case	200:
												ss	<<	"	OK"	;
												break	;
								case	400:
												ss	<<	"	Bad	Request"	;
												break	;
								case	404:
												ss	<<	"	Not	Found"	;
												break	;
								case	405:
												ss	<<	"	Method	Not	Allowed"	;
												break	;
								default:
												ss	<<	"	Unknown"	;
												break	;
				}
				ss	<<	std::endl	;
				ss	<<	"Content-Type:	application/json"	<<	std::endl	;
				ss	<<	"Content-Length:	"	<<	body.str().size()	<<	std::endl	<<	std::endl	;
				ss	<<	body.str()	;
				//std::cout	<<	ss.str()	;
				tc_write(&connection,	(char	*)ss.str().c_str()	,	ss.str().size())	;
}

/*
@details
-#	create	top	page	contents	and	return	success
*/
int	Trick::JSONVariableServerThread::get_top_page(	std::stringstream	&	body	)	{
				body	<<	"			\"commands\":	\"http://"	<<	hostname	<<	":"	<<	port	<<	"/commands\","	<<	std::endl	;
				body	<<	"			\"vars\":	\"http://"	<<	hostname	<<	":"	<<	port	<<	"/vars\""	<<	std::endl	;
				return	200	;
}

/*
@details
-#	if	the	path	is	empty	after	/commands	create	commands	top	level	page.
-#	else	if	the	path	is	exec_get_sim_time	return	create	a	message	with	the	sim	time.
-#	else	create	an	error	message.
*/
int	Trick::JSONVariableServerThread::get_commands(	std::stringstream	&	body	,	char	*	path	)	{
				int	ret	=	200	;
				if	(	path[strlen(path)	-	1]	==	'/')	{
								path[strlen(path)	-	1]	=	'\0'	;
				}
				if	(	path[0]	==	'\0'	)	{
								body	<<	"			\"exec_get_sim_time\":	\"exec_get_sim_time()\""	<<	std::endl	;
				}	else	if	(	!	strcmp(	path	,	"/exec_get_sim_time"	)	)	{
								body	<<	"			\"sim_time\":	\""	<<	exec_get_sim_time()	<<	"\""	<<	std::endl	;
				}	else	{
								body	<<	"				\"message\"	:	\"Not	Found\""	<<	std::endl	;
								ret	=	404	;
				}
				return	ret	;
}

/*
@details
-#	if	the	path	is	empty	after	/vars
	-#	loop	through	all	allocations	in	the	memory	manager
	-#	if	the	allocation	has	a	name	create	an	item	in	the	reply	message	for	that	allocation.
-#	else
	-#	convert	"/"	characters	to	"."	in	the	path	and	strip	trailing	"/"	characters.
	-#	add	converted	variable	name	to	the	reply	message.
	-#	call	ref_attributes	to	find	the	variable.
		-#	if	the	variable	exists
			-#	if	the	variable	type	is	a	structure.
				-#	loop	through	the	attributes	and	add	each	of	the	struct/class	variables	as	links	in	the	reply	message.
			-#	else	add	the	variables	value	and	units	to	the	reply	message.
			-#	add	the	description	text	to	the	reply	message
			-#	add	the	type	information	to	the	reply	message
			-#	add	the	type	io	specification	to	the	reply	message
			-#	if	the	variable	has	dimensions	add	the	dimension	information	to	the	reply	message.
	-#	else	add	an	error	message	to	the	reply	message
*/
int	Trick::JSONVariableServerThread::get_vars(	std::stringstream	&	body	,	char	*	path	)	{

				int	ret	=	200	;

				//std::cout	<<	"get_vars	"	<<	path	<<	std::endl	;
				if	(	path[0]	==	'\0'	)	{
								Trick::ALLOC_INFO_MAP_ITER	aim_it	;
								bool	first_item	=	true	;
								for	(	aim_it	=	trick_MM->alloc_info_map_begin()	;	aim_it	!=	trick_MM->alloc_info_map_end()	;	++aim_it	)	{
												ALLOC_INFO	*	alloc_info	=	(*aim_it).second	;
												if	(	alloc_info->name	!=	NULL	and	alloc_info->user_type_name	!=	NULL	)	{
																if	(	first_item	==	true	)	{
																				first_item	=	false	;
																}	else	{
																				body	<<	"	,"	<<	std::endl	;
																}
																body	<<	"				\""	<<	alloc_info->name	<<	"\"	:	\""	<<	alloc_info->user_type_name	<<	"\""	;
												}
								}
								body	<<	std::endl	;
				}	else	{

								//TODO:	More	path	character	translations	for	characters	like	"[]"

								if	(	path[strlen(path)	-	1]	==	'/')	{
												path[strlen(path)	-	1]	=	'\0'	;
								}
								for	(	unsigned	int	ii	=	1	;	ii	<	strlen(path)	;	ii++	)	{
												if	(	path[ii]	==	'/')	{
																path[ii]	=	'.'	;
												}
								}

								body	<<	"				\"name\"	:	\""	<<	&path[1]	<<	"\"	,"	<<	std::endl	;
								//std::cout	<<	"getting	var	"	<<	path	<<	std::endl	;
								//	skip	leading	"/"
								REF2	*	ref	=	ref_attributes(	&path[1]	)	;
								if	(	ref	!=	NULL	and	ref->attr	!=	NULL	)	{
												ret	=	200	;
												//std::cout	<<	"num_index	"	<<	ref->num_index	<<	std::endl	;
												if	(	ref->attr->type	==	TRICK_STRUCTURED	)	{
																ATTRIBUTES	*	attr	=	(ATTRIBUTES	*)ref->attr->attr	;
																body	<<	"				\"links\"	:	["	<<	std::endl	;
																for	(	int	ii	=	0	;	attr[ii].name[0]	!=	'\0'	;	ii++	)	{
																				if	(	ii	!=	0	)	{
																								body	<<	"	,"	<<	std::endl	;
																				}
																				body	<<	"																\""	<<	attr[ii].name	<<	"\""	;
																}
																body	<<	std::endl	<<	"														]	,"	<<	std::endl	;
												}	else	{
																body	<<	"				\"value\"	:	"	;
																Trick::PythonPrint::write_rvalue(body,	ref->address,	ref->attr,	ref->num_index,	0,	false,	true)	;
																body	<<	"	,"	<<	std::endl	;
																if	(	ref->attr->units	!=	NULL	)	{
																				body	<<	"				\"units\"	:	\""	<<	ref->attr->units	<<	"\"	,"	<<	std::endl	;
																}
												}
												if	(	ref->attr->des	!=	NULL	)	{
																body	<<	"				\"description\"	:	\""	<<	ref->attr->des	<<	"\"	,"	<<	std::endl	;
												}
												body	<<	"				\"type_name\"	:	"	;
												if	(	ref->attr->type_name	!=	NULL	)	{
																body	<<	"\""	<<	ref->attr->type_name	<<	"\"	,"	<<	std::endl	;
												}	else	{
																body	<<	"\"(null)\"	,"	<<	std::endl	;
												}
												body	<<	"				\"type\"	:	"	<<	ref->attr->type	<<	"	,"	<<	std::endl	;
												body	<<	"				\"io\"	:	"	<<	ref->attr->io	;
												if	(	(ref->attr->num_index	-	ref->num_index)	>	0	)	{
																body	<<	"	,"	<<	std::endl	;
																body	<<	"				\"num_index\"	:	"	<<	ref->attr->num_index	-	ref->num_index	<<	"	,"	<<	std::endl	;
																body	<<	"				\"index\"	:	["	<<	std::endl	;
																for	(	int	ii	=	ref->num_index	,	jj	=	0	;	ii	<	ref->attr->num_index	;	ii++	,	jj++	)	{
																				if	(	jj	!=	0	)	{
																								body	<<	"	,"	<<	std::endl	;
																				}
																				body	<<	"																{"	<<	std::endl	;
																				body	<<	"																				\"size\"	:	"	<<	ref->attr->index[ii].size	<<	"	,"	<<	std::endl	;
																				body	<<	"																				\"start\"	:	"	<<	ref->attr->index[ii].start	<<	std::endl	;
																				body	<<	"																}"	;
																}
																body	<<	std::endl	<<	"														]"	<<	std::endl	;
												}	else	{
																body	<<	std::endl	;
												}
												free(ref)	;
								}	else	{
												body	<<	"				\"message\"	:	\"Not	Found\""	<<	std::endl	;
												ret	=	404	;
								}
				}
				return	ret	;
}

