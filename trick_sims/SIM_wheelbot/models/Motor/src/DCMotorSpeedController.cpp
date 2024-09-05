
#include	"Motor/include/DCMotorSpeedController.hh"
#include	<iostream>

DCMotorSpeedController::DCMotorSpeedController(
								DCMotor&	dc_motor,
								double	Gain,
								const	double&	actual_speed,
								const	double&	supply_voltage)
								:	motorVoltage(0.0),
										motor(dc_motor),
										gain(Gain),
										actualSpeed(actual_speed),
										supplyVoltage(supply_voltage)
{};

void	DCMotorSpeedController::setCommandedSpeed	(	double	commandedSpeed)
{
				MotorSpeedController::setCommandedSpeed(	commandedSpeed);
				motorVoltage	=	supplyVoltage	*	gain	*	(commandedSpeed	-	actualSpeed);
				motorVoltage	=	std	::	min	(supplyVoltage,	motorVoltage);
				motorVoltage	=	std	::	max	(-supplyVoltage,	motorVoltage);
				motor.update(	motorVoltage);
}

double	DCMotorSpeedController::getMotorVoltage()
{
				return	motorVoltage;
}
