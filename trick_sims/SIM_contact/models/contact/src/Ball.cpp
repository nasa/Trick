/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	balls	contacting	boundaries.	)
LIBRARY	DEPENDENCY:
				((Ball.o))
*******************************************************************************/
#include	"../include/Ball.hh"
#include	"trick/memorymanager_c_intf.h"
#include	<new>

Ball::Ball(double	x,	double	y,	double	vx,	double	vy,	double	r,	double	m)	{
				pos[0]	=	x;
				pos[1]	=	y;
				vel[0]	=	vx;
				vel[1]	=	vy;
				radius	=	r;
				mass	=	m;
}

Ball*	make_Ball(double	x,	double	y,	double	vx,	double	vy,	double	r,	double	m)	{
				Ball*	b	=	(Ball*)TMM_declare_var_s("Ball");
				return	(new	(b)	Ball(x,y,vx,vy,r,m));
}
