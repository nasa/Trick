#include	"../include/CrewModuleShape.hh"
#include	"trick/vector_macros.h"
#include	"trick/matrix_macros.h"

CrewModuleShape::CrewModuleShape	()	{
				sphere_radius	=	6.04;
				cone_angle	=	(M_PI/180.0)	*	32.5;

				sphere_center_base[0]	=	0.00;
				sphere_center_base[1]	=	0.00;
				sphere_center_base[2]	=	5.14;

				cone_point_base[0]	=	0.00;
				cone_point_base[1]	=	0.00;
				cone_point_base[2]	=	3.82;

				plane_point_base[0]	=	0.00;
				plane_point_base[1]	=	0.00;
				plane_point_base[2]	=	2.40;

				double	M[3][3]	=	{{1.0,	0.0,	0.0},
																						{0.0,	1.0,	0.0},
																						{0.0,	0.0,	1.0}};

				double	P[3]	=	{0.0,	0.0,	0.0};

				transformCoordinates(M,	P);
}

bool	CrewModuleShape::containsPoint(double	(&test_point)[3])	{
				double	sdiff[3];
				V_SUB(sdiff,	test_point,	sphere_center_trans);
				if	(	V_MAG(sdiff)	<=	sphere_radius)	{	//	The	point	is	in	the	sphere.
								double	cdiff[3];
								V_SUB(cdiff,	test_point,	cone_point_trans);
								double	test_angle	=	acos	(	V_DOT(cdiff,	cone_vector_trans)	/	V_MAG(cdiff)	);
								if	(	test_angle	<	cone_angle	)	{	//	The	point	is	in	the	cone.
												double	pdiff[3];
												V_SUB(pdiff,	test_point,	plane_point_trans);
												return	(	V_DOT(pdiff,	plane_vector_trans)	>	0.0);	//	The	point	is	on	the	correct	side	of	the	plane.
								}
				}
				return	false;
}

void	CrewModuleShape::transformCoordinates(	double	(&rotation)[3][3],	double	(&position)[3])	{
				transformPoint(rotation,	position,	sphere_center_base,	sphere_center_trans	);
				transformPoint(rotation,	position,	cone_point_base,	cone_point_trans	);
				transformPoint(rotation,	position,	plane_point_base,	plane_point_trans	);

				/*
							Calculate	these	unit	vectors	so	they	dont	have	to	be	calculated	everytime	we	test	a	point.
				*/
				
				double	scale;
				V_SUB(cone_vector_trans,	position,	cone_point_trans);
				scale	=	1.0/V_MAG(cone_vector_trans);
				V_SCALE(cone_vector_trans,	cone_vector_trans,	scale);

				V_SUB(plane_vector_trans,	position,	plane_point_trans);
				scale	=	1.0/V_MAG(plane_vector_trans);
				V_SCALE(plane_vector_trans,	plane_vector_trans,	scale);
}

void	CrewModuleShape::transformPoint(double	(&rotation)[3][3],	double	(&translation)[3],	double	(&in_point)[3],	double	(&out_point)[3]	)	{
				double	work[3];
				MxV(work,	rotation,	in_point);
				V_ADD(out_point,	work,	translation);
}
