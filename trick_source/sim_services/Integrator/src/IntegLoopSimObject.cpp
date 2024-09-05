
#include	"trick/IntegLoopSimObject.hh"
#include	"trick/exec_proto.hh"

void	IntegLoopSimObject::add_jobs(double	in_cycle,	unsigned	int	child)	{
				Trick::JobData	*	job	;

				exec_register_scheduler(&integ_sched)	;
				job	=	add_job(0,	0,	"default_data",	NULL,	1,	"integ_sched.rebuild_jobs",	"",	65534)	;
				job->add_tag("TRK")	;
				job	=	add_job(0,	1,	"initialization",	NULL,	1,	"integ_sched.get_first_step_deriv_from_integrator",	"",	65535)	;
				job->add_tag("TRK")	;
				job	=	add_job(0,	2,	"initialization",	NULL,	1,	"integ_sched.call_deriv_jobs",	"",	65535)	;
				job->add_tag("TRK")	;
				job	=	add_job(child,	3,	"integ_loop",	NULL,	in_cycle,	"integ_sched.integrate",	"",	60000)	;
				job->add_tag("TRK")	;
				job	=	add_job(0,	4,	"preload_checkpoint",	NULL,	1,	"integ_sched.restart_checkpoint",	"",	0)	;
				job->add_tag("TRK")	;
				job	=	add_job(0,	5,	"restart",	NULL,	1,	"integ_sched.rebuild_jobs",	"",	60000)	;
				job->add_tag("TRK")	;
				job	=	add_job(0,	6,	"restart",	NULL,	1,	"integ_sched.get_first_step_deriv_from_integrator",	"",	65535)	;
				job->add_tag("TRK")	;
}

int	IntegLoopSimObject::call_function	(	Trick::JobData	*	curr_job	)	{

				int	trick_ret	=	0	;
				if	(	curr_job->disabled	)	return	(trick_ret)	;

				switch	(	curr_job->id	)	{
								case	0:
												integ_sched.rebuild_jobs()	;
												break	;
								case	1:
												integ_sched.get_first_step_deriv_from_integrator()	;
												break	;
								case	2:
												integ_sched.call_deriv_jobs()	;
												break	;
								case	3:
												integ_sched.integrate()	;
												break	;
								case	4:
												integ_sched.restart_checkpoint()	;
												break	;
								case	5:
												integ_sched.rebuild_jobs()	;
												break	;
								case	6:
												integ_sched.get_first_step_deriv_from_integrator()	;
												break	;
								default:
												trick_ret	=	-1	;
												break	;
				}

				return(trick_ret)	;
}

double	IntegLoopSimObject::call_function_double	(	Trick::JobData	*	curr_job	)	{

				double	trick_ret	=	0.0	;
				if	(	curr_job->disabled	)	return	(trick_ret)	;

				switch	(	curr_job->id	)	{
								default:
												trick_ret	=	0.0	;
												break	;
				}

				return(trick_ret)	;
}

