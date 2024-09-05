#include	"../include/PWM.hh"
#include	<iostream>
#include	<cmath>

PWM::PWM	(	double	HighVoltage,
											double	LowVoltage,
											double	DutyCycle)	{

			highVoltage	=	HighVoltage;
			lowVoltage		=	LowVoltage;
			setDutyCycle(	DutyCycle);
}

void	PWM::setDutyCycle(	double	DutyCycle)	{
			if	((	DutyCycle	>=	0.0	)	&&	(	DutyCycle	<=	1.0	))	{
							dutyCycle	=	DutyCycle;
			}	else	{
							throw	std::logic_error("PWM::PWM():	DutyCycle	must	be	between	0.0	..	1.0.");
			}
}

double	PWM	::	getDutyCycle()	const	{
				return	dutyCycle;
}

double	PWM::getAverageVoltage()	const	{

				return	(	(highVoltage	*	dutyCycle	+	lowVoltage	*	(1	-	dutyCycle))	/	2);
}
