#include	<gtest/gtest.h>
#define	private	public
#include	"Control/include/testMotorController.hh"
#include	"Control/include/vehicleController.hh"
#include	<algorithm>

#ifndef	PI
#define	PI	3.1415926535
#endif
#define	FLOAT_TOLERANCE	0.000001

/*
Test	Fixture.
*/
class	vehicleControllerTest	:	public	::testing::Test	{
				protected:
				std::vector<Point>	waypointQueue;
				Navigator	*navigator;
				TestMotorController*	rightMotorController;
				TestMotorController*	leftMotorController;
				DifferentialDriveController*	driveController;
				VehicleController*	vehicleController;

				vehicleControllerTest()	{

								Point	waypoint(1.0,	3.0);
								waypointQueue.push_back(	waypoint);

								Point	initLocation(0.0,	0.0);
								rightMotorController	=	new	TestMotorController();
								leftMotorController		=	new	TestMotorController();
								driveController	=	new	DifferentialDriveController(0.183,	0.045,	8.880,	0.200,
																																																										*rightMotorController,
																																																										*leftMotorController);
								navigator	=	new	Navigator(0.0,	initLocation);
								vehicleController	=	new	VehicleController(	&waypointQueue,	*navigator,	*driveController,	0.02);

				}

				~vehicleControllerTest()	{
								delete	navigator;
								delete	driveController;
				}

				void	SetUp()	{}
				void	TearDown()	{}
};


TEST_F(	vehicleControllerTest	,	one	)	{

				Point	current_destination;
				int	result	=	vehicleController->getCurrentDestination(current_destination);
				EXPECT_EQ(result,	0);
				EXPECT_EQ(current_destination.x,	1);
				EXPECT_EQ(current_destination.y,	3);
}

