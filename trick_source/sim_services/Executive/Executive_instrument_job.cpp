
#include	<iostream>
#include	<iomanip>

#include	"trick/Executive.hh"

int	Trick::Executive::instrument_job_before(	std::string	in_job	,	unsigned	int	instance	)	{

				Trick::JobData	*	instrument_job	;

				instrument_job	=	get_job(in_job	,	instance)	;
				return	instrument_job_before(	instrument_job	)	;
}

int	Trick::Executive::instrument_job_before(	Trick::JobData	*	instrument_job	)	{

				unsigned	int	ii,	count	;

				count	=	0	;
				if	(	instrument_job	!=	NULL	)	{
								//	instrument	scheduled	jobs
								for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
												count	+=	threads[ii]->job_queue.instrument_before(instrument_job)	;
												count	+=	threads[ii]->top_of_frame_queue.instrument_before(instrument_job)	;
												count	+=	threads[ii]->end_of_frame_queue.instrument_before(instrument_job)	;
								}

								//	instrument	other	queued	jobs
								//	cannot	instrument	default_data	because	they	come	before	input	processor
								count	+=	default_data_queue.instrument_before(instrument_job)	;
								count	+=	initialization_queue.instrument_before(instrument_job)	;
								count	+=	top_of_frame_queue.instrument_before(instrument_job)	;
								count	+=	end_of_frame_queue.instrument_before(instrument_job)	;
								count	+=	shutdown_queue.instrument_before(instrument_job)	;

								count	+=	freeze_init_queue.instrument_before(instrument_job)	;
								count	+=	freeze_scheduled_queue.instrument_before(instrument_job)	;
								count	+=	freeze_queue.instrument_before(instrument_job)	;
								count	+=	unfreeze_queue.instrument_before(instrument_job)	;
								count	+=	time_tic_changed_queue.instrument_before(instrument_job)	;
								count	+=	thread_sync_queue.instrument_before(instrument_job)	;

								for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
												count	+=	other_schedulers[ii]->instrument_job_before(	instrument_job	)	;
								}

				}

				return(count)	;

}

int	Trick::Executive::instrument_job_after(	std::string	in_job	,	unsigned	int	instance	)	{

				Trick::JobData	*	instrument_job	;

				instrument_job	=	get_job(in_job	,	instance)	;
				return	instrument_job_after(	instrument_job	)	;

}

int	Trick::Executive::instrument_job_after(	Trick::JobData	*	instrument_job)	{

				unsigned	int	ii,	count	;

				count	=	0	;
				if	(	instrument_job	!=	NULL	)	{
								//	instrument	scheduled	jobs
								for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
												count	+=	threads[ii]->job_queue.instrument_after(instrument_job)	;
												count	+=	threads[ii]->top_of_frame_queue.instrument_after(instrument_job)	;
												count	+=	threads[ii]->end_of_frame_queue.instrument_after(instrument_job)	;
								}

								//	instrument	other	queued	jobs
								//	cannot	instrument	default_data	because	they	come	after	input	processor
								count	+=	default_data_queue.instrument_after(instrument_job)	;
								count	+=	initialization_queue.instrument_after(instrument_job)	;
								count	+=	top_of_frame_queue.instrument_after(instrument_job)	;
								count	+=	end_of_frame_queue.instrument_after(instrument_job)	;
								count	+=	shutdown_queue.instrument_after(instrument_job)	;

								count	+=	freeze_init_queue.instrument_after(instrument_job)	;
								count	+=	freeze_scheduled_queue.instrument_after(instrument_job)	;
								count	+=	freeze_queue.instrument_after(instrument_job)	;
								count	+=	unfreeze_queue.instrument_after(instrument_job)	;
								count	+=	time_tic_changed_queue.instrument_after(instrument_job)	;
								count	+=	thread_sync_queue.instrument_after(instrument_job)	;

								for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
												count	+=	other_schedulers[ii]->instrument_job_after(	instrument_job	)	;
								}
				}

				return(count)	;

}

int	Trick::Executive::instrument_job_remove(std::string	in_job)	{

				unsigned	int	ii	;

				/**	@par	Detailed	Design	*/
				/**	@li	Remove	instrumentation	job	from	initialization,	scheduled,
								integration,	end	of	frame,	&	shutdown		queues	by	calling	ScheduledJobQueue::instrument_remove(string).	*/
				default_data_queue.instrument_remove(in_job)	;
				initialization_queue.instrument_remove(in_job)	;
				top_of_frame_queue.instrument_remove(in_job)	;
				end_of_frame_queue.instrument_remove(in_job)	;
				shutdown_queue.instrument_remove(in_job)	;

				freeze_init_queue.instrument_remove(in_job)	;
				freeze_scheduled_queue.instrument_remove(in_job)	;
				freeze_queue.instrument_remove(in_job)	;
				unfreeze_queue.instrument_remove(in_job)	;
				time_tic_changed_queue.instrument_remove(in_job)	;
				thread_sync_queue.instrument_remove(in_job)	;

				for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
								threads[ii]->job_queue.instrument_remove(in_job)	;
								threads[ii]->top_of_frame_queue.instrument_remove(in_job)	;
								threads[ii]->end_of_frame_queue.instrument_remove(in_job)	;
				}

				for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
								other_schedulers[ii]->instrument_job_remove(in_job)	;
				}

				return(0)	;
}

