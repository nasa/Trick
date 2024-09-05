/*	Listed	requirements	are	under	requirement	Trick-153	(3.7.1.1)	*/
/*				Real-time	control	shall	be	able	to	use	a	system	clock	source	*/

#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>
#include	<sys/time.h>
#include	<map>

#include	"gtest/gtest.h"
#include	"trick/GetTimeOfDayClock.hh"
#include	"trick/JobData.hh"
//#include	"trick/RequirementScribe.hh"

#define	TIME_TOL	1e3
#define	RATIO_TOL	1e-9
#define	TICS_PER_SEC	1000000

//	Stub	for	message_publish
extern	"C"	int	message_publish(int	level,	const	char	*	format_msg,	...)	{	(void)level;	(void)format_msg;	return	0;	}

class	GetTimeOfDayClockTest	:	public	::testing::Test	{

				protected:
								Trick::GetTimeOfDayClock	dClk;
								//Trick::RequirementScribe	req;

								GetTimeOfDayClockTest()	{}
								~GetTimeOfDayClockTest()	{}
								virtual	void	SetUp()	{}
								virtual	void	TearDown()	{}
}	;

/*	Ensure	clock	initializes	correctly	*/
TEST_F(GetTimeOfDayClockTest,	Initialize)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	initialize	with	time	equal	to	computer	system	time,	reference	time	of	0,	and	all	ratios	set	to	1.");

				struct	timeval	res;
				Trick::GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				timclk->clock_init();

				/*	General	expected	clock	initialization	*/
				ASSERT_EQ(timclk->get_rt_clock_ratio()	,	1.0);
				ASSERT_EQ(timclk->sim_tic_ratio,	1.0);
				ASSERT_EQ(timclk->ref_time_tics,	0);

				/*	Time	of	Day	clock	initialization	tests	*/
				EXPECT_STREQ(timclk->get_name()	,	"GetTimeOfDay	-	CLOCK_REALTIME");

				gettimeofday(&res,	NULL);

				EXPECT_NEAR(timclk->wall_clock_time(),	(res.tv_sec	*	1000000LL	+	res.tv_usec),	TIME_TOL);
}

TEST_F(GetTimeOfDayClockTest,	ClockSimRatio)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	provide	the	ability	to	change	the	sim	time-to-real	time	ratio.");

				long	long	tim_curr;
				double	tic_ratio,	tic_val;
				Trick::GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				tic_val	=	100000;

				timclk->calc_sim_time_ratio(tic_val);

				usleep(100000);
				tim_curr	=	timclk->wall_clock_time();
				tic_ratio	=	(double)	timclk->clock_time()	/	(double)	tim_curr;

				EXPECT_NEAR(tic_ratio,	(double)	tic_val	/	(double)	TICS_PER_SEC,	RATIO_TOL);

				delete	timclk;
}

TEST_F(GetTimeOfDayClockTest,	ClockRTRatio)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	provide	the	ability	to	adjust	the	reference	time	ratio.");

				long	long	tim_curr,	tim_diff;
				double	tic_ratio;
				int	rt_ratio;
				Trick::GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				rt_ratio	=	6.0;
				ASSERT_EQ(timclk->sim_tic_ratio,	1);

				timclk->calc_sim_time_ratio(TICS_PER_SEC);

				timclk->set_rt_clock_ratio(rt_ratio);
				tim_diff	=	timclk->wall_clock_time()	-	timclk->ref_time_tics;
				EXPECT_NEAR(tim_diff,	(long	long)	(timclk->wall_clock_time()	/	rt_ratio),	TIME_TOL);

				timclk->set_reference(0);

				usleep(100000);
				tim_curr	=	timclk->wall_clock_time();
				tic_ratio	=	(double)	timclk->clock_time()	/	(double)	tim_curr;

				EXPECT_NEAR(tic_ratio,	rt_ratio,	RATIO_TOL);

				delete	timclk;
}

TEST_F(GetTimeOfDayClockTest,	ClockSpin)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	provide	the	ability	to	spin	(wait)	until	a	specified	time	is	reached.");

				long	long	tim_curr,	spin_time;
				Trick::	GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				spin_time	=	100000;

				tim_curr	=	timclk->wall_clock_time();

				dClk.clock_spin(tim_curr	+	spin_time);

				//	We	know	that	the	time	difference	must	be	at	least	as	long	as	the	spin_time.
				//	But	that's	all	we	know.
				EXPECT_GE((timclk->wall_clock_time()	-	tim_curr),	spin_time);

				delete	timclk;
}

/*	Change	and	test	reference	times	*/
TEST_F(GetTimeOfDayClockTest,	ReferenceTime)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	provide	the	ability	to	set	a	reference	time.");

				long	long	tic_adjust;
				Trick::	GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				timclk->clock_init();

				tic_adjust	=	50000;
				timclk->set_reference(tic_adjust);

				EXPECT_NEAR(timclk->clock_time(),	timclk->wall_clock_time()	-	tic_adjust,	TIME_TOL);

				timclk->adjust_ref_time(2*tic_adjust);

				EXPECT_EQ(timclk->ref_time_tics,	tic_adjust	+	2*tic_adjust);

				delete	timclk;
}

TEST_F(GetTimeOfDayClockTest,	ClockSync)	{
				//req.add_requirement("timeofday_clock");
				//"The	system	clock	shall	provide	the	ability	to	sync	reference	time	to	wall	clock	time");

				double	align_tics_mult;
				int	clock_unit;
				Trick::GetTimeOfDayClock	*	timclk	=	new	Trick::GetTimeOfDayClock;

				align_tics_mult	=	1.5;
				clock_unit	=	int((TICS_PER_SEC	*	align_tics_mult)	/
																				(timclk->rt_clock_ratio	*	timclk->sim_tic_ratio));

				timclk->sync_to_wall_clock(align_tics_mult,	TICS_PER_SEC);

				EXPECT_EQ((timclk->ref_time_tics%clock_unit),	0);

				delete	timclk;
}

