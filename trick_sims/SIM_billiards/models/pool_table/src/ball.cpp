/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	balls	contacting	boundaries.	)
LIBRARY	DEPENDENCY:
				((ball.o))
*******************************************************************************/
#include	"../include/ball.hh"
//	#include	"trick/memorymanager_c_intf.h"

//	#include	"trick/MemoryManager.hh"
//	extern	Trick::MemoryManager*	trick_MM;

#include	<new>

Ball::Ball(double	x,	double	y,	double	mass,	double	radius,	bool	isFixed,	int	id)	:
				mass(mass),
				radius(radius),
				fixed(isFixed),
				id(id)
{
				pos	=	Vec(x,	y,	0);
}

void	Ball::setPos(double	x,	double	y)	{	setPos(x,	y,	0);	}
void	Ball::setPos(double	x,	double	y,	double	z)	{
				pos	=	Vec(x,	y,	z);
}

void	Ball::setVel(double	x,	double	y)	{	setVel(x,	y,	0);	}
void	Ball::setVel(double	x,	double	y,	double	z)	{
				vel	=	Vec(x,	y,	z);
}

void	Ball::setAccel(double	x,	double	y)	{	setAccel(x,	y,	0);	}
void	Ball::setAccel(double	x,	double	y,	double	z)	{
				accel	=	Vec(x,	y,	z);
}

void	Ball::setRelativeVel(double	x,	double	y,	double	z)	{
				relativeVel	=	Vec(x,	y,	z);
}

void	Ball::setAngularVel(double	x,	double	y,	double	z)	{
				w	=	Vec(x,	y,	z);
}

void	Ball::setAngularAccel(double	x,	double	y,	double	z)	{
				angularAccel	=	Vec(x,	y,	z);
}

void	Ball::clearAllState()	{
				setPos(0,	0,	0);
				setVel(0,	0,	0);
				setAccel(0,	0,	0);
				setRelativeVel(0,	0,	0);
				setAngularVel(0,	0,	0);
				setAngularAccel(0,	0,	0);
				sliding	=	false;
}




