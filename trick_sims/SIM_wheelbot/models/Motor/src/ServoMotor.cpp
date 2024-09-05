#include	"Motor/include/ServoMotor.hh"

ServoMotor::ServoMotor	(char	side)
{
		_side	=	side;
}

double	ServoMotor::getActualSpeed(int	PulseWidth)
{
		_PulseWidth	=	PulseWidth;

		if	(_side	==	'L')
		{
					actualspeed	=	-1.8147280722744906e+001	*	pow(_PulseWidth,0)
						+	-3.4553463215611258e-001	*	pow(_PulseWidth,1)
						+		4.5593326051360884e-002	*	pow(_PulseWidth,2)
						+	-1.8392645176315394e-003	*	pow(_PulseWidth,3)
						+		3.3261726281542813e-005	*	pow(_PulseWidth,4)
						+	-2.8937430901462806e-007	*	pow(_PulseWidth,5)
						+		1.2003663411874751e-009	*	pow(_PulseWidth,6)
						+	-1.9140644089539568e-012	*	pow(_PulseWidth,7);
		}
		else	if	(_side	==	'R')
		{
						actualspeed	=	1.8147280722744906e+001	*	pow(_PulseWidth,0)
						+		3.4553463215611258e-001	*	pow(_PulseWidth,1)
						+	-4.5593326051360884e-002	*	pow(_PulseWidth,2)
						+		1.8392645176315394e-003	*	pow(_PulseWidth,3)
						+	-3.3261726281542813e-005	*	pow(_PulseWidth,4)
						+		2.8937430901462806e-007	*	pow(_PulseWidth,5)
						+	-1.2003663411874751e-009	*	pow(_PulseWidth,6)
						+		1.9140644089539568e-012	*	pow(_PulseWidth,7);
		}
		return	actualspeed;
}
