
#include	"trick/MessageSubscriber.hh"

Trick::MessageSubscriber::MessageSubscriber()	:
	enabled(true)	,
	color(1)	{}

int	Trick::MessageSubscriber::set_enabled(bool	yes_no)	{
				enabled	=	yes_no	;
				return(0)	;
}

int	Trick::MessageSubscriber::set_color(bool	yes_no)	{
				color	=	yes_no	;
				return(0)	;
}
