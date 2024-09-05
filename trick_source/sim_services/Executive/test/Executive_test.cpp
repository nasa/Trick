
#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>
#include	"gtest/gtest.h"

#define	protected	public
#include	"trick/ExecutiveException.hh"
#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"
#include	"trick/MessagePublisher.hh"
#include	"trick/MessageCout.hh"
#include	"trick/SimObject.hh"
#include	"trick/MemoryManager.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/CommandLineArguments.hh"
#include	"trick/GetTimeOfDayClock.hh"

void	sig_hand(int	sig)	;
void	ctrl_c_hand(int	sig)	;
void	term_hand(int	sig)	;
void	child_handler(int	sig)	;
#if	(__APPLE__	|	__CYGWIN__	|	__INTERIX	)
void	fpe_sig_handler(int	sig)	;
#else
void	fpe_sig_handler(int	sig,	siginfo_t	*	sip,	void	*uap)	;
#endif

namespace	Trick	{

class	emptySimObject	:	public	Trick::SimObject	{
				public:
								emptySimObject()	{
												add_job(0,	0,	"sensor",	NULL,	0.00000001,	"scheduled_1",	"TRK")	;
												add_job(0,	1,	"system_advance_sim_time",	NULL,	1,	"advance_sim_time",	"TRK")	;
								}

								virtual	int	call_function(	Trick::JobData	*	)	{	return	0	;	}	;
								virtual	double	call_function_double(	Trick::JobData	*	curr_job	)	{	(void)curr_job	;	return	0.0	;	}	;
}	;

class	testSimObject	:	public	Trick::SimObject	{
				public:

								unsigned	int	default_data_ran	;
								unsigned	int	input_processor_ran	;
								unsigned	int	initialization_ran	;
								unsigned	int	top_of_frame_ran	;
								unsigned	int	scheduled_ran	;
								unsigned	int	end_of_frame_ran	;
								unsigned	int	shutdown_ran	;
								unsigned	int	freeze_init_ran	;
								unsigned	int	freeze_ran	;
								unsigned	int	freeze_scheduled_ran	;
								unsigned	int	unfreeze_ran	;
								unsigned	int	time_tic_changed_ran	;

								int	default_data_1()	{
												default_data_ran++	;
												return	0	;
								}

								int	input_processor_1()	{
												input_processor_ran++	;
												return	0	;
								}

								int	initialization_1()	{
												initialization_ran++	;
												return	0	;
								}

								int	top_of_frame_1()	{
												top_of_frame_ran++	;
												return	0	;
								}

								int	scheduled_1()	{
												scheduled_ran++	;
												return	0	;
								}

								int	end_of_frame_1()	{
												end_of_frame_ran++	;
												return	0	;
								}

								int	shutdown_1()	{
												shutdown_ran++	;
												return	0	;
								}

								int	freeze_init_1()	{
												freeze_init_ran++	;
												return	0	;
								}

								int	freeze_1()	{
												freeze_ran++	;
												return	0	;
								}

								int	freeze_scheduled_1()	{
												freeze_scheduled_ran++	;
												return	0	;
								}

								int	unfreeze_1()	{
												unfreeze_ran++	;
												return	0	;
								}

								int	time_tic_changed_1()	{
												time_tic_changed_ran++	;
												return	0	;
								}

								int	return_error()	{
												return	-1	;
								}

								int	throw_exception()	{
												return	exec_terminate_with_return(-1,	"throw_exception",	1	,	"exec_terminate	called")	;
								}

								testSimObject()	:
									default_data_ran(0)	,
									input_processor_ran(0)	,
									initialization_ran(0)	,
									top_of_frame_ran(0)	,
									scheduled_ran(0)	,
									end_of_frame_ran(0)	,
									shutdown_ran(0)	,
									freeze_init_ran(0)	,
									freeze_ran(0)	,
									freeze_scheduled_ran(0)	,
									unfreeze_ran(0)	,
									time_tic_changed_ran(0)
								{
												int	ii	=	0	;
												add_job(0,	ii++,	"default_data",	NULL,	1,	"default_data_1",	"TRK")	;
												add_job(0,	ii++,	"input_processor",	NULL,	1,	"input_processor_1",	"TRK")	;
												add_job(0,	ii++,	"initialization",	NULL,	1,	"initialization_1",	"TRK")	;
												add_job(0,	ii++,	"top_of_frame",	NULL,	1,	"top_of_frame_1",	"TRK")	;
												add_job(0,	ii++,	"automatic",	NULL,	1,	"automatic_1",	"TRK")	;
												add_job(0,	ii++,	"effector",	NULL,	3,	"scheduled_3",	"TRK")	;
												add_job(0,	ii++,	"scheduled",	NULL,	2,	"scheduled_2",	"TRK")	;
												add_job(0,	ii++,	"sensor",	NULL,	1,	"scheduled_1",	"TRK")	;
												add_job(0,	ii++,	"system_advance_sim_time",	NULL,	1,	"advance_sim_time",	"TRK")	;
												add_job(0,	ii++,	"end_of_frame",	NULL,	1,	"end_of_frame_1",	"TRK")	;
												add_job(0,	ii++,	"shutdown",	NULL,	1,	"shutdown_1",	"TRK")	;
												add_job(0,	ii++,	"freeze_init",	NULL,	1,	"freeze_init_1",	"TRK")	;
												add_job(0,	ii++,	"freeze",	NULL,	1,	"freeze_1",	"TRK")	;
												add_job(0,	ii++,	"freeze_scheduled",	NULL,	1,	"freeze_scheduled_1",	"TRK")	;
												add_job(0,	ii++,	"unfreeze",	NULL,	1,	"unfreeze_1",	"TRK")	;
												add_job(0,	ii++,	"exec_time_tic_changed",	NULL,	1,	"time_tic_changed_1",	"TRK")	;
								}

								virtual	int	call_function(	Trick::JobData	*	curr_job	)	;
								virtual	double	call_function_double(	Trick::JobData	*	curr_job	)	{	(void)curr_job	;	return	0.0	;	}	;
}	;

int	testSimObject::call_function(	Trick::JobData	*	curr_job	)	{
				int	trick_ret	=	0	;
				switch	(curr_job->id)	{
								case	0:
												trick_ret	=	default_data_1()	;
												break	;
								case	1:
												trick_ret	=	input_processor_1()	;
												break	;
								case	2:
												trick_ret	=	initialization_1()	;
												break	;
								case	3:
												trick_ret	=	top_of_frame_1()	;
												break	;
								case	4:
								case	5:
								case	6:
								case	7:
								case	8:
												trick_ret	=	scheduled_1()	;
												break	;
								case	9:
												trick_ret	=	end_of_frame_1()	;
												break	;
								case	10:
												trick_ret	=	shutdown_1()	;
												break	;
								case	11:
												trick_ret	=	freeze_init_1()	;
												break	;
								case	12:
												trick_ret	=	freeze_1()	;
												break	;
								case	13:
												trick_ret	=	freeze_scheduled_1()	;
												break	;
								case	14:
												trick_ret	=	unfreeze_1()	;
												break	;
								case	15:
												trick_ret	=	time_tic_changed_1()	;
												break	;
								case	101:
												trick_ret	=	return_error()	;
												break	;
								case	102:
												trick_ret	=	throw_exception()	;
												break	;
								default:
												trick_ret	=	-1	;
												break	;
				}
				return	trick_ret	;
}	;


class	ExecutiveTest	:	public	::testing::Test	{

				protected:
								Trick::Executive	exec;
								testSimObject	so1	;
								emptySimObject	empty_so	;
								Trick::MessagePublisher	mpublisher	;
								Trick::MessageCout	mcout	;
								Trick::MemoryManager	mm	;
								Trick::CommandLineArguments	cla	;
								Trick::GetTimeOfDayClock	gtodc	;
								//Trick::RequirementScribe	req;


								ExecutiveTest()	{gtodc.set_global_clock();}
								~ExecutiveTest()	{}
								virtual	void	SetUp()	;
								virtual	void	TearDown()	{}

								int	get_class_map_value(std::string	job_class)	{	return	exec.class_map[job_class]	;	}
								Trick::ScheduledJobQueue	*	get_default_data_queue()	{	return	&exec.default_data_queue	;	}
								Trick::ScheduledJobQueue	*	get_input_processor_queue()	{	return	&exec.input_processor_queue	;	}
								Trick::ScheduledJobQueue	*	get_initialization_queue()	{	return	&exec.initialization_queue	;	}
								Trick::ScheduledJobQueue	*	get_scheduled_queue()	{	return	&exec.scheduled_queue	;	}
								Trick::ScheduledJobQueue	*	get_end_of_frame_queue()	{	return	&exec.end_of_frame_queue	;	}
								Trick::ScheduledJobQueue	*	get_shutdown_queue()	{	return	&exec.shutdown_queue	;	}
								Trick::ScheduledJobQueue	*	get_freeze_init_queue()	{	return	&exec.freeze_init_queue	;	}
								Trick::ScheduledJobQueue	*	get_freeze_queue()	{	return	&exec.freeze_queue	;	}
								Trick::ScheduledJobQueue	*	get_unfreeze_queue()	{	return	&exec.unfreeze_queue	;	}
								Trick::JobData	*	get_all_jobs()	{	return	exec.all_jobs_for_checkpoint	;	}
								Trick::JobData	*	get_job_from_all_jobs(unsigned	int	ii	)	{	return	&(exec.all_jobs_for_checkpoint[ii])	;	}
								Trick::SimObject	*	get_sim_object(unsigned	int	ii)	{	return	exec.sim_objects[ii]	;	}
								Trick::Threads	*	get_thread(unsigned	int	ii)	{	return	exec.threads[ii]	;	}
}	;


void	ExecutiveTest::SetUp()	{

				exec_add_scheduled_job_class("automatic")	;
				exec_add_scheduled_job_class("sensor")	;
				exec_add_scheduled_job_class("scheduled")	;
				exec_add_scheduled_job_class("effector")	;
				exec_add_scheduled_job_class("system_advance_sim_time")	;

				mpublisher.init()	;
}

/*
		Executive	functions	not	tested	here:
				Process	simulation	arguments
				Registering	other	schedulers
				Creating	threads
				Get	Process	ID
				Run	Loop
				Freeze	Loop
				Restart
				Shutdown
				Instrumenting	Jobs
				Write	S_job_execution	and	S_run_summary
				Signal	handling
*/

/*	JOB	TESTS	*/

TEST_F(ExecutiveTest	,	AddSimObject)	{
				//req.add_requirement(	"r_exec_jobs");
	//"The	Executive	Scheduler	shall	provide	a	method	for	adding	simulation	objects	to	the	scheduler"
				exec_add_sim_object(&so1	,	"so1")	;
}

TEST_F(ExecutiveTest	,	AddJobsToQueue)	{
				//req.add_requirement(	"r_exec_jobs");
	//"The	Executive	Scheduler	shall	sort	jobs	with	executive	job	classes	in	ScheduledJobQueues"
				exec_add_sim_object(&so1	,	"so1")	;

				Trick::JobData	*	curr_job	;
				curr_job	=	get_default_data_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.default_data_1")	;

				curr_job	=	get_input_processor_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.input_processor_1")	;

				curr_job	=	get_initialization_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.initialization_1")	;

				curr_job	=	get_scheduled_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.automatic_1")	;

				curr_job	=	get_scheduled_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_1")	;

				curr_job	=	get_scheduled_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_2")	;

				curr_job	=	get_scheduled_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_3")	;
}

TEST_F(ExecutiveTest	,	JobQueueNonScheduledIDs)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	assign	job	class	id	of	the	following	classes	<	1000,	default_data	,	input_processor	,	initialization	,	top_of_frame	,	end_of_frame	,	shutdown	,	freeze_init	,	freeze_scheduled	,	freeze_automatic,	freeze	,	unfreeze,	and	exec_time_tic_changed"
				EXPECT_LT(	get_class_map_value("default_data")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("input_processor")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("initialization")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("top_of_frame")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("end_of_frame")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("shutdown")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("freeze_init")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("freeze_scheduled")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("freeze_automatic")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("freeze")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("unfreeze")	,	1000	)	;
				EXPECT_LT(	get_class_map_value("exec_time_tic_changed")	,	1000	)	;
}

TEST_F(ExecutiveTest	,	AddScheduledJobClass)	{
				//req.add_requirement("1630595230");
	//"The	Executive	Scheduler	shall	provide	a	method	for	adding	scheduled	job	class	names"
				//	some	names	were	added	in	the	SetUp	method
				EXPECT_EQ(	get_class_map_value("automatic")	,	1000	)	;
}

TEST_F(ExecutiveTest	,	JobQueueStartIndex)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	assign	the	first	scheduled	job	class	id	=	1000"
				EXPECT_EQ(	get_class_map_value("automatic")	,	1000	)	;
}

TEST_F(ExecutiveTest	,	JobQueueNameOrder)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	increment	the	job	class	id	for	each	additional	scheduled	job	class"

				EXPECT_EQ(	get_class_map_value("automatic")	,	1000	)	;
				EXPECT_EQ(	get_class_map_value("sensor")	,	1001	)	;
				EXPECT_EQ(	get_class_map_value("scheduled")	,	1002	)	;
				EXPECT_EQ(	get_class_map_value("effector")	,	1003	)	;
				EXPECT_EQ(	get_class_map_value("system_advance_sim_time")	,	1004	)	;
}

//	Job	class	ordering	is	tested	in	ScheduledJobQueue.

TEST_F(ExecutiveTest	,	AddDepends)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	provide	the	capability	to	add	dependency	jobs	to	scheduled	jobs"

				Trick::JobData	*	curr_job	;

				so1.add_job(1,	100,	"scheduled",	NULL,	1,	"child_job_1",	"TRK")	;
				so1.add_job(2,	100,	"scheduled",	NULL,	1,	"child_job_2a",	"TRK")	;
				so1.add_job(2,	100,	"scheduled",	NULL,	1,	"child_job_2b",	"TRK")	;

				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_EQ(exec.add_depends_on_job("so1.child_job_3"	,	1	,	"so1.scheduled_1"	,	1),	-1)	;
				EXPECT_EQ(exec.add_depends_on_job("so1.child_job_1"	,	1	,	"so1.scheduled_4"	,	1),	-2)	;
				EXPECT_EQ(exec.add_depends_on_job("so1.child_job_2a"	,	1	,	"so1.child_job_2b"	,	1),	-3)	;
				EXPECT_EQ(exec.add_depends_on_job("so1.scheduled_1"	,	1	,	"so1.scheduled_1"	,	1),	-3)	;

				curr_job	=	exec.get_job("so1.child_job_1")	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_EQ(curr_job->depends.size(),	(unsigned	int)0)	;
				EXPECT_EQ(exec.add_depends_on_job("so1.child_job_1"	,	1	,	"so1.scheduled_1"	,	1),	0)	;
				EXPECT_EQ(curr_job->depends.size(),	(unsigned	int)1)	;
				EXPECT_STREQ(curr_job->depends[0]->name.c_str(),	"so1.scheduled_1")	;

}

TEST_F(ExecutiveTest	,	UnhandledJobs)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	provide	the	capability	to	list	jobs	not	handled	by	any	scheduler."

				so1.add_job(0,	100,	"unhandled",	NULL,	1,	"unhandled_job",	"")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.check_all_jobs_handled()	,	1	)	;
}

TEST_F(ExecutiveTest	,	JobCycleNotDivisibleByTimeTic)	{
				//req.add_requirement("r_exec_jobs");
	//"The	Executive	Scheduler	shall	provide	the	capability	to	list	jobs	not	handled	by	any	scheduler."

				EXPECT_EQ(exec_get_time_tic_value(),	1000000)	;
				exec_add_sim_object(&empty_so	,	"empty_so")	;
				EXPECT_EQ(exec.check_all_job_cycle_times()	,	-1	)	;
}

/*	INITIALIZATION	*/

TEST_F(ExecutiveTest	,	GoodInitialization)	{
				//req.add_requirement("2605574440")	;

				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_EQ(exec.init()	,	0	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	1	)	;
				EXPECT_EQ(so1.initialization_ran	,	1	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;

}

TEST_F(ExecutiveTest	,	ErrorDefaultData)	{
				//req.add_requirement("773089468")	;

				so1.add_job(0,	101,	"default_data",	NULL,	1,	"return_error",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	0	)	;
				EXPECT_EQ(so1.initialization_ran	,	0	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

TEST_F(ExecutiveTest	,	ErrorInputProcessor)	{
				//req.add_requirement("773089468")	;

				so1.add_job(0,	101,	"input_processor",	NULL,	1,	"return_error",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	1	)	;
				EXPECT_EQ(so1.initialization_ran	,	0	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

TEST_F(ExecutiveTest	,	ErrorInitialization)	{
				//req.add_requirement("773089468")	;

				so1.add_job(0,	101,	"initialization",	NULL,	1,	"return_error",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	1	)	;
				EXPECT_EQ(so1.initialization_ran	,	1	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

TEST_F(ExecutiveTest	,	ExeceptionDefaultData)	{
				//req.add_requirement("773089468")	;
				so1.add_job(0,	102,	"default_data",	NULL,	1,	"throw_exception",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	0	)	;
				EXPECT_EQ(so1.initialization_ran	,	0	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

TEST_F(ExecutiveTest	,	ExeceptionInputProcessor)	{
				//req.add_requirement("773089468")	;
				so1.add_job(0,	102,	"input_processor",	NULL,	1,	"throw_exception",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	1	)	;
				EXPECT_EQ(so1.initialization_ran	,	0	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

TEST_F(ExecutiveTest	,	ExeceptionInitialization)	{
				//req.add_requirement("2605574440")	;
				so1.add_job(0,	102,	"initialization",	NULL,	1,	"throw_exception",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.init()	,	-1	)	;

				EXPECT_EQ(so1.default_data_ran	,	1	)	;
				EXPECT_EQ(so1.input_processor_ran	,	1	)	;
				EXPECT_EQ(so1.initialization_ran	,	1	)	;
				EXPECT_EQ(so1.top_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.end_of_frame_ran	,	0	)	;
				EXPECT_EQ(so1.shutdown_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_init_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_ran	,	0	)	;
				EXPECT_EQ(so1.freeze_scheduled_ran	,	0	)	;
				EXPECT_EQ(so1.unfreeze_ran	,	0	)	;
				EXPECT_EQ(so1.time_tic_changed_ran	,	0	)	;
}

/*	TIME	TESTS	*/

TEST_F(ExecutiveTest	,	IntegerTime)	{
				//req.add_requirement("2618149062");
	//"The	Executive	Scheduler	shall	track	simulation	elapsed	time	by	an	integer	count	of	tics/second")	;
				EXPECT_EQ(exec_get_time_tics(),	0)	;
}

TEST_F(ExecutiveTest	,	DefaultTimeTicValue)	{
				//req.add_requirement("2436759852");
	//	"The	Executive	Scheduler	shall	default	to	1,000,000	tics/second")	;
				EXPECT_EQ(exec_get_time_tic_value(),	1000000)	;
}

TEST_F(ExecutiveTest	,	AdvanceSimTimeToNextJob)	{

				//req.add_requirement("2449626778");
	//"The	Executive	Scheduler	shall	advance	simulation	time	to	the	time	of	the	next	lowest	job	call	time	if	the	next	lowest	job	call	time	is	less	than	the	next	software	frame	boundary	")	;
				Trick::Threads	*	curr_thread	;
				Trick::JobData	*	curr_job	;

				exec_add_sim_object(&empty_so	,	"empty_so")	;

				curr_thread	=	get_thread(0)	;

				//	Test	where	next	job	call	time	is	less	than	the	frame	boundary
				exec.set_time_tics(0)	;
				do	{
							curr_job	=	curr_thread->job_queue.get_next_job()	;
				}	while	(	curr_job	!=	NULL	)	;
				curr_thread->job_queue.set_next_job_call_time(20000)	;
				exec.set_software_frame(0.05)	;
				EXPECT_EQ(	exec.get_time_tics()	,	0	)	;
				exec.advance_sim_time()	;
				EXPECT_EQ(	exec.get_time_tics()	,	20000	)	;
}

TEST_F(ExecutiveTest	,	AdvanceSimTimeToBoundary)	{

				//req.add_requirement("4140012270");
	//	"The	Executive	Scheduler	shall	advance	simulation	time	to	the	time	of	the	next	software	boundary	if	the	next	lowest	job	call	time	is	greater	than	or	equal	to	the	next	software	frame	boundary	")	;
				Trick::Threads	*	curr_thread	;

				exec_add_sim_object(&empty_so	,	"empty_so")	;

				curr_thread	=	get_thread(0)	;

				//	Test	where	next	job	call	time	is	less	than	the	frame	boundary
				exec.set_software_frame(0.05)	;
				exec.init()	;
				curr_thread->job_queue.set_next_job_call_time(200000)	;
				EXPECT_EQ(	exec.get_time_tics()	,	0	)	;
				exec.advance_sim_time()	;
				EXPECT_EQ(	exec.get_time_tics()	,	50000	)	;

}

TEST_F(ExecutiveTest	,	Checkpoint)	{

				//req.add_requirement("2678139818	1653083281");
	//"The	Executive	Scheduler	shall	copy	all	job	information	to	a	checkpointable	array	before	a	checkpoint	is	executed")	;

				unsigned	int	ii	=	0	;
				Trick::JobData	*	curr_job	;
				Trick::SimObject	*	curr_obj	;

				exec_add_sim_object(&so1	,	"so1")	;
				exec.checkpoint()	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.default_data_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.input_processor_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.initialization_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.top_of_frame_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.automatic_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_3")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_2")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.scheduled_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.advance_sim_time")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.end_of_frame_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.shutdown_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.freeze_init_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.freeze_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.freeze_scheduled_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.unfreeze_1")	;

				curr_job	=	get_job_from_all_jobs(ii++)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.time_tic_changed_1")	;

				ii	=	0	;
				curr_obj	=	get_sim_object(ii++)	;
				ASSERT_FALSE(	curr_obj	==	NULL	)	;
				EXPECT_STREQ(	curr_obj->name.c_str()	,	"so1")	;
}

TEST_F(ExecutiveTest	,	ClearScheduledQueues)	{
	//req.add_requirement("");

				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_FALSE(get_default_data_queue()->empty())	;
				EXPECT_FALSE(get_initialization_queue()->empty())	;
				EXPECT_FALSE(get_scheduled_queue()->empty())	;
				EXPECT_FALSE(get_end_of_frame_queue()->empty())	;
				EXPECT_FALSE(get_shutdown_queue()->empty())	;
				EXPECT_FALSE(get_freeze_init_queue()->empty())	;
				EXPECT_FALSE(get_freeze_queue()->empty())	;
				EXPECT_FALSE(get_unfreeze_queue()->empty())	;

				exec.clear_scheduled_queues()	;

				EXPECT_TRUE(get_default_data_queue()->empty())	;
				EXPECT_TRUE(get_initialization_queue()->empty())	;
				EXPECT_TRUE(get_scheduled_queue()->empty())	;
				EXPECT_TRUE(get_end_of_frame_queue()->empty())	;
				EXPECT_TRUE(get_shutdown_queue()->empty())	;
				EXPECT_TRUE(get_freeze_init_queue()->empty())	;
				EXPECT_TRUE(get_freeze_queue()->empty())	;
				EXPECT_TRUE(get_unfreeze_queue()->empty())	;

}

TEST_F(ExecutiveTest	,	Moding)	{
	//req.add_requirement("411993330	2253353824	2874482372");

				exec.freeze()	;
				exec.async_freeze_to_exec_command()	;
				EXPECT_EQ(exec_get_exec_command()	,	FreezeCmd)	;
				exec.run()	;
				EXPECT_EQ(exec_get_exec_command()	,	RunCmd)	;
				exec.stop()	;
				EXPECT_EQ(exec_get_exec_command()	,	ExitCmd)	;
				exec.stop(123.0)	;
				EXPECT_NEAR(exec_get_terminate_time()	,	123.0	,	0.000001)	;
}

TEST_F(ExecutiveTest	,	GetJob)	{
	//req.add_requirement("");

				Trick::JobData	*	curr_job	;

				/*	Add	a	second	job	of	the	same	name	of	an	existing	job	*/
				curr_job	=	so1.add_job(0,	100,	"sensor",	NULL,	1,	"shutdown_1",	"TRK")	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	exec.get_job(	std::string("so1.shutdown_1"))	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->job_class_name.c_str()	,	"shutdown")	;

				curr_job	=	exec.get_job(	std::string("so1.shutdown_1"),	2)	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->job_class_name.c_str()	,	"sensor")	;

				curr_job	=	exec.get_job(	std::string("so1.shutdown_1"),	3)	;
				ASSERT_TRUE(	curr_job	==	NULL	)	;

				curr_job	=	exec.get_job(	std::string("so1.does_not_exist"))	;
				ASSERT_TRUE(	curr_job	==	NULL	)	;

}

TEST_F(ExecutiveTest	,	JobCycle)	{
	//req.add_requirement("");

				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_NEAR(	exec.get_job_cycle(std::string("so1.scheduled_3"))	,	3.0	,	0.000001)	;
				EXPECT_NEAR(	exec.get_job_cycle(std::string("so1.scheduled_2"))	,	2.0	,	0.000001)	;

				EXPECT_EQ(exec.set_job_cycle("so1.scheduled_2",	1,	22.2),	0)	;
				EXPECT_NEAR(	exec.get_job_cycle(std::string("so1.scheduled_2"))	,	22.2	,	0.000001)	;

				EXPECT_EQ(exec.set_job_cycle("so1.scheduled_2",	1,	-3.3),	-1)	;

				EXPECT_EQ(exec.set_job_cycle("so1.scheduled_4",	1,	22.2),	-2)	;

				//	All	other	job	classes	are	defaulted	to	cycle	time	of	1.0	seconds	even	though	they	are	not	used.
				EXPECT_NEAR(	exec.get_job_cycle(std::string("so1.shutdown_1"))	,	1.0	,	0.000001)	;
}


TEST_F(ExecutiveTest	,	SetSignalHandlers)	{
	//req.add_requirement("");

				static	struct	sigaction	sigact;

				exec.init_signal_handlers()	;
				sigaction(SIGINT,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	ctrl_c_hand	)	;
				sigaction(SIGTERM,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	term_hand	)	;
				sigaction(SIGBUS,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	sig_hand	)	;
				sigaction(SIGSEGV,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	sig_hand	)	;
				sigaction(SIGFPE,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	SIG_DFL	)	;

				exec.set_trap_sigbus(1)	;
				exec.set_trap_sigsegv(1)	;
				exec.set_trap_sigfpe(1)	;
				exec.set_trap_sigchld(1)	;
				sigaction(SIGBUS,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	sig_hand	)	;
				sigaction(SIGSEGV,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	sig_hand	)	;
				sigaction(SIGCHLD,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	child_handler	)	;
				sigaction(SIGFPE,	NULL	,	&sigact)	;
#if	__APPLE__
				EXPECT_TRUE(	sigact.sa_handler	==	fpe_sig_handler	)	;
#else
				EXPECT_TRUE(	sigact.sa_sigaction	==	fpe_sig_handler	)	;
#endif

				exec.set_trap_sigbus(0)	;
				exec.set_trap_sigsegv(0)	;
				exec.set_trap_sigfpe(0)	;
				sigaction(SIGBUS,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	SIG_DFL	)	;
				sigaction(SIGSEGV,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	SIG_DFL	)	;
				sigaction(SIGFPE,	NULL	,	&sigact)	;
				EXPECT_TRUE(	sigact.sa_handler	==	SIG_DFL	)	;
}

TEST_F(ExecutiveTest	,	JobOnOff)	{
	//req.add_requirement("2445198072");

				Trick::JobData	*	curr_job	;

				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_EQ(exec.set_job_onoff("so1.scheduled_1"	,	1	,	1),	0)	;
				curr_job	=	exec.get_job(	std::string("so1.scheduled_1"))	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_EQ(	curr_job->disabled	,	false)	;

				EXPECT_EQ(exec.set_job_onoff("so1.scheduled_1"	,	1	,	0),	0)	;
				EXPECT_EQ(	curr_job->disabled	,	true)	;

				EXPECT_EQ(exec.set_job_onoff("so1.scheduled_4"	,	1	,	0),	-1)	;
}

TEST_F(ExecutiveTest	,	SimObjectJobsOnOff)	{
				Trick::JobData	*	curr_job	;

				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_EQ(exec.set_sim_object_jobs_onoff("so1"	,	0),	0)	;
				curr_job	=	exec.get_job(	std::string("so1.scheduled_1"))	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_EQ(	curr_job->disabled	,	true)	;
				EXPECT_EQ(	exec.get_sim_object_onoff("so1"),	1);

				EXPECT_EQ(exec.set_sim_object_jobs_onoff("so1"	,	1),	0)	;
				EXPECT_EQ(	curr_job->disabled	,	false)	;

				EXPECT_EQ(exec.set_sim_object_jobs_onoff("so2"	,	1),	-1)	;

}

TEST_F(ExecutiveTest	,	SimObjectOnOff)	{

				exec_add_sim_object(&so1	,	"so1")	;

				//	Test	that	after	disabling	and	enabling	sim	object,	top_of_frame	is	still	disabled
				Trick::JobData	*	curr_job	=	exec.get_job(	std::string("so1.top_of_frame_1"))	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;

				curr_job->disable();

				EXPECT_EQ(exec.set_sim_object_onoff("so1"	,	0),	0)	;
				EXPECT_EQ(exec.get_sim_object_onoff("so1"),	0);
				EXPECT_EQ(curr_job->disabled	,	true)	;

				//	Add	an	extra	job
				so1.add_job(1,	100,	"scheduled",	NULL,	1,	"child_job_1",	"TRK")	;
				exec.add_jobs_to_queue(&so1,	false);

				Trick::JobData	*	new_job	=	exec.get_job(	std::string	("so1.child_job_1"));
				ASSERT_FALSE(	new_job	==	NULL	)	;

				EXPECT_EQ(exec.set_sim_object_onoff("so1"	,	1),	0)	;
				EXPECT_EQ(	exec.get_sim_object_onoff("so1"),	1);
				EXPECT_EQ(	curr_job->disabled	,	true)	;
				EXPECT_EQ(	new_job->disabled	,	false)	;

				EXPECT_EQ(exec.set_sim_object_onoff("so2"	,	1),	-1)	;
				EXPECT_EQ(exec.get_sim_object_onoff("so2"),	-1)	;
}

TEST_F(ExecutiveTest	,	LockMemory)	{
	//req.add_requirement("715042410");
				//	Requires	root	to	run.	Will	pass	if	run	as	root.
				//EXPECT_EQ(exec.set_lock_memory(true),	0)	;

}

TEST_F(ExecutiveTest	,	SetThreadParameters)	{
	//req.add_requirement("3839914374");

				so1.add_job(1,	100,	"scheduled",	NULL,	1,	"child_job_1",	"TRK")	;
				exec_add_sim_object(&so1	,	"so1")	;

				EXPECT_EQ(	exec.set_thread_process_type(0	,	PROCESS_TYPE_SCHEDULED)	,	-2	)	;
				EXPECT_EQ(	exec.set_thread_process_type(2	,	PROCESS_TYPE_SCHEDULED)	,	-2	)	;

				EXPECT_EQ(	exec.set_thread_amf_cycle_time(0	,	1.0)	,	-2	)	;
				EXPECT_EQ(	exec.set_thread_amf_cycle_time(2	,	1.0)	,	-2	)	;

				EXPECT_EQ(	exec.set_thread_process_type(1	,	PROCESS_TYPE_SCHEDULED)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_amf_cycle_time(1	,	1.0)	,	0	)	;

				EXPECT_EQ(	exec.set_thread_process_type(1	,	PROCESS_TYPE_ASYNC_CHILD)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_amf_cycle_time(1	,	1.0)	,	0	)	;

				EXPECT_EQ(	exec.set_thread_process_type(1	,	PROCESS_TYPE_AMF_CHILD)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_amf_cycle_time(1	,	1.0)	,	0	)	;

				EXPECT_EQ(	exec.set_thread_async_wait(0	,	1)	,	-2	)	;
				EXPECT_EQ(	exec.set_thread_async_wait(2	,	1)	,	-2	)	;
				EXPECT_EQ(	exec.set_thread_async_wait(1	,	1)	,	0	)	;

				EXPECT_EQ(	exec.set_thread_cpu_affinity(0	,	1)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_cpu_affinity(1	,	2)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_cpu_affinity(2	,	1)	,	-2	)	;

				EXPECT_EQ(	exec.set_thread_priority(0	,	1)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_priority(1	,	2)	,	0	)	;
				EXPECT_EQ(	exec.set_thread_priority(2	,	1)	,	-2	)	;
}

}
