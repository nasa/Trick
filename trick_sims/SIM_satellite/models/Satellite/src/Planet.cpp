#include		"Planet.hh"

Planet::Planet()	{
				mass	=	EARTH_MASS;
				radius	=	EARTH_RADIUS;
				for	(int	ii	=	0;	ii<3	;	ii++	)	{
								pos[ii]	=	0.0;
				}
}

