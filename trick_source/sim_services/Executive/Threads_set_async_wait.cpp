
#include	"trick/Threads.hh"

int	Trick::Threads::set_async_wait(int	yes_no)	{

				shutdown_wait_async	=	(bool)yes_no	;
				return(0)	;
}
