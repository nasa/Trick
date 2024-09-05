#include	"trick/RK4_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::RK4_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	4;

				/**	Init	parent	class.	*/
				dt	=	Dt;
				num_state	=	State_size;

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
Trick::RK4_Integrator::RK4_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::RK4_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::RK4_Integrator::~RK4_Integrator()	{

				const	int	n_steps	=	4;
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
int	Trick::RK4_Integrator::integrate()	{

				int	i;
				double	dto2;																/*	dt/2	*/
				double	dto3;																/*	dt/3	*/
				double	dto6;																/*	dt/6	*/

				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	and	compute	time	increments	*/
												time_0	=	time;
												dto2	=	dt	/	2.0;

												/*	Save	initial	state	and	compute	state	at	t	=	t	+	dt/2	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	dto2;
												}
												intermediate_step	=	1;
												break;

								case	1:
												/*	Compute	time	increments	*/
												dto2	=	dt	/	2.0;

												/*	Compute	adjusted	state	at	t	=	t	+	dt/2	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[2][i]	=	state_ws[0][i]	+	deriv[1][i]	*	dto2;
												intermediate_step	=	2;
												break;

								case	2:
												/*	Compute	state	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[3][i]	=	state_ws[0][i]	+	deriv[2][i]	*	dt;

												intermediate_step	=	3;
												break;

								case	3:
												/*	Calculate	time	step	increments	*/
												dto3	=	dt	/	3.0;
												dto6	=	dt	/	6.0;

												/*	Compute	adjusted	state	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	+=
																				((deriv[0][i]	+	deriv[3][i])	*	dto6	+	(deriv[1][i]	+	deriv[2][i])	*	dto3);
												}

												/*	Update	time	*/
												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}

				return(	intermediate_step);
}

