
#include	<iostream>
#include	"trick/FrameDataRecordGroup.hh"
#include	"trick/clock_proto.h"
#include	"trick/exec_proto.h"

/**
-#	Creates	frame	timing	jobs	if	this	is	not	the	main	thread.
-#	All	instances	get	the	end_of_frame	frame_log_clear	job.
*/
Trick::FrameDataRecordGroup::FrameDataRecordGroup(	int	in_thread_id	,	std::string	in_name	)
	:	Trick::DRBinary(in_name,	false)	,	thread_id(in_thread_id	)	{
				if	(	thread_id	>	0	)	{
								add_job(thread_id,	1000,	(char	*)"top_of_frame",	NULL,	1.0,	(char	*)"start_timer",	(char	*)"TRK",	1)	;
								//	Frame	logging	uses	phase	65533	in	FrameLog.ccp.	Stop	the	timer	just	before	that.
								add_job(thread_id,	1001,	(char	*)"end_of_frame",	NULL,	1.0,	(char	*)"stop_timer",	(char	*)"TRK",	65532)	;
				}
				add_job(thread_id,	1003,	(char	*)"end_of_frame",	NULL,	1.0,	(char	*)"adjust_time",	(char	*)"TRK",	65532)	;
				add_job(thread_id,	1002,	(char	*)"end_of_frame",	NULL,	1.0,	(char	*)"frame_log_clear",	(char	*)"TRK",	65535)	;

				//	The	DataRecordGroup	class	establishes	a	sim_time	variable.		All	of	the	frame	logging	data	recording	groups
				//	record	at	end_of_frame.		At	this	point	the	sim_time	has	already	advanced.		We	need	to	back	up	time	when
				//	recording	the	data.	the	thread_record_time	variable	stores	the	time	of	the	previous	frame	and	can	be
				//	used	in	place	of	the	current	sim	time	when	recording	frame	data.
				rec_buffer[0]->ref->address	=	&thread_record_time	;
}

/**
-#	Clears	the	frame	log	times	for	jobs	recorded	by	this	group
*/
int	Trick::FrameDataRecordGroup::frame_log_clear()	{
				if	(	record	==	false	)	{
								return	0	;
				}
				std::vector<	Trick::JobData	*>::iterator	it	;
				for	(	it	=	rec_jobs.begin()	;	it	!=	rec_jobs.end()	;	++it	)	{
								(*it)->frame_time	=	0	;
								(*it)->frame_time_seconds	=	0.0;
				}

				return	0	;
}

/**
-#	Turns	on	recording
-#	Clears	the	frame	log	times
*/
int	Trick::FrameDataRecordGroup::enable()	{
				record	=	true	;
				frame_log_clear()	;
				return	0	;
}

/**
-#	Adds	a	recording	job	to	the	list	we	are	recording
*/
void	Trick::FrameDataRecordGroup::add_rec_job(	Trick::JobData	*	new_job	)	{
				rec_jobs.push_back(new_job)	;
}

/**
-#	Calls	the	FrameDataRecordGroup	specific	jobs	or	dispatches	the	job	call	to	the	DataRecordGroup
*/
int	Trick::FrameDataRecordGroup::call_function(	Trick::JobData	*	curr_job	)	{

				switch	(curr_job->id	)	{
								case	1000:
												start_timer()	;
												break	;
								case	1001:
												stop_timer()	;
												break	;
								case	1002:
												return	frame_log_clear()	;
								case	1003:
												adjust_time()	;
												break	;
								default:
												return	DataRecordGroup::call_function(curr_job)	;
				}

				return	0	;
}

/**
-#	Starts	the	AMF	thread	frame	time
*/
void	Trick::FrameDataRecordGroup::start_timer()	{
				start_time	=	clock_time()	;
}

/**
-#	Records	the	AMF	thread	frame	time	in	frame_sched_time.
*/
void	Trick::FrameDataRecordGroup::stop_timer()	{
				frame_sched_time	=	clock_time()	-	start_time	;
				frame_time	=	frame_sched_time	*	(1.0/exec_get_time_tic_value());
}

void	Trick::FrameDataRecordGroup::adjust_time()	{
				if	(	thread_id	==	0	)	{
								thread_record_time	=		exec_get_sim_time()	-	exec_get_software_frame()	;
				}	else	{
								thread_record_time	=		exec_get_sim_time()	-	exec_get_thread_amf_cycle_time(thread_id)	;
				}
}

/**
-#	Restart	the	frame	log	record	group	after	checkpoint	load
*/
int	Trick::FrameDataRecordGroup::restart()	{
				//	clear	the	list	of	target	jobs	for	this	frame	recording	group
				rec_jobs.clear()	;
				//	Add	back	the	time	variable
				add_time_variable()	;

				return	0	;
}
