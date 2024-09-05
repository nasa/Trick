#include	"trick/Euler_Integrator.hh"
#include	"trick/message_proto.h"
#include	<iostream>
#include	<stdlib.h>

/**
	*/
void	Trick::Euler_Integrator::initialize(int	State_size,	double	Dt)	{

				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::Euler_Integrator::initialize().\n")	;
				}

				/**	Init	parent	class.	*/
				dt	=	Dt;
				num_state	=	State_size;

				state_origin	=		INTEG_ALLOC(	double*,	num_state	);
				for(int	i=0;	i<num_state	;	i++)	{
								state_origin[i]	=	(double*)NULL;
				}

				/**	Allocate	the	state	vector.*/
				state	=	INTEG_ALLOC(	double,	num_state	);

				/**	Allocate	the	state	deriv	vector.*/
				deriv	=	INTEG_ALLOC(	double*,	1);
				deriv[0]	=	INTEG_ALLOC(	double,	num_state);

				/**	Allocate	the	workspace.*/
				state_ws	=	INTEG_ALLOC(	double*,	1);
				state_ws[0]	=	INTEG_ALLOC(	double,	num_state);
}

/**
	*/
Trick::Euler_Integrator::Euler_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::Euler_Integrator().\n")	;
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::Euler_Integrator::~Euler_Integrator()	{

				if	(state_origin)	INTEG_FREE(state_origin);
				if	(state)	INTEG_FREE(state);
				if	(deriv[0])	INTEG_FREE(deriv[0]);
				if	(deriv)	INTEG_FREE(deriv);
				if	(state_ws[0])	INTEG_FREE(state_ws[0]);
				if	(state_ws)	INTEG_FREE(state_ws);

}

/**
	*/
int	Trick::Euler_Integrator::integrate()	{

				int	i;

				switch	(intermediate_step)	{
								case	0:
												time_0	=	time;
												/*	Compute	1st	state	estimate	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[0][i]	=	state[i]	+	deriv[0][i]	*	dt;

												/*	Update	time	*/
												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}

				return	(	intermediate_step);

}

void	Trick::Euler_Integrator::set_first_step_deriv(bool	first_step)	{
				if	(	!first_step		)	{
								message_publish(MSG_WARNING,	"Euler	Integration	scheme	should	always	have	first_step_deriv	=	1\n");
				}
				first_step_deriv	=	first_step;
}

