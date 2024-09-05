#include	"trick/ABM_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::ABM_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;

				/**	Init	parent	class	*/
				dt	=	Dt;
				num_state	=	State_size;

				state_origin	=		INTEG_ALLOC(	double*,	num_state	);
				for(i=0;	i<num_state	;	i++)	{
								state_origin[i]	=	(double*)NULL;
				}

				/**	Allocate	the	state	vector.*/
				state	=		INTEG_ALLOC(	double,	num_state	);

				/**	Allocate	the	state	deriv	vector.*/
				deriv	=	INTEG_ALLOC(	double*,	4);
				for(i=0;	i<4	;	i++)	{
								deriv[i]	=	INTEG_ALLOC(	double,	num_state);
				}

				/**	Allocate	the	workspace.*/
				state_ws	=	INTEG_ALLOC(	double*,	4);
				for(i=0;	i<4	;	i++)	{
								state_ws[i]	=	INTEG_ALLOC(	double,	num_state);
				}

				/**	Allocate	the	stored	data.*/
				stored_data	=	INTEG_ALLOC(	double*,	8);
				for(i=0;	i<8	;	i++)	{
								stored_data[i]	=	INTEG_ALLOC(	double,	num_state);
				}
}

/**
	*/
Trick::ABM_Integrator::ABM_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::ABM_Integrator().\n");
				}
				initialize(State_size,	Dt);
}

Trick::ABM_Integrator::~ABM_Integrator()	{

				int	i;

				if	(state_origin)	INTEG_FREE(state_origin);

				if	(state)	INTEG_FREE(state);

				for(i=0;	i<4	;	i++)	{
								if	(deriv[i])	INTEG_FREE(deriv[i]);
				}
				if	(deriv)	INTEG_FREE(deriv);

				for(i=0;	i<4	;	i++)	{
								if	(state_ws[i])	INTEG_FREE(state_ws[i]);
				}
				if	(state_ws)	INTEG_FREE(state_ws);

				for(i=0;	i<8	;	i++)	{
								if	(stored_data[i])	INTEG_FREE(stored_data[i]);
				}
				if	(stored_data)	INTEG_FREE(stored_data);
}

/**
	*/
int	Trick::ABM_Integrator::integrate()	{

				int	i;
				int	j;
				double	dto2;
				double	dto3,	dto6;

				if	(!primed)	{
								/*	1)	prime	using	standard	rk_4	2)	set	primed	to	1	*/
								switch	(intermediate_step)	{
												case	0:
																/*	Do	a	sanity	check	on	stored_data	to	see	if	it	is	allocated	*/
																if	(stored_data	==	NULL)	{
																				message_publish(MSG_ERROR,	"ABM_Integrator:	stored_data	is	NULL\n");
																				return(0);
																}

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
																for	(i	=	0;	i	<	num_state;	i++)	{
																				state_ws[3][i]	=	state_ws[0][i]	+	deriv[2][i]	*	dt;
																}

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
																				stored_data[4	+	counter][i]	=	deriv[3][i];
																}

																/*	Update	time	*/
																time	=	time_0	+	dt;

																intermediate_step	=	0;
																counter	=	counter	+	1;
																if	((counter	+	4)	==	8)	{
																				first_step_deriv	=	0;
																				primed	=	1;
																}
																break;
								}
				}	else	{
								switch	(intermediate_step)	{
												case	0:
																/*	Use	the	predictor	method	to	calc	deriv	*/
																for	(i	=	0;	i	<	num_state;	i++)	{
																				state_ws[0][i]	=	state[i];
																				state_ws[1][i]	=	state[i]	+	dt	/	24.0	*	(55.0	*	stored_data[7][i]
																																																																						-	59.0	*	stored_data[6][i]
																																																																						+	37.0	*	stored_data[5][i]
																																																																						-	9.0	*	stored_data[4][i]);
																}
																intermediate_step	=	1;
																break;
												case	1:
																/*	Use	the	corrector	method	to	calc	new	deriv	*/
																for	(i	=	0;	i	<	num_state;	i++)	{
																				state_ws[2][i]	=	state_ws[0][i]	+	dt	/	24.0	*	(9.0	*	deriv[1][i]
																																																																												+	19.0	*	stored_data[7][i]
																																																																												-	5.0	*	stored_data[6][i]
																																																																												+	stored_data[5][i]);
																}
																intermediate_step	=	2;
																break;
												case	2:
																/*	Reset	the	values	in	the	array	of	derivatives	*/
																for	(i	=	0;	i	<	num_state;	i++)	{
																				state_ws[0][i]	=	state_ws[2][i];
																}
																/*	Shift	values	in	data	array	and	add	new	data	*/
																for	(i	=	0;	i	<	num_state;	i++)	{
																				for	(j	=	0;	j	<	7;	j++)	{
																								stored_data[j][i]	=	stored_data[j	+	1][i];
																				}
																				stored_data[7][i]	=	deriv[2][i];
																}

																/*	Update	time	*/
																time	=	time_0	+	dt;

																intermediate_step	=	0;
																break;
								}
				}

				return(	intermediate_step);
}

