/*	Listed	requirements	are	under	requirement	Trick-163	(3.7.5.2)	*/
/*		Real-time	control	shall	be	able	to	use	a	system	clock	source	*/

#define	protected	public
#define	TOL	2e3

#include	<iostream>
#include	<sys/types.h>
#include	<map>
//#include	<signal.h>
#include	<unistd.h>

#include	"gtest/gtest.h"
#include	"trick/ITimer.hh"
#include	"trick/GetTimeOfDayClock.hh"
#include	"trick/SimObject.hh"
//#include	"trick/RequirementScribe.hh"

namespace	Trick	{

class	ITimerTest	:	public	testing::Test	{

	public:
		Trick::GetTimeOfDayClock	dClk;
		//Trick::RequirementScribe	req;
		double	sec;
		long	long	tim_st,	tim_elap;

		ITimerTest()	{}
		~ITimerTest()	{}
		virtual	void	SetUp()	{}
		virtual	void	TearDown()	{}
};

/*	Proper	initialization	of	ITimer	*/
TEST_F(ITimerTest,	Initialize)	{
	//req.add_requirement("timer");
	//"The	ITimer	shall	initialize	with	disabled	and	inactive	status.");

	Trick::ITimer	*iTim;
	iTim	=	new	Trick::ITimer;

				iTim->init();

	EXPECT_FALSE(iTim->get_enabled());
	EXPECT_FALSE(iTim->active);

	iTim->enable();
	EXPECT_TRUE(iTim->get_enabled());

	iTim->disable();
	EXPECT_FALSE(iTim->get_enabled());

	delete	iTim;
}

/*	Timer	not	enabled;	ensure	no	activity	when	starting,	pausing,	etc	*/
TEST_F(ITimerTest,	TimerNotEnabled)	{
				//req.add_requirement("timer");
	//"The	ITimer	shall	start	if	and	only	if	the	ITimer	is	enabled	to	do	so.");


	bool	active_ch;

				Trick::ITimer	*iTim;
	iTim	=	new	Trick::ITimer;

	active_ch	=	iTim->active;
	sec	=	0.05;

	iTim->init();

	iTim->start(sec);
	EXPECT_EQ(iTim->active,	active_ch);

	iTim->set_active(~active_ch);
	active_ch	=	iTim->active;
	iTim->start(sec);
	EXPECT_EQ(iTim->active,	active_ch);

	delete	iTim;
}

/*	Tolerance:	2	ms	*/
TEST_F(ITimerTest,	TimerStartSuccess)	{
				//req.add_requirement("timer");
	//"The	ITimer	shall	sleep	when	commanded,	thereby	releasing	the	processor	for	a	specified	amount	of	time.");

	sec	=	0.05;

	Trick::ITimer	*iTim;
	iTim	=	new	Trick::ITimer;

	iTim->init();
	iTim->enable();

	tim_st	=	dClk.wall_clock_time();

	iTim->start(sec);
	ASSERT_TRUE(iTim->active);

	/*	Cause	simulation	to	wait	until	"sec"	seconds	have	passed	*/
	iTim->pause();

	tim_elap	=	dClk.wall_clock_time()	-	tim_st;
	EXPECT_NEAR((tim_elap+2000),	sec*1e6,	TOL);

	delete	iTim;
}

TEST_F(ITimerTest,	TimerStartReset)	{
				//req.add_requirement("timer");
	//"The	ITimer	shall	reset	to	a	new	alarm	time	when	commanded	if	the	current	time	until	alarm	has	not	yet	expired.");

				sec	=	0.05;

				Trick::ITimer	*iTim;
				iTim	=	new	Trick::ITimer;

				iTim->init();
				iTim->enable();

				iTim->start(sec);
	usleep(40000);

	/*	This	is	equivalent	to	saying	start(2.0*sec),	both	are	valid	*/
	iTim->reset(2.0*sec);
	//iTim->start(2.0*sec);

	tim_st	=	dClk.wall_clock_time();
	iTim->pause();
	tim_elap	=	dClk.wall_clock_time()	-	tim_st;

	EXPECT_NEAR((tim_elap+2000),	(2*sec*1e6),	TOL);

				delete	iTim;
}

TEST_F(ITimerTest,	TimerStartStop)	{
				//req.add_requirement("timer");
	//"The	ITimer	shall	stop	when	commanded	if	the	time	until	alarm	has	not	yet	expired.");

				sec	=	0.05;

				Trick::ITimer	*iTim;
				iTim	=	new	Trick::ITimer;

				iTim->init();
				iTim->enable();

	tim_st	=	dClk.wall_clock_time();
				iTim->start(sec);
				usleep(40000);
	iTim->stop();
	tim_elap	=	dClk.wall_clock_time()	-	tim_st;
	EXPECT_NEAR((tim_elap+2000),	40000,	(TOL+4000));

	//std::cout	<<	TOL+4000	<<	std::endl;

				EXPECT_FALSE(iTim->active);
	tim_st	=	dClk.wall_clock_time();
	iTim->pause();
				tim_elap	=	dClk.wall_clock_time()	-	tim_st;

	EXPECT_NEAR(tim_elap,	0,	TOL);


				delete	iTim;
}


TEST_F(ITimerTest,	TimerStartFailure)	{
				//req.add_requirement("timer");
	//"The	ITimer	shall	not	initiate	with	start	times	that	are	negative	or	less	than	10	ms");

	sec	=	0.05;

				Trick::ITimer	*iTim;
				iTim	=	new	Trick::ITimer;

				iTim->init();
				iTim->enable();

	/*Passing	negative	time	to	elapse;	expecting	throw	*/
				EXPECT_ANY_THROW(iTim->start(-sec));

	/*	Cannot	activate	timer	with	<	10	ms	*/
	sec	=	0.005;
	iTim->start(sec);
	EXPECT_FALSE(iTim->active);

				delete	iTim;
}

}
