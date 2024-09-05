#include	"trick/RKF45_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::RKF45_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	6;

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
Trick::RKF45_Integrator::RKF45_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::RKF45_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::RKF45_Integrator::~RKF45_Integrator()	{

				const	int	n_steps	=	6;
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
int	Trick::RKF45_Integrator::integrate()	{

				int	i;
				double	dto4;																/*	dt/4	*/
				double	b_1,	b_2,	b_3,	b_4,	b_5;					/*	dt	workspace	*/
				double	c_1,	c_3,	c_4,	c_5,	c_6;					/*	dt	workspace	*/

				static	const	double	ch_45[]	=	{	0.0,	16.0	/	135.0,	0.0,	6656.0	/	12825.0,	28561.0	/	56430.0,	-9.0	/	50.0,	2.0	/	55.0	};
				static	const	double	b3_45[]	=	{	0.0,	3.0	/	32.0,	9.0	/	32.0	};
				static	const	double	b4_45[]	=	{	0.0,	1932.0	/	2197.0,	-7200.0	/	2197.0,	7296.0	/	2197.0	};
				static	const	double	b5_45[]	=	{	0.0,	439.0	/	216.0,	-8.0,	3680.0	/	513.0,	-845.0	/	4104.0	};
				static	const	double	b6_45[]	=	{	0.0,	-8.0	/	27.0,	2.0,	-3544.0	/	2565.0,	1859.0	/	4104.0,	-11.0	/	40.0	};


				switch	(intermediate_step)	{

								case	0:
												/*	Save	initial	time	and	calculate	quarter	time	step	*/
												time_0	=	time;
												dto4	=	dt	/	4.0;

												/*	Save	initial	state	and	compute	state	at	t	=	t	+	dt/4	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	dto4;
												}

												intermediate_step	=	1;
												break;

								case	1:
												/*	Compute	next	state	*/
												b_1	=	b3_45[1]	*	dt;
												b_2	=	b3_45[2]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[2][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[1][i]	*	b_2;

												intermediate_step	=	2;
												break;

								case	2:
												/*	Compute	next	state	*/
												b_1	=	b4_45[1]	*	dt;
												b_2	=	b4_45[2]	*	dt;
												b_3	=	b4_45[3]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[3][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[1][i]	*	b_2	+	deriv[2][i]	*	b_3;

												intermediate_step	=	3;
												break;

								case	3:
												/*	Compute	next	state	*/
												b_1	=	b5_45[1]	*	dt;
												b_2	=	b5_45[2]	*	dt;
												b_3	=	b5_45[3]	*	dt;
												b_4	=	b5_45[4]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[4][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[1][i]	*	b_2	+	deriv[2][i]	*	b_3	+	deriv[3][i]	*	b_4;

												intermediate_step	=	4;
												break;

								case	4:
												/*	Compute	next	state	*/
												b_1	=	b6_45[1]	*	dt;
												b_2	=	b6_45[2]	*	dt;
												b_3	=	b6_45[3]	*	dt;
												b_4	=	b6_45[4]	*	dt;
												b_5	=	b6_45[5]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[5][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1
																				+	deriv[1][i]	*	b_2	+	deriv[2][i]	*	b_3	+	deriv[3][i]	*	b_4	+	deriv[4][i]	*	b_5;

												intermediate_step	=	5;
												break;

								case	5:
												/*	Set	up	coefficients	for	final	state	comuptation	*/
												c_1	=	ch_45[1]	*	dt;
												c_3	=	ch_45[3]	*	dt;
												c_4	=	ch_45[4]	*	dt;
												c_5	=	ch_45[5]	*	dt;
												c_6	=	ch_45[6]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[0][i]	+=	(deriv[0][i]	*	c_1
																																						+	deriv[2][i]	*	c_3
																																						+	deriv[3][i]	*	c_4	+	deriv[4][i]	*	c_5	+	deriv[5][i]	*	c_6);
												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}

				return	(intermediate_step);

}

void	Trick::RKF45_Integrator::set_first_step_deriv(bool	first_step)	{
				if	(	!first_step	)	{
								message_publish(MSG_WARNING,	"5th	Order	Runge	Kutta	Fehlberg	should	always	have	first_step_deriv	=	1\n");
				}
				first_step_deriv	=	first_step;
}

