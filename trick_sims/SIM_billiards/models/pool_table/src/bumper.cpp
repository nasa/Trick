#include	"bumper.hh"
#include	"stdlib.h"

#include	"trick/memorymanager_c_intf.h"

Bumper::Bumper()	{}

Bumper::Bumper(int	numPoints,	double	x1,	double	y1,	double	x2,	double	y2)	:
																												border(Line(Vec(x1,	y1),	Vec(x2,	y2))),
																												numPoints(numPoints)	
{
}


void	Bumper::AddPointToRender(double	x,	double	y)	{
				int	id	=	nextPointSlot++;
				if	(id	<	numPoints)	{
								Vec	*	point	=	(Vec*)	TMM_declare_var_s("Vec");
								renderedShape[id]	=	(new	(point)	Vec(x,	y));
				}
}

void	Bumper::AddBorder	(double	x1,	double	y1,	double	x2,	double	y2)	{
				border	=	Line(Vec(x1,	y1),	Vec(x2,	y2));
}