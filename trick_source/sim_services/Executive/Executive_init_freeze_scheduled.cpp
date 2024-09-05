
#include	"trick/Executive.hh"

int	Trick::Executive::init_freeze_scheduled()	{

				freeze_time_tics	=	0	;
				freeze_frame_count	=	0	;
				next_freeze_frame_check_tics	=	freeze_frame_tics	;
				freeze_scheduled_queue.set_next_job_call_time(freeze_time_tics)	;

				Trick::JobData	*	jd	;
				freeze_scheduled_queue.reset_curr_index()	;
				while	(	(jd	=	freeze_scheduled_queue.get_next_job())	!=	NULL	)	{
								jd->next_tics	=	0	;
				}

				return	0	;
}

