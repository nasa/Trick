
#include	"trick/RK2_Integrator.hh"
#include	<iostream>
#include	"trick/message_proto.h"

/**
	*/
void	Trick::RK2_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	2;

				/**	Init	parent	class	*/
				num_state	=	State_size;
				dt	=	Dt;

				state_origin	=		INTEG_ALLOC(	double*,	num_state	);
				for(i=0;	i<num_state	;	i++)	{
								state_origin[i]	=	(double*)NULL;
				}

				/**	Allocate	the	state	vector.*/
				state	=		INTEG_ALLOC(	double,	num_state	);

				/**	Allocate	the	state	deriv	vector.*/
				deriv	=	INTEG_ALLOC(	double*,	n_steps);
				for(i=0;	i<n_steps	;	i++)	{
								deriv[i]	=	INTEG_ALLOC(	double,	num_state);
				}

				/**	Allocate	the	workspace.*/
				state_ws	=	INTEG_ALLOC(	double*,	n_steps);
				for(i=0;	i<n_steps	;	i++)	{
								state_ws[i]	=	INTEG_ALLOC(	double,	num_state);
				}
}

/**
	*/
Trick::RK2_Integrator::RK2_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::RK2_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::RK2_Integrator::~RK2_Integrator()	{

				const	int	n_steps	=	2;
				int	i;

				if	(state_origin)	INTEG_FREE(state_origin);

				/**	Free	the	state	vector.*/
				if	(state)	INTEG_FREE(state);

				/**	Free	the	state	derivative	vector.*/
				for(i=0;	i<n_steps	;	i++)	{
								if	(deriv[i])	INTEG_FREE(deriv[i]);
				}
				if	(deriv)	INTEG_FREE(deriv);

				/**	Free	the	workspace.*/
				for(i=0;	i<n_steps	;	i++)	{
								if	(state_ws[i])	INTEG_FREE(state_ws[i]);
				}
				if	(state_ws)	INTEG_FREE(state_ws);
}

/**
	*/
int	Trick::RK2_Integrator::integrate()	{

				int	i;
				double	dto2;																/*	dt/2	*/


				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	*/
												time_0	=	time;

												/*	Save	first	step	0	state	and	compute	1st	state	estimate	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	dt;
												}
												intermediate_step	=	1;
												break;

								case	1:
												/*	Calculate	half	time	step	*/
												dto2	=	dt	/	2.0;

												/*	Compute	final	state	estimate	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																			state_ws[0][i]	+=	((deriv[0][i]	+	deriv[1][i])	*	dto2);
												}
												intermediate_step	=	0;

												/*	Update	time	*/
												time	=	time_0	+	dt;
												break;
				}

				return(	intermediate_step);
}

