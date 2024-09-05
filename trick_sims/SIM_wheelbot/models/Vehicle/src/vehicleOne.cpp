/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	two	wheeled	robotic	vehicle.)
LIBRARY	DEPENDENCY:
				((vehicleOne.o)
					(Control/src/vehicleController.o)
					(Control/src/differentialDriveController.o)
					(Motor/src/DCMotorSpeedController.o)
					(Motor/src/DCMotor.o))
PROGRAMMERS:
				(((John	M.	Penn)	(L3	Communications)	(June	2015)	(Trick	Refresher	Project)))
*******************************************************************************/
#include	"Vehicle/include/vehicleOne.hh"
#include	<iostream>
#include	<math.h>

int	VehicleOne::default_data()	{

				distanceBetweenWheels	=	0.183;
				wheelRadius	=	0.045;
				vehicleMass		=	2.0;
				double	axelRadius	=	0.5	*	distanceBetweenWheels;
				ZAxisMomentofInertia	=	0.5	*	vehicleMass	*	axelRadius	*	axelRadius;

				//	Vehicle	Controller	Parameters
				slowDownDistance	=	0.5;
				arrivalDistance		=	0.1;
				wheelSpeedLimit	=	8.880;
				headingRateLimit	=	M_PI/4;

				//	DCMotor	Parameters
				//	At	5v	the	following	parameters	will	result	in	a	current	of
				//	0.5	amperes,	and	a	torque	of	0.5	x	0.15	=	0.075	Newton-meters.
				DCMotorInternalResistance	=	10.0;	//	Ohms
				DCMotorTorqueConstant					=	0.15;	//	Newton-meters	/	Ampere

			//	Assuming	torque	=	0.075	Nm,	then	the	wheel	force	due	to
			//	torque	=	0.075	Nm	/	0.045	=	1.67N.	If	we	want	the	wheel	force
			//	to	be	0	when	wheel	speed	=	0.4	m/s
			//	0.075	=	wheelDragConstant	*	0.4
			//	wheelDragConstant	=	0.075/0.4	=	1.875
				wheelDragConstant		=	1.875;
				corningStiffness	=	10.0;

				//	Dynamic	State
				position[0]	=	0.0;
				position[1]	=	0.0;

				velocity[0]	=	0.0;
				velocity[1]	=	0.0;

				acceleration[0]	=	0.0;
				acceleration[1]	=	0.0;

				heading					=	0.0;
				headingRate	=	0.0;
				headingAccel	=	0.0;

				//	Feedback	to	Motors
				rightMotorSpeed	=	0.0;
				leftMotorSpeed		=	0.0;

				batteryVoltage		=	5.0;

				//	Initialize	homing	variables
				homeCommanded	=	0;
				endofHoming	=	false;

				return	0;
}

int	VehicleOne::state_init()	{


				Point	initLocation(	position[0],
																								position[1]);

				rightDCMotor	=	new	DCMotor(	DCMotorInternalResistance,	DCMotorTorqueConstant	);
				leftDCMotor		=	new	DCMotor(	DCMotorInternalResistance,	DCMotorTorqueConstant	);

				//	Note	that	right	and	left	motor	speeds	are	passed	by	reference.
				rightMotorController	=	new	DCMotorSpeedController(*rightDCMotor,	.3,	rightMotorSpeed,	batteryVoltage	);
				leftMotorController		=	new	DCMotorSpeedController(*leftDCMotor,		.3,	leftMotorSpeed,		batteryVoltage);

				driveController	=
								new	DifferentialDriveController(distanceBetweenWheels,
																																								wheelRadius,
																																								wheelSpeedLimit,
																																								headingRateLimit,
																																								slowDownDistance,
																																								*rightMotorController,
																																								*leftMotorController);

				navigator	=
								new	Navigator(heading,	initLocation);

				vehicleController	=
								new	VehicleController(	&waypointQueue,	*navigator,	*driveController,	arrivalDistance);

				return	(0);
}

void	VehicleOne::add_waypoint(double	x,	double	y)	{
				Point	wayPoint(x,y);
				waypointQueue.push_back(wayPoint);
}


void	VehicleOne::control()	{

				//	Perfect	Sensors	for	now.
				navigator->setHeading(heading);
				navigator->setLocation(position[0],	position[1]);

				//	Check	to	see	if	the	variable	server	client	input	for	homeCommanded	has	been	activated
				//	if	so,	go	home	and	declare	end	of	simulation
				if	(homeCommanded	==	1	&&	endofHoming	==	false)	{
								vehicleController->gohome();
								endofHoming	=	true;
				}
				vehicleController->update();
}


int	VehicleOne::state_deriv()	{

			double	speed	=	sqrt(velocity[0]	*	velocity[0]	+	velocity[1]	*	velocity[1]);

			//	Direction	that	the	vehicle	is	moving.
			double	velocity_unit[2];
			velocity_unit[0]	=	velocity[0]	/	speed;
			velocity_unit[1]	=	velocity[1]	/	speed;

			//	Direction	that	the	vehicle	is	pointing.
			double	heading_unit[2];
			heading_unit[0]	=		cos(heading);
			heading_unit[1]	=		sin(heading);

			//	Meters/second,	positive	forward
			double	turningSpeed	=	0.5	*	distanceBetweenWheels	*	headingRate;

			//	Motorspeed:	(radians/second),	positive	counter-clockwise.
			//	Feedback	to	the	motor	controllers.
			rightMotorSpeed	=	-(speed	+	turningSpeed)	/	wheelRadius;
			leftMotorSpeed		=		(speed	-	turningSpeed)	/	wheelRadius;


			//	Traction	Force:	Newtons	positive	forward
			double	leftWheelTractionForce		=		leftDCMotor->getTorque()		/	wheelRadius;
			double	rightWheelTractionForce	=	-rightDCMotor->getTorque()	/	wheelRadius;
			double	driveForceMag	=	leftWheelTractionForce	+	rightWheelTractionForce;

			//	Traction	Torque
			vehicleZTorque	=	(rightWheelTractionForce	-	leftWheelTractionForce)	*	(0.5	*	distanceBetweenWheels)	;

			driveForce[0]	=	cos(heading)	*	driveForceMag;
			driveForce[1]	=	sin(heading)	*	driveForceMag;

			//	Lateral	Tire	(Turning)	Force
			if	(speed	==	0.0)	{
							lateralTireForce[0]	=	0.0;
							lateralTireForce[1]	=	0.0;
			}	else	{
							double	tireSlip[2];
							tireSlip[0]	=	heading_unit[0]	-	velocity_unit[0];
							tireSlip[1]	=	heading_unit[1]	-	velocity_unit[1];

							lateralTireForce[0]	=	corningStiffness	*	tireSlip[0];
							lateralTireForce[1]	=	corningStiffness	*	tireSlip[1];
			}

			//	Rolling	Tire	Resistance	Force
			rollingResistForce[0]	=	-velocity[0]	*	wheelDragConstant;
			rollingResistForce[1]	=	-velocity[1]	*	wheelDragConstant;

			//	Total	Body	Force
			forceTotal[0]	=	driveForce[0]	+	lateralTireForce[0]	+	rollingResistForce[0];
			forceTotal[1]	=	driveForce[1]	+	lateralTireForce[1]	+	rollingResistForce[1];

			//	Body	Rotational	Acceleration
			headingAccel	=	vehicleZTorque	/	ZAxisMomentofInertia;

			//	If	the	simulation	is	at	the	end,	the	vehicle	stops	moving
			if	(vehicleController->getStatus()	==	true)	{
						forceTotal[0]	=	0;
						forceTotal[1]	=	0;
						rightMotorSpeed	=	0;
						leftMotorSpeed	=	0;
						velocity[0]	=	0;
						velocity[1]	=	0;
						headingRate	=	0;
						headingAccel	=	0;
			}

			//	Body	Linear	Acceleration
			acceleration[0]	=	forceTotal[0]	/	vehicleMass;
			acceleration[1]	=	forceTotal[1]	/	vehicleMass;

			return	0;
}

#include	"trick/integrator_c_intf.h"

int	VehicleOne::state_integ()	{
				int	integration_step;

				load_state(
																&heading,
																&headingRate,
																&position[0],
																&position[1],
																&velocity[0],
																&velocity[1],
																(double*)0
														);

				load_deriv(
																&headingRate,
																&headingAccel,
																&velocity[0],
																&velocity[1],
																&acceleration[0],
																&acceleration[1],
																(double*)0
														);

				integration_step	=	integrate();

				unload_state(
																&heading,
																&headingRate,
																&position[0],
																&position[1],
																&velocity[0],
																&velocity[1],
																(double*)0
														);

				if	(!integration_step)	{
								if	(heading	<	-M_PI)	heading	+=		2*M_PI;
								if	(heading	>		M_PI)	heading	+=	-2*M_PI;
				}

				return(integration_step);

}
