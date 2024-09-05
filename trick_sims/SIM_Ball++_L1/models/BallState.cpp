
#include	<stdio.h>

#include	"BallState.hh"
#include	"trick/constant.h"
#include	"trick/memorymanager_c_intf.h"

BallStateInput::BallStateInput()	{

				mass	=	10.0	;
				position[0]	=	5.0	;
				position[1]	=	5.0	;
				speed	=	3.5	;
				elevation	=	45.0	*	DTR	;

}

BallStateOutput::BallStateOutput()	{

				position[0]	=	0.0	;
				position[1]	=	0.0	;
				velocity[0]	=	0.0	;
				velocity[1]	=	0.0	;
				acceleration[0]	=	0.0	;
				acceleration[1]	=	0.0	;
				external_force[0]	=	0.0	;
				external_force[1]	=	0.0	;

}

BallStateWork::BallStateWork()	{
				num_external_force	=	0	;
				external_force	=	NULL	;
}

int	BallState::add_external_force(	double	*	in_ptr	)	{
				if	(	work.num_external_force	==	0	)	{
								//	Declares	a	new	array	to	hold	a	single	double	*
								work.external_force	=	(double	**)TMM_declare_var_1d("double	*",	++work.num_external_force)	;
				}	else	{
								//	Increases	the	array	size	by	one	to	accommodate	a	new	force.
								work.external_force	=	(double	**)TMM_resize_array_1d_a(	work.external_force,	++work.num_external_force	)	;
				}
				//	Add	the	force	to	the	list
				work.external_force[work.num_external_force	-	1]	=	in_ptr	;
				return	0	;
}
