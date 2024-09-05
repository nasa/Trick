/************************************************************************
PURPOSE:	(Represent	the	state	and	initial	conditions	for	my	server)
**************************************************************************/
#include	<sys/stat.h>	//	for	mkdir()
#include	<unistd.h>			//	for	symlink(),	access()
#include	<stdlib.h>			//	for	getenv()
#include	<dirent.h>			//	for	opendir(),	readdir()
#include	<iostream>
#include	<fstream>
#include	<string.h>
#include	<string>
#include	<sstream>

#include	"trick/MyCivetServer.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/input_processor_proto.h"
#include	"trick/exec_proto.h"
#include	"simpleJSON.hh"
#include	"VariableServerSession.hh"
#include	"trick/WebSocketSession.hh"

#ifndef	SWIG
#include	"CivetServer.h"
#endif

#include	"../include/http_GET_handlers.hh"

void	MyCivetServer::deleteWebSocketSession(struct	mg_connection	*	nc)	{
				std::map<mg_connection*,	WebSocketSession*>::iterator	iter;
				pthread_mutex_lock(&WebSocketSessionMapLock);
				iter	=	webSocketSessionMap.find(nc);
				if	(iter	!=	webSocketSessionMap.end())	{
								WebSocketSession*	session	=	iter->second;
								delete	session;
								webSocketSessionMap.erase(iter);
				}
				pthread_mutex_unlock(&WebSocketSessionMapLock);
}

static	const	char	*	style_css	=
				"h1	{"
								"font-family:	fantasy,	cursive,	serif;"
								"font-size:	32px;"
								"margin-left:	1em;"
				"}"
				"h2	{"
								"font-family:	sans-serif;"
								"font-size:	18px;"
								"margin-left:	1em;"
				"}"
				"a	{"
								"font-family:	sans-serif;"
								"font-size:	16px;"
				"}"
				"div.header	{	background-image:	linear-gradient(#afafff,	white);	}";

static	const	char	*	index_html	=
				"<!DOCTYPE	html>\n"
				"<html>\n"
				"<head>\n"
								"<link	rel=\"stylesheet\"	type=\"text/css\"	href=\"style.css\">\n"
								"<title>Trick	Simulation</title>\n"
								"<div	class=\"header\">\n"
												"<table>\n"
																"<th><img	src=\"images/trick_icon.png\"	height=\"64\"	width=\"64\"></th>\n"
																"<th><h1>Trick	Simulation</h1></th>\n"
												"</table>\n"
								"</div>\n"
				"</head>\n"
				"<body>\n"
				"<div	style=\"background:#efefef\">\n"
						"<ul>\n"
						"<li><a	href=\"http://github.com/nasa/trick\">Trick	on	GitHub</a></li>\n"
						"<li><a	href=\"http://github.com/nasa/trick/wiki/Tutorial\">Trick	Tutorial</a></li>\n"
						"<li><a	href=\"http://github.com/nasa/trick/wiki/Documentation-Home\">Trick	Documentation</a></li>\n"
						"</ul>\n"
				"</div>\n"
				"<div	style=\"background:#efefef\">\n"
						"<ul>\n"
						"<li><a	href=\"/apps\">Applications</a></li>\n"
						"</ul>\n"
				"</div>\n"
				"</body>\n"
				"</html>";

static	int	confirmDocumentRoot	(	std::string	documentRoot	)	{

				if	(	access(	documentRoot.c_str(),	F_OK	)	!=	-1	)	{
								message_publish(MSG_INFO,	"Trick	Webserver:	Document	root	\"%s\"	exists.\n",	documentRoot.c_str());
				}	else	{
								message_publish(MSG_INFO,	"Trick	Webserver:	Document	root	\"%s\"	doesn't	exist,	so	we'll	create	it.\n",	documentRoot.c_str());

					char*	trick_home	=	getenv("TRICK_HOME");
								std::string	trickHome	=	std::string(trick_home);

								if	(trick_home	!=	NULL)	{
												if	(	mkdir(	documentRoot.c_str(),	0700)	==	0)	{

																	std::string	styleFilePath	=	documentRoot	+	"/style.css";
																	std::fstream	style_fs	(styleFilePath,	std::fstream::out);
																	style_fs	<<	style_css	<<	std::endl;
																	style_fs.close();

																	std::string	appsDirPath	=	documentRoot	+	"/apps";
																	if	(	mkdir(	appsDirPath.c_str(),	0700)	==	0)	{
																					DIR	*dr;
																					struct	dirent	*	dir_entry;
																					std::string	trickAppsDirPath	=	trickHome	+	"/trick_source/web/apps";
																					if	(	(dr	=	opendir(trickAppsDirPath.c_str()))	!=	NULL)	{
																									while	((	dir_entry	=	readdir(dr))	!=	NULL)	{
																													std::string	fName	=	std::string(	dir_entry->d_name);
																													std::string	sPath	=	trickAppsDirPath	+	'/'	+	fName;
																													std::string	dPath	=	appsDirPath	+	'/'	+	fName;
																													symlink(sPath.c_str(),	dPath.c_str());
																									}
																					}
																	}	else	{
																					message_publish(MSG_ERROR,	"Trick	Webserver:	Failed	to	create	\"%s\".\n",	appsDirPath.c_str());
																					return	1;
																	}

																	std::string	imagesDirPath	=	documentRoot	+	"/images";
																	if	(	mkdir(	imagesDirPath.c_str(),	0700)	==	0)	{
																					DIR	*dr;
																					struct	dirent	*	dir_entry;
																					std::string	trickImagesDirPath	=	trickHome	+	"/trick_source/web/images";
																					if	(	(dr	=	opendir(trickImagesDirPath.c_str()))	!=	NULL)	{
																									while	((	dir_entry	=	readdir(dr))	!=	NULL)	{
																													std::string	fName	=	std::string(	dir_entry->d_name);
																													std::string	sPath	=	trickImagesDirPath	+	'/'	+	fName;
																													std::string	dPath	=	imagesDirPath	+	'/'	+	fName;
																													symlink(sPath.c_str(),	dPath.c_str());
																									}
																					}
																	}	else	{
																					message_publish(MSG_ERROR,	"Trick	Webserver:	Failed	to	create	\"%s\".\n",	imagesDirPath.c_str());
																					return	1;
																	}

																	std::string	indexFilePath	=	documentRoot	+	"/index.html";
																	std::fstream	index_fs	(indexFilePath,	std::fstream::out);
																	index_fs	<<	index_html	<<	std::endl;
																	index_fs.close();

													}	else	{
																	message_publish(MSG_ERROR,	"Trick	Webserver:	Failed	to	create	\"%s\".\n",	documentRoot.c_str());
																	return	1;
													}
								}	else	{
													message_publish(MSG_ERROR,	"Trick	Webserver:	TRICK_HOME	is	not	set.\n");
													return	1;
								}
				}
				return	0;
}

WebSocketSession*	MyCivetServer::makeWebSocketSession(mg_connection	*nc,	std::string	name)	{
				std::map<std::string,	WebSocketSessionMaker>::iterator	iter;
				iter	=	WebSocketSessionMakerMap.find(name);
				if	(iter	!=	WebSocketSessionMakerMap.end())	{
								WebSocketSessionMaker	maker	=	iter->second;
								return	maker(nc);
				}	else	{
								return	NULL;
				}
}

int	MyCivetServer::default_data()	{
				port	=	8888;
				enable	=	false;
				debug	=	false;
	sessionDataMarshalled	=	false;
				time_homogeneous	=	false;
				document_root	=	"www";
				shutting_down	=	false;
				path_to_ssl_cert	=	"~/.ssl/server.pem";	//TODO:Make	a	better	default	path
				ssl_enable	=	false;
				error_log_file	=	"civet_server_error.log";

				installWebSocketSessionMaker("VariableServer",	makeVariableServerSession);
				installHTTPGEThandler("vs_connections",	handle_HTTP_GET_vs_connections);
				installHTTPGEThandler("alloc_info",	handle_HTTP_GET_alloc_info);
				
				return	0;
}

void	MyCivetServer::installHTTPGEThandler(std::string	handlerName,	httpMethodHandler	handler)	{
				pthread_mutex_lock(&httpGETHandlerMapLock);
				httpGETHandlerMap.insert(std::pair<std::string,	httpMethodHandler>(handlerName,	handler));
				pthread_mutex_unlock(&httpGETHandlerMapLock);
}

void	MyCivetServer::installWebSocketSessionMaker(std::string	name,	WebSocketSessionMaker	maker)	{
				pthread_mutex_lock(&WebSocketSessionMakerMapLock);
				WebSocketSessionMakerMap.insert(std::pair<std::string,	WebSocketSessionMaker>(name,	maker));
				pthread_mutex_unlock(&WebSocketSessionMakerMapLock);
}

void	MyCivetServer::addWebSocketSession(struct	mg_connection	*nc,	WebSocketSession*	session)	{
				pthread_mutex_lock(&WebSocketSessionMapLock);
				webSocketSessionMap.insert(	std::pair<mg_connection*,	WebSocketSession*>(nc,	session)	);
				pthread_mutex_unlock(&WebSocketSessionMapLock);
}

void*	main_loop(void*	S)	{
	MyCivetServer*	server	=	(MyCivetServer*)	S;
	bool	messageSent;


	while(1)	{
								pthread_mutex_lock(&server->lock_loop);
								while	(!server->service_connections)
												pthread_cond_wait(&server->cond_loop,	&server->lock_loop);
								if	(server->shutting_down)	{
												return	NULL;
								}
		if	(!server->sessionDataMarshalled)	{
			server->marshallWebSocketSessionData();
		}

		std::map<mg_connection*,	WebSocketSession*>::iterator	iter;
		messageSent	=	false;
		pthread_mutex_lock(&server->WebSocketSessionMapLock);
		for	(iter	=	server->webSocketSessionMap.begin();	iter	!=	server->webSocketSessionMap.end();	iter++	)	{
			WebSocketSession*	session	=	iter->second;
			session->sendMessage();
			messageSent	=	true;
		}
		if	(messageSent)	{	//If	any		message	was	sent	we	say	the	data	is	now	not	marshalled.
			server->sessionDataMarshalled	=	false;
		}
		pthread_mutex_unlock(&server->WebSocketSessionMapLock);
								server->service_connections	=	false;
								pthread_mutex_unlock(&server->lock_loop);
	}
}


int	MyCivetServer::init()	{
				if	(enable)	{
								if	(debug)	{	message_publish(MSG_DEBUG,	"Trick	Webserver:	Debug	logging	is	enabled.\n");	}

								//Setting	up	server
								confirmDocumentRoot(	std::string(document_root)	);
								mg_init_library(0);

								struct	mg_callbacks	callbacks;
								memset(&callbacks,	0,	sizeof(callbacks));
								//Add	callback	functions	here

								std::string	port_str;	
								if	(ssl_enable)	{
												port_str	=	std::to_string(port)	+	"s";	//s	at	the	end	of	the	port	specifies	ssl.
												message_publish(MSG_INFO,	"Trick	Webserver:	SSL	is	enabled\n");
												message_publish(MSG_INFO,	"Trick	Webserver:	Current	path	to	ssl	certificate	is	%s.	To	change	this	put	\"web.server.path_to_ssl_cert	=	\'/path/to/cert\'\"	in	your	input	file.\n",	path_to_ssl_cert);
								}	else	{
												message_publish(MSG_INFO,	"Trick	Webserver:	SSL	is	not	enabled.	To	enable	put	\"web.server.ssl_enable	=	True\"	in	your	input	file.\n");
												port_str	=	std::to_string(port);
								}
								const	char*	options[]	=	{
												"listening_ports",	port_str.c_str(),	"ssl_certificate",	path_to_ssl_cert,	"document_root",	document_root,	"enable_directory_listing",	"yes"
																,	"error_log_file",	error_log_file
																,	0
								};

								if	(debug)	{
												message_publish(MSG_DEBUG,	"Trick	Webserver:	Starting	webserver	with	the	following	options:\n");
												for	(int	i=0;	options[i]	!=	0;	i+=2)	{
																message_publish(MSG_DEBUG,	"Trick	Webserver:	\t%s	=	%s\n",	options[i],	options[i+1]);
												}
								}

								ctx	=	mg_start(&callbacks,	0,	options);
								if	(ctx	==	NULL)	{
												message_publish(MSG_ERROR,	"Trick	Webserver:	Failed	to	create	listener,	exiting	Simulation.\n"
																																"Perhaps	another	program	is	already	using	port	%i.		See	%s	file	for	more	information.\n",	port,	error_log_file);
												exit(-1);
								}	else	{
												message_publish(MSG_INFO,	"Trick	Webserver:	Listening	on	port.	%i\n",	port);
												message_publish(MSG_INFO,	"Trick	Webserver:	Document	root	=	\"%s.\"\n",	document_root);
								}

								//Assigning	general	handlers.
								mg_set_request_handler(ctx,	"/api/http",	parent_http_handler,	(void*)this);
								mg_set_websocket_handler(ctx,	"/api/ws",	ws_connect_handler,	ws_ready_handler,	ws_data_handler,	ws_close_handler,	(void*)this);

								//Starting	the	main	loop
								int	rc;
								rc	=	pthread_create(&server_thread,	NULL,	main_loop,	(void*)this);
								if	(rc)	{
												message_publish(MSG_ERROR,	"Trick	Webserver:	Failed	to	create	main	loop.		Web	socket	connections	will	not	work.");
												exit(-1);
								}
				}	else	{
								message_publish(MSG_INFO,	"Trick	Webserver:	DISABLED.	To	enable,	add	"
																																		"\"web.server.enable	=	True\"	to	your	input	file.\n");
				}
				return	0;
}

std::vector<std::string>	split(std::string	s,	std::string	delim)	{
				std::vector<std::string>	values;
				auto	start	=	0;
				auto	end	=	s.find(delim);
				while	(end	!=	std::string::npos)
				{
								values.push_back(s.substr(start,	end	-	start));
								start	=	end	+	delim.length();
								end	=	s.find(delim,	start);
				}
				values.push_back(s.substr(start,	end	-	start));
				return	values;
}

int	MyCivetServer::http_top_of_frame()	{
				if	(ctx	!=	NULL)	{
								if	(time_homogeneous)	{
						marshallWebSocketSessionData();
								}
		unlockConnections();
	}
				return	0;
}

void	MyCivetServer::unlockConnections()	{
				pthread_mutex_lock(&lock_loop);
				service_connections	=	true;
				pthread_cond_signal(&cond_loop);
				pthread_mutex_unlock(&lock_loop);
}

void	MyCivetServer::sendWebSocketSessionMessages(struct	mg_connection	*nc)	{

				std::map<mg_connection*,	WebSocketSession*>::iterator	iter;
				pthread_mutex_lock(&WebSocketSessionMapLock);
				iter	=	webSocketSessionMap.find(nc);
				if	(iter	!=	webSocketSessionMap.end())	{
								WebSocketSession*	session	=	iter->second;
								session->sendMessage();
				}
				sessionDataMarshalled	=	false;
				pthread_mutex_unlock(&WebSocketSessionMapLock);
}

void	MyCivetServer::marshallWebSocketSessionData()	{
				std::map<mg_connection*,	WebSocketSession*>::iterator	iter;
				pthread_mutex_lock(&WebSocketSessionMapLock);
				for	(iter	=	webSocketSessionMap.begin();	iter	!=	webSocketSessionMap.end();	iter++	)	{
								WebSocketSession*	session	=	iter->second;
								session->marshallData();
				}
				sessionDataMarshalled	=	true;
				pthread_mutex_unlock(&WebSocketSessionMapLock);
}

int	MyCivetServer::shutdown()	{
				if	(enable)	{
								message_publish(MSG_INFO,"Trick	Webserver:	Shutting	down	on	port	%i.\n",	port);
								shutting_down	=	true;
								unlockConnections();
								join();
		mg_stop(ctx);
		mg_exit_library();
				}
				return	0;
}

int	MyCivetServer::join()	{
				pthread_join(server_thread,	NULL);
				return	0;
}

void	MyCivetServer::handleWebSocketClientMessage(struct	mg_connection	*conn,	const	std::string&	data)	{
				std::map<mg_connection*,	WebSocketSession*>::iterator	iter;
				iter	=	webSocketSessionMap.find(conn);
				if	(iter	!=	webSocketSessionMap.end())	{
								WebSocketSession*	session	=	iter->second;
								session->handleMessage(data);
				}
}

void	MyCivetServer::handleHTTPGETrequest(struct	mg_connection	*conn,	const	struct	mg_request_info*	ri,	std::string	handlerName)	{
								std::map<std::string,	httpMethodHandler>::iterator	iter;
								iter	=	httpGETHandlerMap.find(handlerName);
		if	(iter	!=	httpGETHandlerMap.end())	{
			httpMethodHandler	handler	=	iter->second;
			handler(conn,	(void*)this);
		}	else	{
												std::stringstream	ss;
												ss	<<	"Error:	http	api	"	<<	handlerName	<<	"	is	not	implemented.";
												http_send_error(conn,	404,	ss.str().c_str(),	ss.str().size(),	100);
								}
}
