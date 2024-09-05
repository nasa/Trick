#include	"trick/NL2_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::NL2_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	2;

				/**	Init	parent	class.	*/
				first_step_deriv	=	0;
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
Trick::NL2_Integrator::NL2_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::NL2_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::NL2_Integrator::~NL2_Integrator()	{

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
int	Trick::NL2_Integrator::integrate()	{

				int	i,	no2;
				double	dto2;																/*	dt/2	*/

				dto2	=	dt	/	2.0;
				no2	=	num_state	/	2;

				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	*/
												time_0	=	time;

												/*	Save	first	step	0	state	and	compute	1st	state	estimate	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	no2;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	dto2;
												}
												for	(;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i];
												}
												intermediate_step	=	1;
												break;

								case	1:

												/*	Compute	final	state	estimate	at	t	=	t	+	dt	*/
												for	(i	=	no2;	i	<	num_state;	i++)
																state_ws[0][i]	+=	(deriv[1][i]	*	dt);

												for	(i	=	0;	i	<	no2;	i++)	{
																deriv[1][i]	=	state_ws[0][no2	+	i];
																state_ws[0][i]	=	state_ws[1][i]	+	deriv[1][i]	*	dto2;
												}
												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}

				return	(intermediate_step);
}

void	Trick::NL2_Integrator::set_first_step_deriv(bool	first_step)	{
				if	(	first_step	)	{
								message_publish(MSG_WARNING,	"2nd	order	Nystrom-Lear	should	always	have	first_step_deriv	=	0\n");
				}
				first_step_deriv	=	first_step;
}

