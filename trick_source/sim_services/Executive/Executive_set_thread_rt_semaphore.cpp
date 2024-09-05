
#include	"trick/Executive.hh"
#include	"trick/message_proto.h"

int	Trick::Executive::set_thread_rt_semaphores(unsigned	int	thread_id	,	bool	yes_no)	{
				(void)thread_id	;
				(void)yes_no	;
				message_publish(MSG_WARNING,	"set_thread_rt_semaphores	has	been	deprecated\n"
					"Use	exec_get_thread(thread_id).set_trigger_type(Trick::ThreadTriggerType	tt_type)")	;
				return	-1	;
}

