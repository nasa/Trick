#include	<gtest/gtest.h>
#include	"findpath.hh"
#include	"arena.hh"
#include	<algorithm>
#include	<vector>

TEST(FindPathTest,	FindPath_one)
{
	//Tests	the	failure	case	of	the	FindPath	function
	//This	is	when	the	goal	is	completely	blocked
	Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(2,1);
	GridSquare	*goal	=	arena.getGridSquare(7,5);

	arena.block(7,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(6,5);
	arena.block(8,5);
	arena.block(6,6);
	arena.block(8,6);

	std::cout	<<	std::endl;

	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);

	std::cout	<<	std::endl;
	std::cout	<<	std::endl;

	EXPECT_EQ(returnvalue.size(),0);

}

TEST(FindPathTest,	FindPath_two)
{
	//Tests	if	FindPath	finds	a	path
	Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(2,1);
	GridSquare	*goal	=	arena.getGridSquare(7,5);


	arena.block(2,0);
	arena.block(7,1);
	arena.block(0,2);
	arena.block(1,2);
	arena.block(3,2);
	arena.block(6,2);
	arena.block(3,3);
	arena.block(6,3);
	arena.block(8,3);
	arena.block(2,4);
	arena.block(3,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(2,5);
	arena.block(6,6);
	arena.block(7,6);

	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);

	EXPECT_GT(returnvalue.size(),0);

}

TEST(FindPathTest,	FindPath_three)
{
	//Tests	if	FindPath	finds	a	path	when	start	and	goal	are	the	same
	Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(2,1);
	GridSquare	*goal	=	arena.getGridSquare(2,1);


	arena.block(2,0);
	arena.block(7,1);
	arena.block(0,2);
	arena.block(1,2);
	arena.block(3,2);
	arena.block(6,2);
	arena.block(3,3);
	arena.block(6,3);
	arena.block(8,3);
	arena.block(2,4);
	arena.block(3,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(2,5);
	arena.block(6,6);
	arena.block(7,6);

	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);

	EXPECT_EQ(returnvalue.size(),1);

}

TEST(FindPathTest,	FindPath_four)
{
	//Tests	if	FindPath	finds	a	path	when	start	is	outside	the	arena
	Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(11,13);
	GridSquare	*goal	=	arena.getGridSquare(2,1);



	arena.block(2,0);
	arena.block(7,1);
	arena.block(0,2);
	arena.block(1,2);
	arena.block(3,2);
	arena.block(6,2);
	arena.block(3,3);
	arena.block(6,3);
	arena.block(8,3);
	arena.block(2,4);
	arena.block(3,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(2,5);
	arena.block(6,6);
	arena.block(7,6);

	std::cout	<<	std::endl;
	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);
	std::cout	<<	std::endl;
	std::cout	<<	std::endl;

	EXPECT_EQ(returnvalue.size(),0);

}

TEST(FindPathTest,	FindPath_five)
{
	//Tests	if	FindPath	finds	a	path	when	goal	is	outside	the	arena
	Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(2,1);
	GridSquare	*goal	=	arena.getGridSquare(11,12);


	arena.block(2,0);
	arena.block(7,1);
	arena.block(0,2);
	arena.block(1,2);
	arena.block(3,2);
	arena.block(6,2);
	arena.block(3,3);
	arena.block(6,3);
	arena.block(8,3);
	arena.block(2,4);
	arena.block(3,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(2,5);
	arena.block(6,6);
	arena.block(7,6);

	std::cout	<<	std::endl;

	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);

	std::cout	<<	std::endl;
	std::cout	<<	std::endl;

	EXPECT_EQ(returnvalue.size(),0);

}

/*TEST(FindPathTest,	FindPath_three)
{
		//Tests	if	FindPath	returns	the	correct	values
		Arena	arena(10,7);
	Arena*	testarena;
	testarena	=	&arena;

	GridSquare	*origin	=	arena.getGridSquare(2,1);
	GridSquare	*goal	=	arena.getGridSquare(7,5);


	arena.block(2,0);
	arena.block(7,1);
	arena.block(0,2);
	arena.block(1,2);
	arena.block(3,2);
	arena.block(6,2);
	arena.block(3,3);
	arena.block(6,3);
	arena.block(8,3);
	arena.block(2,4);
	arena.block(3,4);
	arena.block(6,4);
	arena.block(8,4);
	arena.block(2,5);
	arena.block(6,6);
	arena.block(7,6);

	std::vector<Point>	desiredvalues	(7);

	desiredvalues[0].x	=	2;
	desiredvalues[0].y	=	1;
	desiredvalues[1].x	=	3;
	desiredvalues[1].y	=	1;
	desiredvalues[2].x	=	4;
	desiredvalues[2].y	=	2;
	desiredvalues[3].x	=	5;
	desiredvalues[3].y	=	3;
	desiredvalues[4].x	=	5;
	desiredvalues[4].y	=	4;
	desiredvalues[5].x	=	6;
	desiredvalues[5].y	=	5;
	desiredvalues[6].x	=	7;
	desiredvalues[6].y	=	5;

	std::vector<Point>	returnvalue	=	FindPath(origin,goal,testarena);

	EXPECT_EQ(desiredvalues[0].x,	returnvalue[0].x);
	EXPECT_EQ(desiredvalues[0].y,	returnvalue[0].y);
	EXPECT_EQ(desiredvalues[1].x,	returnvalue[1].x);
	EXPECT_EQ(desiredvalues[1].y,	returnvalue[1].y);
	EXPECT_EQ(desiredvalues[2].x,	returnvalue[2].x);
	EXPECT_EQ(desiredvalues[2].y,	returnvalue[2].y);
	EXPECT_EQ(desiredvalues[3].x,	returnvalue[3].x);
	EXPECT_EQ(desiredvalues[3].y,	returnvalue[3].y);
	EXPECT_EQ(desiredvalues[4].x,	returnvalue[4].x);
	EXPECT_EQ(desiredvalues[4].y,	returnvalue[4].y);
	EXPECT_EQ(desiredvalues[5].x,	returnvalue[5].x);
	EXPECT_EQ(desiredvalues[5].y,	returnvalue[5].y);
	EXPECT_EQ(desiredvalues[6].x,	returnvalue[6].x);
	EXPECT_EQ(desiredvalues[6].y,	returnvalue[6].y);
}*/
