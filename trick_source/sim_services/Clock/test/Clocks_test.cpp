#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>

#include	"gtest/gtest.h"
#include	"trick/Clock.hh"
#include	"trick/clock_proto.h"
#include	"trick/GetTimeOfDayClock.hh"
#include	"trick/JobData.hh"

namespace	Trick	{

class	ClocksTest	:	public	::testing::Test	{

	protected:
		Trick::GetTimeOfDayClock	timclk;

		GetTimeOfDayClockTest()	{}
		~GetTimeOfDayClockTest()	{}
		virtual	void	SetUp()	{}
		virtual	void	TearDown()	{}
}	;

TEST_F(ClocksTest,	ExampleTest)	{

}


}

