#include	<math.h>	//	for:	sqrt(),	atan2(),	cos(),	and	sin()
#include	"navigator.hh"
#include	<iostream>

void	Navigator::setHeading(double	h)	{
				heading	=	h;
}

void	Navigator::setLocation(double	x,	double	y)	{
				location.x	=	x;
				location.y	=	y;
}

double	Navigator::distanceTo(Point&	mapPoint	)	{

				double	deltaX	=	location.x	-	mapPoint.x;
				double	deltaY	=	location.y	-	mapPoint.y;
				double	distance	=	sqrt(	deltaX	*	deltaX	+	deltaY	*	deltaY	);
				return	(distance)	;
}

double	Navigator::bearingTo(Point&	mapPoint	)	{

				Point	platformPoint	=	convertMapToPlatform(mapPoint);
				Point	bodyPoint	=	convertPlatformToBody(platformPoint);
				double	head2	=	asin(bodyPoint.y/distanceTo(mapPoint)	);
				return	head2;
}

Point	Navigator::convertMapToPlatform(Point&	mapPoint	)	{

				Point	platformPoint;
				platformPoint.x	=	mapPoint.x	-	location.x;
				platformPoint.y	=	mapPoint.y	-	location.y;
				return	(platformPoint);
}

Point	Navigator::convertPlatformToMap(	Point&	platformPoint	)	{

				Point	mapPoint;
				mapPoint.x	=	platformPoint.x	+	location.x;
				mapPoint.y	=	platformPoint.y	+	location.y;
				return	(mapPoint);
}

Point	Navigator::convertPlatformToBody(	Point&	platformPoint	)	{

				Point	bodyPoint;
				bodyPoint.x	=		cos(heading)	*	platformPoint.x	+	sin(heading)	*	platformPoint.y;
				bodyPoint.y	=	-sin(heading)	*	platformPoint.x	+	cos(heading)	*	platformPoint.y;
				return	(bodyPoint);
}

Point	Navigator::convertBodyToPlatform(	Point&	bodyPoint	)	{

				Point	platformPoint;
				platformPoint.x	=		cos(heading)	*	bodyPoint.x	-	sin(heading)	*	bodyPoint.y;
				platformPoint.y	=		sin(heading)	*	bodyPoint.x	+	cos(heading)	*	bodyPoint.y;
				return	(platformPoint);
}
