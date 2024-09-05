
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"

int	Trick::Executive::clear_scheduled_queues()	{

				unsigned	int	ii	;

				/**	@par	Detailed	Design	*/
				/**	@li	Clear	all	non-scheduled	job	queues	*/
				default_data_queue.clear()	;
				input_processor_queue.clear()	;
				initialization_queue.clear()	;
				input_processor_run_queue.clear()	;
				scheduled_queue.clear()	;
				top_of_frame_queue.clear()	;
				end_of_frame_queue.clear()	;
				shutdown_queue.clear()	;
				freeze_init_queue.clear()	;
				freeze_scheduled_queue.clear()	;
				freeze_queue.clear()	;
				unfreeze_queue.clear()	;
				time_tic_changed_queue.clear()	;

				/**	@li	Clear	all	thread	job	queues	*/
				for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
								threads[ii]->clear_scheduled_queues()	;
				}

				return(0)	;

}
