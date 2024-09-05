
#include	<string.h>
#include	<sstream>

#include	"trick/MessageLCout.hh"

Trick::MessageLCout::MessageLCout()	{
				enabled	=	1	;
				subs['a']	=	"4"	;
				subs['e']	=	"3"	;
				subs['f']	=	"ph"	;
				subs['g']	=	"9"	;
				subs['l']	=	"1"	;
				subs['o']	=	"0"	;
				subs['s']	=	"5"	;
				subs['t']	=	"7"	;

				seed	=	0	;

				name	=	"lcout"	;
}

void	Trick::MessageLCout::update(	unsigned	int	level	,	std::string	header	,	std::string	message	)	{
				if	(!enabled)	{
								return;
				}
				if	(	level	==	1337	)	{
								std::string	in_string	;
								std::string	temp_string	;
								unsigned	int	ii	,	jj	;

								in_string	=	header	+	message	;

								for	(	ii	=	0	;	ii	<	in_string.size()	;	ii++	)	{
												seed	+=	in_string[ii]	*	ii	;
								}
								srandom(seed)	;
								for	(	ii	=	0	;	ii	<	in_string.size()	;	ii++	)	{
												if	(	random()	<	RAND_MAX	*	0.3	)	{
																if	(	subs.find((char)tolower(in_string[ii]))	!=	subs.end())	{
																				//temp_string	+=	subs[(char)tolower(in_string[ii])]	;
																				std::string	replace_str	=	subs[(char)tolower(in_string[ii])]	;
																				for	(	jj	=	0	;	jj	<	replace_str.size()	;	jj++	)	{
																								if	(	random()	<	RAND_MAX	*	0.5	)	{
																												temp_string	+=	toupper(replace_str[jj])	;
																								}	else	{
																												temp_string	+=	tolower(replace_str[jj])	;
																								}
																				}
																}	else	{
																				temp_string	+=	in_string[ii]	;
																}
												}	else	{
																if	(	random()	<	RAND_MAX	*	0.3	)	{
																				temp_string	+=	toupper(in_string[ii])	;
																}	else	{
																				temp_string	+=	tolower(in_string[ii])	;
																}
												}
								}
								std::cout	<<	temp_string	<<	std::flush	;
				}	else	if	(	level	<	100	)	{
								//	Building	the	final	string	in	a	temporary	stream	ensures	an	automic	call	to	cout,	which	prevents
								//	multithreaded	sims	from	interleaving	header	and	message	elements.
								std::ostringstream	oss;
								oss	<<	header	<<	message	;
								std::cout	<<	oss.str()	<<	std::flush	;
				}
}

