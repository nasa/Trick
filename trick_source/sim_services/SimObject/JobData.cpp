
#include	<math.h>

#include	"trick/JobData.hh"
#include	"trick/SimObject.hh"

long	long	Trick::JobData::time_tic_value	=	0	;

Trick::JobData::JobData()	{

				/**	@par	Detailed	Design	*/

				/**	@li	initializes	the	job	as	enabled	with	0	cycle	rate.	*/
				disabled	=	false	;
				handled	=	false	;
				thread	=	0	;
				id	=	0	;
				sim_object_id	=	-1	;
				job_class	=	-1	;
				sup_class_data	=	NULL	;
				cycle	=	0.0	;
				start	=	0.0	;
				stop	=	0.0	;
				complete	=	false	;
				rt_start_time	=	-1;
				phase	=	60000	;
				system_job_class	=	0	;

				cycle_tics	=	0	;
				start_tics	=	0	;
				stop_tics	=	0	;
				next_tics	=	0	;

				frame_time	=	0	;
				frame_time_seconds	=	0.0;
}

Trick::JobData::JobData(int	in_thread,	int	in_id,	std::string	in_job_class_name	,	void*	in_sup_class_data,
																								double	in_cycle,	std::string	in_name,	std::string	in_tag,	int	in_phase,
																								double	in_start,	double	in_stop)	{

				/**	@par	Detailed	Design	*/

				/**	@li	initializes	the	job	according	to	arguments	*/
				disabled	=	false	;
				handled	=	false	;
				thread	=	in_thread	;
				id	=	in_id	;
				sim_object_id	=	-1	;
				job_class	=	-1	;
				job_class_name	=	in_job_class_name	;
				sup_class_data	=	in_sup_class_data	;
				cycle	=	in_cycle	;
				start	=	in_start	;
				stop	=	in_stop	;
				complete	=	false	;
				name	=	in_name	;
				add_tag(in_tag)	;
				rt_start_time	=	-1;
				phase	=	in_phase	;
				system_job_class	=	0	;

				cycle_tics	=	0	;
				start_tics	=	0	;
				stop_tics	=	0	;
				next_tics	=	0	;

				frame_time	=	0	;
				frame_time_seconds	=	0.0	;
}

void	Trick::JobData::enable()	{
				disabled	=	false	;
}

void	Trick::JobData::disable()	{
				disabled	=	true	;
}

bool	Trick::JobData::get_handled()	{
				return(handled)	;
}

int	Trick::JobData::set_handled(bool	yes_no)	{
				handled	=	yes_no	;
				return(0)	;
}

int	Trick::JobData::set_time_tic_value(long	long	in_time_tic_value)	{
				time_tic_value	=	in_time_tic_value	;
				return	0	;
}

int	Trick::JobData::set_cycle(double	in_cycle)	{
				cycle	=	in_cycle	;
				calc_cycle_tics()	;
				return	0	;
}

int	Trick::JobData::calc_cycle_tics()	{
				cycle_tics		=	(long	long)round(cycle	*	time_tic_value)	;
				return	0	;
}

int	Trick::JobData::set_next_call_time(long	long	time_tics)	{
				if	(	next_tics	>	time_tics		)	{
								next_tics	=	time_tics	-	(	time_tics	%	cycle_tics	)	+	cycle_tics	;
				}	else	{
								next_tics	=	time_tics	;
				}
				return	0	;
}


int	Trick::JobData::set_system_job_class(bool	yes_no)	{
				system_job_class	=	yes_no	;
				return(0)	;
}

int	Trick::JobData::add_tag(	std::string	in_tag	)	{
				if	(	!	in_tag.empty()	)	{
								tags.insert(in_tag)	;
				}
				return(0)	;
}

int	Trick::JobData::add_depend(	JobData	*	depend_job	)	{
				depends.push_back(depend_job)	;
				return(0)	;
}

int	Trick::JobData::add_inst_before(	InstrumentBase	*	in_job	)	{
				std::vector<Trick::InstrumentBase*>::size_type	ii	;
				for	(	ii	=	0	;	ii	<	inst_before.size()	;	++ii	)	{
								if	(	in_job->phase	<	inst_before[ii]->phase	)	{
												break	;
								}
				}
				inst_before.insert(	inst_before.begin()	+	ii,	in_job	)	;
				return	0	;
}

int	Trick::JobData::add_inst_after(	InstrumentBase	*	in_job	)	{
				std::vector<Trick::InstrumentBase*>::size_type	ii	;
				for	(	ii	=	0	;	ii	<	inst_after.size()	;	++ii	)	{
								if	(	in_job->phase	<	inst_after[ii]->phase	)	{
												break	;
								}
				}
				inst_after.insert(	inst_after.begin()	+	ii,	in_job	)	;
				return	0	;
}

int	Trick::JobData::remove_inst(	std::string	job_name	)	{
				int	ii	,	size	;

				size	=	inst_before.size()	;
				for	(	ii	=	size	-	1	;	ii	>=	0	;	ii--	)	{
								if	(	!	inst_before[ii]->name.compare(job_name)	)	{
												inst_before.erase(	inst_before.begin()	+	ii	)	;
								}
				}

				size	=	inst_after.size()	;
				for	(	ii	=	size	-	1	;	ii	>=	0	;	ii--	)	{
								if	(	!	inst_after[ii]->name.compare(job_name)	)	{
												inst_after.erase(	inst_after.begin()	+	ii	)	;
								}
				}
				return	0	;
}

int	Trick::JobData::call()	{
				int	ret	;
				unsigned	int	ii	,	size	;
				InstrumentBase	*	curr_job	;

				size	=	inst_before.size()	;
				for	(	ii	=	0	;	ii	<	size	;	ii++	)	{
								curr_job	=	inst_before[ii]	;
								curr_job->call()	;
				}

				ret	=	parent_object->call_function(this)	;

				size	=	inst_after.size()	;
				for	(	ii	=	0	;	ii	<	size	;	ii++	)	{
								curr_job	=	inst_after[ii]	;
								curr_job->call()	;
				}

				return	ret	;
}

double	Trick::JobData::call_double()	{
				double	ret	;
				unsigned	int	ii	,	size	;
				InstrumentBase	*	curr_job	;

				size	=	inst_before.size()	;
				for	(	ii	=	0	;	ii	<	size	;	ii++	)	{
								curr_job	=	inst_before[ii]	;
								curr_job->call()	;
				}

				ret	=	parent_object->call_function_double(this)	;

				size	=	inst_after.size()	;
				for	(	ii	=	0	;	ii	<	size	;	ii++	)	{
								curr_job	=	inst_after[ii]	;
								curr_job->call()	;
				}

				return	ret	;
}

int	Trick::JobData::copy_from_checkpoint(	JobData	*	in_job	)	{

				/**	@par	Detailed	Design	*/

				/**	@li	resets	the	current	job	disabled,	cycle	rate,	and	job	class	name	and	phase	from
												incoming	job	data	*/

				disabled	=	in_job->disabled	;
				complete	=	in_job->complete	;

				handled	=	in_job->handled	;

				cycle	=	in_job->cycle	;
				start	=	in_job->start	;
				stop	=	in_job->stop	;

				job_class_name	=	in_job->job_class_name	;
				phase	=	in_job->phase	;
				system_job_class	=	in_job->system_job_class	;

				thread	=	in_job->thread	;

				cycle_tics	=	in_job->cycle_tics	;
				start_tics	=	in_job->start_tics	;
				stop_tics	=	in_job->stop_tics	;
				next_tics	=	in_job->next_tics	;

				return(0)	;
}
