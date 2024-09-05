#include	"../include/DCMotor.hh"
#include	<iostream>
#include	<cmath>


DCMotor::DCMotor	(const	double	initialInternalResistance,
																		const	double	initialMotorTorqueConstant)
																:	motorTorque(0.0),
																		motorCurrent(0.0),
																		currentLoad(0.0),
																		internalResistance(initialInternalResistance),
																		motorTorqueConstant(initialMotorTorqueConstant)
{	}

void	DCMotor	::	update	(const	double	motorVoltage)
{
				motorCurrent	=	motorVoltage	/	internalResistance	;
				motorTorque	=	motorCurrent	*	motorTorqueConstant;
				currentLoad	=	std	::	abs	(motorCurrent);
}

void	DCMotor::update	(const	PWM&	PulseWidth)
{
				update(PulseWidth.getAverageVoltage());
}

double	DCMotor	::	getTorque()
{
				return	motorTorque;
}

double	DCMotor	::	getCurrentLoad()
{
				return	currentLoad;
}
