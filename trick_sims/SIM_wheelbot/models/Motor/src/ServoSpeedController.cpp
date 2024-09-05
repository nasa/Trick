#include	"Motor/include/ServoSpeedController.hh"


ServoSpeedController::ServoSpeedController(ServoMotor&	Servo_Motor,	char	side):
							servo(Servo_Motor),	_side(side)
{};

void	ServoSpeedController::setCommandedSpeed	(double	commandedSpeed)
{
		double	PulseWidth;

		if	(_side	==	'L')
		{
				std::cout	<<"The	left	wheel	desired	speed	is:	"	<<	desiredspeed	<<	std::endl;

				PulseWidth	=	9.1296697267545980e+001	*	pow(desiredspeed,0)
		+		1.3551549019843796e+000	*	pow(desiredspeed,1)
		+	-2.5748263162935388e-002	*	pow(desiredspeed,2)
		+	-3.7691759514032080e-003	*	pow(desiredspeed,3)
		+		3.8490572015823302e-004	*	pow(desiredspeed,4)
		+		4.5526955758039407e-005	*	pow(desiredspeed,5)
		+	-6.7622608926425730e-007	*	pow(desiredspeed,6);
				std::cout	<<	"The	pulse	width	for	the	side	above	is	"	<<	PulseWidth	<<	std::endl;

				_PulseWidth	=	(int)PulseWidth;
		}
		else	if(_side	==	'R')
		{

				std::cout	<<"The	right	wheel	desired	speed	is:	"	<<	desiredspeed	<<	std::endl;

				PulseWidth	=	9.1296697267545980e+001	*	pow(desiredspeed,0)
				+	-1.3551549019843796e+000	*	pow(desiredspeed,1)
				+	-2.5748263162935388e-002	*	pow(desiredspeed,2)
				+		3.7691759514032080e-003	*	pow(desiredspeed,3)
				+		3.8490572015823302e-004	*	pow(desiredspeed,4)
				+	-4.5526955758039407e-005	*	pow(desiredspeed,5)
				+	-6.7622608926425730e-007	*	pow(desiredspeed,6);
				std::cout	<<	"The	pulse	width	for	the	side	above	is	"	<<	PulseWidth	<<	std::endl;

				_PulseWidth	=	(int)PulseWidth;
		}

		if	(_PulseWidth	>	180)
		{
				_PulseWidth	=	180;
		}
		else	if	(_PulseWidth	<	0)
		{
				_PulseWidth	=	0;
		}

		if	(_side	==	'L')
				std::cout<<"Left	Servo	angle	is:	"	<<	_PulseWidth	<<std::endl;
		else	if	(_side	==	'R')
				std::cout<<"Right	Servo	angle	is:	"<<	_PulseWidth	<<std::endl;

		//return	_PulseWidth;
}
