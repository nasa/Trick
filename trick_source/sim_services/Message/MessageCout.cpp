
#include	"trick/MessageCout.hh"
#include	"trick/message_type.h"

#include	<sstream>

Trick::MessageCout::MessageCout()	{
				/**	By	default,	this	subscriber	is	enabled	when	it	is	created.	*/
				enabled	=	1	;
				color	=	1	;
				name	=	"cout"	;
}

void	Trick::MessageCout::update(	unsigned	int	level	,	std::string	header	,	std::string	message	)	{

				/**	@par	Design	Details:	*/
				std::string	color_code	;

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

				/**	@li	Prints	the	received	message	to	the	standard	output	stream.	*/
				if	(enabled	&&	level	<	100	)	{

								//	Building	the	final	string	in	a	temporary	stream	ensures	an	automic	call	to	cout,	which	prevents
								//	multithreaded	sims	from	interleaving	header	and	message	elements.
								std::ostringstream	oss;
								if	(	color	)	{
												oss	<<	header	<<	color_code	<<	message	<<	"\033[00m"	;
								}	else	{
												oss	<<	header	<<	message	;
								}
								std::cout	<<	oss.str()	<<	std::flush;
				}
}

