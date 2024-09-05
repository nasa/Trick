
#include	"trick/ScheduledJobQueueInstrument.hh"
#include	"trick/SimObject.hh"

/**
@details
-#	Copy	name	from	the	instrumentation	S_define	job.
-#	Make	a	copy	of	the	instrumentation	job
-#	Save	pointer	to	the	target	job.
*/
Trick::ScheduledJobQueueInstrument::ScheduledJobQueueInstrument(	Trick::JobData*	in_inst_job,
																																																							Trick::JobData*	in_targ_job	)	{
				name	=	in_inst_job->name;
				instru_job	=	new	JobData(*in_inst_job)	;
				target_job	=	in_targ_job;
				instru_job->sup_class_data	=	target_job	;
				phase	=	in_inst_job->phase	;
}

/**
@details
-#	Delete	the	instru_job	allocated	in	the	constructor.
*/
Trick::ScheduledJobQueueInstrument::~ScheduledJobQueueInstrument()	{
				delete	instru_job	;
}

/**
@details
-#	Copy	the	target	job	into	the	sup_class_data	pointer.		This	is	available	from	in	the	insturmentation	job.
-#	Call	the	instrumentation	function	directly	to	save	a	bit	of	time
*/
int	Trick::ScheduledJobQueueInstrument::call()	{
				return	instru_job->parent_object->call_function(instru_job)	;
}

