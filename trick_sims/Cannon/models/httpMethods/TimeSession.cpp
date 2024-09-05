#include	<stdio.h>
#include	<time.h>
#include	<iostream>
#include	"TimeSession.hh"
#include	<cstring>

//	CONSTRUCTOR
TimeSession::TimeSession(	struct	mg_connection	*nc	)	:	WebSocketSession(nc)	{
				time(&now);
}

//	DESTRUCTOR
TimeSession::~TimeSession()	{}

void	TimeSession::marshallData()	{
				time(&now);
}

void	TimeSession::sendMessage()	{

				char	message[1024];
				struct	tm	*theTime;
				if	(zone	==	TimeSession::LOCAL)	{
								theTime	=	localtime(&now);
				}	else	{
								theTime	=	gmtime(&now);
				}
				int	hours			=	theTime->tm_hour;
				int	minutes	=	theTime->tm_min;
				int	seconds	=	theTime->tm_sec;
				int	day					=	theTime->tm_mday;
				int	month			=	theTime->tm_mon	+	1;
				int	year				=	theTime->tm_year	+	1900;

				snprintf(message,	sizeof(message),	"Time:	%02d:%02d:%02d	Date:	%02d/%02d/%d\n",	hours,	minutes,	seconds,	month,	day,	year);
				mg_websocket_write(connection,	MG_WEBSOCKET_OPCODE_TEXT,	message,	strlen(message));
}

int	TimeSession::handleMessage(const	std::string&	client_msg)	{

			if	(client_msg.compare("GMT")	==	0)	{
							zone	=	TimeSession::GMT;
			}	else	if	(client_msg.compare("LOCAL")	==	0)	{
							zone	=	TimeSession::LOCAL;
			}	else	{
						std::cerr	<<	"ERROR:	Unknown	command	\""	<<	client_msg	<<	"\"."	<<	std::endl;
			}
			return	0;
}

//	WebSocketSessionMaker	function	for	a	TimeSession.
WebSocketSession*	makeTimeSession(	struct	mg_connection	*nc	)	{
				std::cerr	<<	"DEBUG:	Creating	new	TimeSession."	<<	std::endl;
				return	new	TimeSession(nc);
}

