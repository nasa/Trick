
#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>

#include	"gtest/gtest.h"
#include	"trick/ScheduledJobQueue.hh"
//#include	"trick/RequirementScribe.hh"

namespace	Trick	{

class	ScheduledJobQueueTest	:	public	::testing::Test	{

				protected:
								Trick::ScheduledJobQueue	sjq;
		//Trick::RequirementScribe	req;

								ScheduledJobQueueTest()	{}
								~ScheduledJobQueueTest()	{}
								virtual	void	SetUp()	{}
								virtual	void	TearDown()	{}

}	;

TEST_F(	ScheduledJobQueueTest	,	PushJobsbyJobOrder	)	{
	//req.add_requirement("815793485");

				Trick::JobData	*	job_ptr	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;
				EXPECT_TRUE(	sjq.empty()	)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_3")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)4)	;
				EXPECT_FALSE(	sjq.empty()	)	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_4")	;

				sjq.clear()	;
				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;
				EXPECT_TRUE(	sjq.empty()	)	;
}

TEST_F(	ScheduledJobQueueTest	,	PushJobsbySimObjectOrder	)	{
	//req.add_requirement("512154259");

				Trick::JobData	*	job_ptr	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_3")	;
				job_ptr->sim_object_id	=	3	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4")	;
				job_ptr->sim_object_id	=	4	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2")	;
				job_ptr->sim_object_id	=	2	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)4)	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_4")	;

}

TEST_F(	ScheduledJobQueueTest	,	PushJobsbyPhaseOrder	)	{
	//req.add_requirement("3144632784");

				Trick::JobData	*	job_ptr	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_3",	"",	3)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4",	"",	4)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2",	"",	2)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)4)	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_4")	;

}

TEST_F(	ScheduledJobQueueTest	,	PushJobsbyJobClassOrder	)	{
	//req.add_requirement("925196430");

				Trick::JobData	*	job_ptr	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_103",	NULL,	1.0	,	"job_3",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	103	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_101",	NULL,	1.0	,	"job_4",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	104	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_102",	NULL,	1.0	,	"job_1",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	101	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	102	;
				sjq.push(job_ptr)	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)4)	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_4")	;

}

TEST_F(	ScheduledJobQueueTest	,	PushJobsIgnoreSimObject	)	{
	//req.add_requirement("815793485");

				Trick::JobData	*	job_ptr	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)0)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_103",	NULL,	1.0	,	"job_1",	"",	1)	;
				job_ptr->sim_object_id	=	3	;
				job_ptr->job_class	=	100	;
				sjq.push_ignore_sim_object(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_101",	NULL,	1.0	,	"job_2",	"",	1)	;
				job_ptr->sim_object_id	=	4	;
				job_ptr->job_class	=	100	;
				sjq.push_ignore_sim_object(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_102",	NULL,	1.0	,	"job_3",	"",	1)	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push_ignore_sim_object(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4",	"",	1)	;
				job_ptr->sim_object_id	=	2	;
				job_ptr->job_class	=	100	;
				sjq.push_ignore_sim_object(job_ptr)	;

				EXPECT_EQ(	sjq.size()	,	(unsigned	int)4)	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.get_next_job()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_4")	;

}

TEST_F(	ScheduledJobQueueTest	,	TopJob	)	{
	//req.add_requirement("");

				Trick::JobData	*	job_ptr	;

				EXPECT_TRUE(	sjq.top()	==	NULL	)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_3")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	sjq.top()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.top()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				sjq.get_next_job()	;
				job_ptr	=	sjq.top()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				sjq.reset_curr_index()	;
				job_ptr	=	sjq.top()	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;
}

TEST_F(	ScheduledJobQueueTest	,	FindNextJob	)	{
	//req.add_requirement("3664773300	1758653947");

				Trick::JobData	*	job_ptr	;
				long	long	curr_time	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	2.0	,	"job_2")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	4.0	,	"job_3")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				sjq.push(job_ptr)	;

				//	Time	=	0.0
				curr_time	=	0	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	1.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	1000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	2.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	2000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	3.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	3000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	4.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	4000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	5.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	5000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;
}

TEST_F(	ScheduledJobQueueTest	,	TestNextJob	)	{
	//req.add_requirement("1758653947	3664773300");

				Trick::JobData	*	job_ptr	;
				long	long	curr_time	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	3.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				job_ptr->system_job_class	=	true	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				job_ptr->system_job_class	=	true	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	2.0	,	"job_3")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				job_ptr->cycle_tics	=	(long	long)(job_ptr->cycle	*	1000000)	;
				job_ptr->stop_tics	=	1000000000	;
				job_ptr->system_job_class	=	true	;
				sjq.push(job_ptr)	;

				//	Time	=	0.0
				curr_time	=	0	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	0	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	0	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	0	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	1000000	)	;

				//	Time	=	1.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	1000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	2000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	2.0
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	2000000	)	;
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	2000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_3")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	2000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;

				//	Time	=	3.0
				sjq.reset_curr_index()	;
				sjq.set_next_job_call_time(1000000000)	;
				curr_time	=	sjq.get_next_job_call_time()	;
				EXPECT_EQ(	curr_time	,	3000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_1")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	3000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_STREQ(	job_ptr->name.c_str()	,	"job_2")	;
				job_ptr->next_tics	+=	job_ptr->cycle_tics	;
				sjq.test_next_job_call_time(job_ptr	,	curr_time)	;
				EXPECT_EQ(	sjq.get_next_job_call_time()	,	3000000	)	;

				job_ptr	=	sjq.find_next_job(curr_time)	;
				EXPECT_TRUE(	job_ptr	==	NULL	)	;
}

TEST_F(	ScheduledJobQueueTest	,	InstrumentBeforeAll	)	{
	//req.add_requirement("3990429752");

				Trick::JobData	*	job_ptr	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_1")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_2")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_3")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"class_100",	NULL,	1.0	,	"job_4")	;
				job_ptr->sim_object_id	=	1	;
				job_ptr->job_class	=	100	;
				sjq.push(job_ptr)	;

				job_ptr	=	new	Trick::JobData(0,	2	,	"instrumentation",	NULL,	1.0	,	"instrument_1")	;
				sjq.instrument_before(	job_ptr	)	;

}

}

