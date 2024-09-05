#include	<gtest/gtest.h>
#define	private	public
#include	"navigator.hh"
#include	<math.h>

#define	PI	3.14159265358979
#define	FP_TOLERANCE	0.000000001

TEST(NavigatorTest,	distanceTo_one)	{

		Point	location(0.0,	0.0);
		double	heading	=	0.0;
		Navigator	navigator(heading,	location);
		Point	mapPoint(3.0,	4.0);
		double	distance	=	navigator.distanceTo(mapPoint);
		EXPECT_NEAR(distance,	5.0,	FP_TOLERANCE);
}

TEST(NavigatorTest,	distanceTo_two)	{

		Point	location(2.0,	2.0);
		double	heading	=	0.0;
		Navigator	navigator(heading,	location);
		Point	mapPoint(5.0,	6.0);
		double	distance	=	navigator.distanceTo(mapPoint);
		EXPECT_NEAR(distance,	5.0,	FP_TOLERANCE);
}

TEST(NavigatorTest,	distanceTo_three)	{
		//Tests	if	distance	is	found	correctly	from	a	negative	location	to	a	positive	mapPoint
		Point	location(-4,-5);
		Navigator	navigator(PI/6,	location);
		Point	mapPoint(6,	9);
		double	distance	=	navigator.distanceTo(mapPoint);

		EXPECT_NEAR(distance,	17.204650534,	FP_TOLERANCE);
}

TEST(NavigatorTest,	convertMapToPlatform_one)
{
		//Tests	if	the	mapPoint	gets	converted	to	platform	correctly
		Point	location(5,4);
		Navigator	navigator(PI/6,	location);
		Point	mapPoint(6,9);
		Point	platformPoint;
		platformPoint	=	navigator.convertMapToPlatform(mapPoint);

		EXPECT_EQ(platformPoint.x,	1);
		EXPECT_EQ(platformPoint.y,	5);
}

TEST(NavigatorTest,	convertMapToPlatform_two)
{
		//Tests	if	the	mapPoint	gets	converted	to	platform	correctly
		//under	slightly	more	strenuous	conditions	than	the	previous	test
		Point	location(-8,-9);
		Navigator	navigator(5*PI/6,	location);
		Point	mapPoint(3,-5);
		Point	platformPoint	=	navigator.convertMapToPlatform(mapPoint);

		EXPECT_EQ(platformPoint.x,	11);
		EXPECT_EQ(platformPoint.y,	4);
}

TEST(NavigatorTest,	convertPlatformToBody_one)
{
		//	If	heading	is	45	degrees	then	<1,0>	in	platform	coordinates	should	be
		//	<sqrt(2)/2,	sqrt(2)/2>	in	body	coordinates.
		double	heading	=	45.0	*	(PI/180.0);
		Point	location(0,0);
		Navigator	navigator(heading,	location);
		Point	platformPoint(1,0);
		Point	bodyPoint	=	navigator.convertPlatformToBody(platformPoint);

		double	expectedResult	=	sqrt(2.0)/2.0;
		EXPECT_NEAR(bodyPoint.x,	expectedResult,	FP_TOLERANCE);
		EXPECT_NEAR(bodyPoint.y,	expectedResult,	FP_TOLERANCE);
}

TEST(NavigatorTest,	convertPlatformToBody_two)
{
		//	If	heading	is	45	degrees,	then	<0,1>	in	platform	coordinates
		//	should	be	<sqrt(2)/2,	sqrt(2)/2>	in	body	coordinates.
		double	heading	=	45.0	*	(PI/180.0);
		Point	location(0,0);
		Navigator	navigator(heading,	location);
		Point	platformPoint(1,0);
		Point	bodyPoint	=	navigator.convertPlatformToBody(platformPoint);

		double	expectedResult	=	sqrt(2.0)/2.0;
		EXPECT_NEAR(bodyPoint.x,	expectedResult,	FP_TOLERANCE);
		EXPECT_NEAR(bodyPoint.y,	expectedResult,	FP_TOLERANCE);
}

TEST(NavigatorTest,	convertBodyToPlatform_one)
{
		//	This	test	is	the	inverse	of	convertPlatformToBody_one.
		double	heading	=	45.0	*	(PI/180.0);
		Point	location(0,0);
		Navigator	navigator(heading,	location);

		double	H		=	sqrt(2.0)/2.0;
		Point	bodyPoint(H,H);
		Point	platformPoint	=	navigator.convertBodyToPlatform(bodyPoint);

		EXPECT_NEAR(platformPoint.x,	1.0,	00001);
		EXPECT_NEAR(platformPoint.y,	0.0,	00001);
}

TEST(NavigatorTest,	convertPlatformToMap_one)
{
		//Tests	if	Platform	points	get	converted	to	mapPoints	correctly
		Point	location(-8,-9);
		Navigator	navigator(PI/6,	location);
		Point	platformPoint(11,4);
		Point	mapPoint;
		mapPoint	=	navigator.convertPlatformToMap(platformPoint);

		EXPECT_EQ	(mapPoint.x,	3);
		EXPECT_EQ	(mapPoint.y,	-5);
}

TEST(NavigatorTest,	bearingTo_one)
{
		Point	location(0,0);
		Navigator	navigator(PI/6,	location);
		Point	mapPoint(3,0);
		double	bearing;
		bearing	=	navigator.bearingTo(mapPoint);

		EXPECT_NEAR	(bearing,	(-PI/6),	FP_TOLERANCE);
}

TEST(NavigatorTest,	bearingTo_two)
{
		Point	location(20,0);
		Navigator	navigator(0.0,	location);
		Point	mapPoint(20,20);
		double	bearing;
		bearing	=	navigator.bearingTo(mapPoint);
		std::cout	<<	"bearing	=	"	<<	bearing	<<	std::endl;
		EXPECT_NEAR	(bearing,	(-PI/6),	FP_TOLERANCE);
}
