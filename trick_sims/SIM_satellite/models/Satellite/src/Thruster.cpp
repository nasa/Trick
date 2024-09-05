#include	"Thruster.hh"
#include	"trick_utils/math/include/vector_macros.h"

Thruster::Thruster()	{
				for	(int	ii=0;	ii<3;ii++)	{	position[ii]		=	0.0;	}
				for	(int	ii=0;	ii<3;ii++)	{	thrust[ii]		=	0.0;	}
				on	=	false;
}

void	Thruster::initialize(double	p0,	double	p1,	double	p2,
																					double	f0,	double	f1,	double	f2)	{

				position[0]	=	p0;
				position[1]	=	p1;
				position[2]	=	p2;

				thrust[0]	=	f0;
				thrust[1]	=	f1;
				thrust[2]	=	f2;

				on	=	false;
}

void	Thruster::add_force_and_torque	(double*	force,	double*	torque)	{
				if	(on)	{
								double	T[3];

								V_CROSS(T,	position,	thrust);
								for	(int	ii=0;	ii<3;ii++)	{	torque[ii]	+=	T[ii];	}

								for	(int	ii=0;	ii<3;ii++)	{	force[ii]	+=	thrust[ii];	}
				}
}

