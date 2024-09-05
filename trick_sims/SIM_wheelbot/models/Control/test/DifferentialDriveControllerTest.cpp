#include	<gtest/gtest.h>
#define	private	public
#include	"Control/include/testMotorController.hh"
#include	"Control/include/differentialDriveController.hh"
#include	<algorithm>

#ifndef	PI
#define	PI	3.1415926535
#endif
#define	FLOAT_TOLERANCE	0.000001

/*
				Test	Fixture.
*/
class	differentialDriveControllerTest	:	public	::testing::Test	{
				protected:

				TestMotorController*	rightMotorController;
				TestMotorController*	leftMotorController;
				DifferentialDriveController*	driveController;

				differentialDriveControllerTest()	{
				rightMotorController	=
								new	TestMotorController();
				leftMotorController	=
								new	TestMotorController();
				driveController	=
								new	DifferentialDriveController(0.183,	0.045,	8.880,	0.200,
																*rightMotorController,	*leftMotorController);
				}

				~differentialDriveControllerTest()	{
								delete	driveController;
								delete	rightMotorController;
								delete	leftMotorController;
				}
};

TEST_F(	differentialDriveControllerTest	,	constructor)	{

				EXPECT_NEAR(0.045,	driveController->wheelRadius,	FLOAT_TOLERANCE);
				EXPECT_NEAR(0.183,	driveController->distanceBetweenWheels,	FLOAT_TOLERANCE);
				EXPECT_NEAR(8.880,	driveController->wheelRotationRateLimit,	FLOAT_TOLERANCE);
				EXPECT_NEAR(0.200,	driveController->slowDownDistance,	FLOAT_TOLERANCE);
}

TEST_F(	differentialDriveControllerTest	,	setDistanceBetweenWheels	)	{

				int	result;

				result	=	driveController->setDistanceBetweenWheels(0.1);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(0.1,	driveController->distanceBetweenWheels,	FLOAT_TOLERANCE);

				result	=	driveController->setDistanceBetweenWheels(0.2);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(0.2,	driveController->distanceBetweenWheels,	FLOAT_TOLERANCE);

				result	=	driveController->setDistanceBetweenWheels(-0.3);
				EXPECT_EQ(1,result);
				EXPECT_NEAR(0.2,	driveController->distanceBetweenWheels,	FLOAT_TOLERANCE);
}

TEST_F(	differentialDriveControllerTest	,	setWheelRadius	)	{

				int	result;
				result	=	driveController->setWheelRadius(0.059);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(0.059,	driveController->wheelRadius,	FLOAT_TOLERANCE);

				result	=	driveController->setWheelRadius(0.083);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(0.083,	driveController->wheelRadius,	FLOAT_TOLERANCE);

				result	=	driveController->setWheelRadius(-0.075);
				EXPECT_EQ(1,result);
				EXPECT_NEAR(0.083,	driveController->wheelRadius,	FLOAT_TOLERANCE);
}

TEST_F(	differentialDriveControllerTest	,	setWheelRotationRateLimit	)	{

				int	result;
				result	=	driveController->setWheelRotationRateLimit(7.123);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(7.123,	driveController->wheelRotationRateLimit,	FLOAT_TOLERANCE);

				result	=	driveController->setWheelRotationRateLimit(5.234);
				EXPECT_EQ(0,result);
				EXPECT_NEAR(5.234,	driveController->wheelRotationRateLimit,	FLOAT_TOLERANCE);

				result	=	driveController->setWheelRotationRateLimit(-4.987);
				EXPECT_EQ(1,result);
				EXPECT_NEAR(5.234,	driveController->wheelRotationRateLimit,	FLOAT_TOLERANCE);
}

TEST_F(	differentialDriveControllerTest	,	PositiveRangeErrorOnly)	{

				//	If	there	is	no	heading	error,	and	the	distance	error	is	non-zero,
				//	then	the	speeds	should	be	the	same,	and	equal	to	the	wheelRotationRateLimit.

				double	distance_err	=	1.0;
				double	heading_err		=	0.0;
				driveController->update(distance_err,	heading_err);

				double	rightMotorSpeedCommand,	leftMotorSpeedCommand;
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				//	the	speeds	should	be	the	same
				EXPECT_NEAR(rightMotorSpeedCommand,	leftMotorSpeedCommand,	FLOAT_TOLERANCE);
				//	and	equal	to	the	setWheelRotationRateLimit.
				EXPECT_NEAR(rightMotorSpeedCommand,	8.880,	FLOAT_TOLERANCE);
}

TEST_F(	differentialDriveControllerTest	,	positiveHeadingError	)	{

				//	If	the	heading	error	is	positive,	then	we	should	turn	to	the	right,
				//	meaning	that	the	left	wheel	should	move	faster	than	the	right	wheel.

				double	rightMotorSpeedCommand,	leftMotorSpeedCommand;

				driveController->update(0.0,	0.1*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(50.0,	30*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(100.0,	60*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(0.0,	89*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(0.0,	90*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(50.0,	91*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(100.0,	120*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(0.0,	150*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);

				driveController->update(50.0,	179*(PI/180.0));
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(leftMotorSpeedCommand,	rightMotorSpeedCommand);
}

TEST_F(	differentialDriveControllerTest,	negativeHeadingError	)	{

				//	If	the	heading	error	is	negative,	then	we	should	turn	to	the	left,
				//	meaning	that	the	right	wheel	should	move	faster	than	the	left	wheel.

				double	distance_err	=	0.0;
				double	heading_err		=	-30*(PI/180.0);
				driveController->update(distance_err,	heading_err);

				double	rightMotorSpeedCommand,	leftMotorSpeedCommand;
				driveController->getCommandedMotorSpeeds(leftMotorSpeedCommand,	rightMotorSpeedCommand);
				EXPECT_GT(rightMotorSpeedCommand,	leftMotorSpeedCommand);
}


