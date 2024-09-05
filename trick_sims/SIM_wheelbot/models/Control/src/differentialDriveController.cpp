#include	<iostream>
#include	<math.h>
#include	"Control/include/differentialDriveController.hh"
#ifndef	PI
#define	PI	3.14159265
#endif

DifferentialDriveController::
				DifferentialDriveController	(	double	DistanceBetweenWheels,
																																		double	WheelRadius,
																																		double	WheelSpeedLimit,
																																		double	HeadingRateLimit,
																																		double	SlowDownDistance,
																																		MotorSpeedController&	RightMotorController,
																																		MotorSpeedController&	LeftMotorController
																																):
				distanceBetweenWheels(DistanceBetweenWheels),
				wheelRadius(WheelRadius),
				wheelSpeedLimit(WheelSpeedLimit),
				headingRateLimit(HeadingRateLimit),
				slowDownDistance(SlowDownDistance),
				rightMotorController(RightMotorController),
				leftMotorController(LeftMotorController),

				rightMotorSpeedCommand(0.0),
				leftMotorSpeedCommand(0.0),
				desiredHeadingRate(0.0),

				//	PID	Controller	Initialization
				headingctrl(1.0,	0.08,	0.5,	headingRateLimit,
																																		-headingRateLimit,	0.1,	0.1),
				wheelspeedctrl(1.0,	0.082,	0.5,	wheelSpeedLimit,
																																						-wheelSpeedLimit,	0.1,	0.1)
{	}

void	DifferentialDriveController::stop()	{
				rightMotorController.setCommandedSpeed(0.0);
				leftMotorController.setCommandedSpeed(0.0);
}

int	DifferentialDriveController::update(	double	distance_err,		//	m
																																									double	heading_err)	{	//	rad	(-PI..+PI)

				rightMotorSpeedCommand	=	0.0;
				leftMotorSpeedCommand		=	0.0;

				//	If	the	vehicle	heading	is	within	2	degrees	of	the	target,	then	the
				//	heading	desired	heading	rate	is	proportional	to	the	heading	error.
				if	(	cos(heading_err)	>	cos(2.0	*	(PI/180.0)))	{
								//	PID	Controller	output	for	heading
								desiredHeadingRate	=	headingctrl.getOutput(headingRateLimit,	heading_err);
				}	else	{
								if	(heading_err	>	0.0)	{
												desiredHeadingRate	=		headingRateLimit;
								}	else	{
												desiredHeadingRate	=		-headingRateLimit;
								}
				}

				double	wheelSpeedForHeadingRate	=	(desiredHeadingRate	*	distanceBetweenWheels)	/	(2.0	*	wheelRadius);

				double	availableWheelSpeedForRangeRate	=	wheelSpeedLimit	-	fabs(wheelSpeedForHeadingRate);

				double	wheelSpeedForRangeRate;

				if	(distance_err	>	slowDownDistance	)	{
								wheelSpeedForRangeRate	=	availableWheelSpeedForRangeRate;
				}	else	{
								//	PID	Controller	output	for	wheelspeed
								wheelSpeedForRangeRate	=	wheelspeedctrl.getOutput(availableWheelSpeedForRangeRate,	distance_err);
				}

				desiredRangeRate	=	wheelSpeedForRangeRate	*	wheelRadius;

				double	desiredRightWheelSpeed	=		wheelSpeedForRangeRate	+	wheelSpeedForHeadingRate;
				double	desiredLeftWheelSpeed		=		wheelSpeedForRangeRate	-	wheelSpeedForHeadingRate;

				rightMotorSpeedCommand	=	-desiredRightWheelSpeed;
				leftMotorSpeedCommand		=		desiredLeftWheelSpeed;

				rightMotorController.setCommandedSpeed(	rightMotorSpeedCommand);
				leftMotorController.setCommandedSpeed(	leftMotorSpeedCommand);

				return	0;
}
