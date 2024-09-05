/*************************************************************************
PURPOSE:	(	HTTP-GET-method-handlers	)
LIBRARY	DEPENDENCIES:
				(	(../src/http_GET_handlers.o))
**************************************************************************/

#include	"http_GET_handlers.hh"
#include	"trick/MyCivetServer.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

#ifndef	SWIG
#include	"CivetServer.h"
#endif

#include	<sstream>

#include	"trick/VariableServer.hh"
extern	Trick::VariableServer	*	the_vs	;

#include	"trick/MemoryManager.hh"
extern	Trick::MemoryManager*	trick_MM;

static	const	std::string	ws_api_prefix	=	"/api/ws";
static	const	std::string	http_api_prefix	=	"/api/http";

int	http_send_error(struct	mg_connection	*conn,	int	error_code,	const	char*	msg,	int	len,	int	chunk_size)	{	//TODO:	Make	this	display	correctly
				message_publish(MSG_DEBUG,	"Sending	error	msg:	%s\n",	msg);

				mg_printf(conn,
												"HTTP/1.1	%i	Method	Not	Allowed\r\nConnection:	close\r\n",	error_code);
				mg_printf(conn,	"Content-Type:	text/plain\r\n\r\n");
				http_send(conn,	msg,	len,	chunk_size);
				return	error_code;
}

int	http_send_ok(struct	mg_connection	*conn,	const	char*	msg,	int	len,	int	chunk_size)	{
				message_publish(MSG_DEBUG,	"Sending	ok	msg:	%s\n",	msg);
				mg_printf(conn,
								"HTTP/1.1	200	OK\r\nConnection:	"
								"close\r\nTransfer-Encoding:	chunked\r\n");
				mg_printf(conn,	"Content-Type:	text/plain\r\n\r\n");
				http_send(conn,	msg,	len,	chunk_size);
				return	200;
}

void	http_send(struct	mg_connection	*conn,	const	char*	msg,	int	len,	int	chunk_size)	{
				int	size	=	len;
				int	count	=	0;
				if	(chunk_size	>	size)	{
								chunk_size	=	len;
				}
				while	(size	>	chunk_size)	{
								std::string	buff	=	std::string(msg).substr(count	*	chunk_size,	chunk_size);
								mg_send_chunk(conn,	buff.c_str(),	buff.length());
								count++;
								size	=	size	-	chunk_size;
				}
				std::string	buff	=	std::string(msg).substr(count	*	chunk_size,	chunk_size);
				mg_send_chunk(conn,	buff.c_str(),	buff.length());
				mg_send_chunk(conn,	"",	0);
}

/////	HTTP


int	parent_http_handler(struct	mg_connection*	conn,	void	*data)	{

				MyCivetServer*	server	=	(MyCivetServer*)data;
				const	struct	mg_request_info*	ri	=	mg_get_request_info(conn);
				std::string	uri	=	ri->local_uri_raw;
				if	(server->debug)	{	message_publish(MSG_INFO,	"Trick	Webserver:	HTTP_REQUEST:	URI	=	\"%s\".\n",	uri.c_str());	}
				std::string	httpType	=	"";
				if	(http_api_prefix.size()	<	uri.size())	{
								httpType	=	uri.substr(http_api_prefix.size()	+	1,	uri.size());
				}
				if	(httpType	!=	"")	{
								if	(server->debug)	{	message_publish(MSG_DEBUG,	"HTTP_REQUEST:	METHOD	=	\"%s\"\n",	ri->request_method);	}
								std::string	method	=	std::string(ri->request_method);
								if	(method	==	"GET")	{
												if	(server->debug)	{	message_publish(MSG_DEBUG,	"HTTP_REQUEST:	HANDLER	=	\"%s\"\n",	httpType.c_str());	}
												server->handleHTTPGETrequest(conn,	ri,	httpType);
												return	200;
								}	else	if	(method	==	"PUT")	{
												std::string	msg	=	"PUT	method	not	allowed";
												return	http_send_error(conn,	405,	msg.c_str(),	msg.size(),	100);
								}	else	if	(method	==	"DELETE")	{
												std::string	msg	=	"DELETE	method	not	allowed";
												return	http_send_error(conn,	405,	msg.c_str(),	msg.size(),	100);
								}	else	if	(method	==	"POST")	{
												std::string	msg	=	"POST	method	not	allowed";
												return	http_send_error(conn,	405,	msg.c_str(),	msg.size(),	100);
								}
				}
				std::string	msg	=	"No	endpoint	given.		To	access	the	api	use	the	/api/http/<endpoint>.";
				return	http_send_error(conn,	405,	msg.c_str(),	msg.size(),	100);
}

void	handle_HTTP_GET_vs_connections(struct	mg_connection*	conn,	void	*cbdata)	{
				std::stringstream	ss;
				ss	<<	*the_vs	<<	std::endl;
				std::string	someJSON	=	ss.str();

				http_send_ok(conn,	someJSON.c_str(),	someJSON.length(),	100);
}

void	handle_HTTP_GET_alloc_info(struct	mg_connection	*conn,	void*	ignore)	{
				mg_printf(conn,
											"HTTP/1.1	200	OK\r\nConnection:	"
											"close\r\nTransfer-Encoding:	chunked\r\n");
	mg_printf(conn,	"Content-Type:	text/plain\r\n\r\n");
				const	struct	mg_request_info*	ri	=	mg_get_request_info(conn);
				int	max_size	=	100;
				char	start_str[max_size],	count_str[max_size];

				int	error_code1,	error_code2;
				assert(ri	!=	NULL);
				if	(ri->query_string	!=	NULL)	{
								std::string	data	=	ri->query_string;
								message_publish(MSG_DEBUG,	"query_string	=	%s\n",	data.c_str());
								error_code1	=	mg_get_var(data.c_str(),	strlen(data.c_str()),	"start",	start_str,	max_size);
								error_code2	=	mg_get_var(data.c_str(),	strlen(data.c_str()),	"count",	count_str,	max_size);
				}	else	{
								error_code1	=	-1;
								error_code2	=	-1;
				}
				if	(error_code1	<	0)	{
								message_publish(MSG_WARNING,	"Could	not	find	uri	param:	start.	Error	code:	%i\n",	error_code1);
								strncpy(start_str,	"0",	1);
				}
				if	(error_code2	<	0)	{
								message_publish(MSG_WARNING,	"Could	not	find	uri	param:	count.	Error	code:	%i\n",	error_code2);
								strncpy(count_str,	"10",	2);	//By	default	we	show	10.
				}
				mg_send_http_ok(conn,	"text/plain",	-1);
				int	start	=	strtol(start_str,	NULL,	0);
				int	count	=	strtol(count_str,	NULL,	0);
				std::stringstream	ss;
				trick_MM->write_JSON_alloc_list(ss,	start,	count);
				
				std::string	someJSON	=	ss.str();
				http_send(conn,	someJSON.c_str(),	someJSON.length(),	100);

}

/////	websockets

int	ws_connect_handler(const	struct	mg_connection	*conn,
									void	*ignore)
{
	int	ret_val	=	0;
	return	ret_val;
}

void	ws_ready_handler(struct	mg_connection	*conn,	void	*my_server)
{
	MyCivetServer*	server	=	(MyCivetServer*)	my_server;
				const	struct	mg_request_info*	ri	=	mg_get_request_info(conn);
				std::string	uri	=	ri->local_uri_raw;
				if	(server->debug)	{	message_publish(MSG_INFO,"Trick	Webserver:	WEBSOCKET_REQUEST:	URI	=	\"%s\".\n",	uri.c_str());	}
				std::string	wsType	=	"";
				if	(ws_api_prefix.size()	<	uri.size())	{
								wsType	=	uri.substr(ws_api_prefix.size()	+	1,	uri.size());
				}
				WebSocketSession*	session	=	server->makeWebSocketSession(conn,	wsType);
				if	(session	!=	NULL)	{
								if	(server->debug)	{	message_publish(MSG_INFO,	"Trick	Webserver:	WEBSOCKET[%p]	OPENED.	URI=\"%s\".\n",	(void*)conn,	uri.c_str());	}
								server->addWebSocketSession(conn,	session);
				}	else	{
								message_publish(MSG_ERROR,	"Trick	Webserver:	No	such	web	socket	interface:	\"%s\".\n",	uri.c_str());	
								mg_websocket_write(conn,	MG_WEBSOCKET_OPCODE_CONNECTION_CLOSE,	NULL,	0);
				}
}

int	ws_data_handler(struct	mg_connection	*conn,	int	bits,
						char	*data,	size_t	data_len,	void	*my_server)
{
	int	rvalue	=	1;
				MyCivetServer*	server	=	(MyCivetServer*)	my_server;
				std::string	msg(data,	data_len);
				if	(server->debug)	{	message_publish(MSG_INFO,	"Trick	Webserver:	WEBSOCKET[%p]	RECIEVED:	\"%s\".\n",	(void*)conn,	msg.c_str());	}

	if	(data_len	>	0)	{	
								std::string	msg(data,	data_len);
		server->handleWebSocketClientMessage(conn,	msg);
	}	
					return	rvalue;
}

void	ws_close_handler(const	struct	mg_connection	*conn,
								void	*my_server)
{
	MyCivetServer*	server	=	(MyCivetServer*)	my_server;
	server->deleteWebSocketSession(const_cast<mg_connection*>(conn));
				if	(server->debug)	{	message_publish(MSG_INFO,"Trick	Webserver:	WEBSOCKET[%p]	CLOSED.\n",	(void*)conn);	}
}
