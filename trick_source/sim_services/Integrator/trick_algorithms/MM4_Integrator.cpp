#include	"trick/MM4_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::MM4_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	3;

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
Trick::MM4_Integrator::MM4_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::MM4_Integrator()\n");
				}
				initialize(	State_size,	Dt);
}

/**
	*/
Trick::MM4_Integrator::~MM4_Integrator()	{

				const	int	n_steps	=	3;
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
int	Trick::MM4_Integrator::integrate()	{

				int	i,	ii;
				double	dto2;																/*	dt	/	2	*/
				int	no2;																				/*	num_state	/	2	*/

				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	and	compute	time	increments	*/
												time_0	=	time;
												dto2	=	dt	/	2.00000000;

												/*	Save	initial	state	and	compute	state	at	t	=	t	+	dt/2	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	dto2;
												}
												intermediate_step	=	1;
												break;

								case	1:
												/*	Update	state	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[2][i]	=	state_ws[0][i]	+	deriv[1][i]	*	dt;

												/*	Update	time	*/
												time	=	time_0	+	dt;
												intermediate_step	=	2;
												break;

								case	2:
												dto2	=	dt	/	2.00000000;
												no2	=	num_state	/	2;

												/*	Update	velocities	first,	then	positions	to	the	full	step	*/
												for	(i	=	no2;	i	<	num_state;	i++)	{
																ii	=	i	-	no2;
																state_ws[0][i]	=	(state_ws[1][i]
																																					+	state_ws[2][i]
																																					+	deriv[2][i]	*	dto2)	/	2.0;
																deriv[2][ii]	=	state_ws[0][i];
																state_ws[0][ii]	=	(state_ws[1][ii]
																																						+	state_ws[2][ii]
																																						+	deriv[2][ii]	*	dto2)	/	2.0;
												}
												intermediate_step	=	0;
												break;
				}

				return(intermediate_step);

}

