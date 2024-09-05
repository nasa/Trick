#include	"DCBattery.hh"
#include	<iostream>


DCBattery::DCBattery(double	initialIdealVoltage,	double	initialInternalResistance)
:	actualVoltage(0),	current(0),	idealVoltage(initialIdealVoltage),
		internalResistance(initialInternalResistance)		{
}

void	DCBattery	::	update	()
{
				actualVoltage	=	idealVoltage	-	internalResistance	*	current;
				actualVoltage	=	std	::	min	(idealVoltage,	actualVoltage);
				actualVoltage	=	std	::	max	(0.0,	actualVoltage);
}
double	DCBattery	::	getActualVoltage	()
{
				return	actualVoltage;
}
void	DCBattery	::	setCurrent	(double	value)
{
				current	=	value;
}
