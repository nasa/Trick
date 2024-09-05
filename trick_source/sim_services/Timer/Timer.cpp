/*
PURPOSE:
				(	Base	Timer	class	)
PROGRAMMERS:
					((Alex	Lin)	(NASA)	(August	2010)	(--)	(c++	port)))
*/

#include	"trick/Timer.hh"

Trick::Timer::Timer()	:	enabled(false)	,	active(false)	{}

int	Trick::Timer::enable()	{
				enabled	=	true	;
				return(0)	;
}

int	Trick::Timer::disable()	{
				enabled	=	false	;
				return(0)	;
}

bool	Trick::Timer::get_enabled()	{
				return(enabled)	;
}

void	Trick::Timer::set_active(bool	in_active)	{
				active	=	in_active	;
}
