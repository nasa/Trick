#include	"trick/RKG4_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::RKG4_Integrator::initialize(int	State_size,	double	Dt)	{

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
				state_ws	=	INTEG_ALLOC(	double*,	5);
				for(i=0;	i<5	;	i++)	{
								state_ws[i]	=	INTEG_ALLOC(	double,	num_state);
				}
}


/**
	*/
Trick::RKG4_Integrator::RKG4_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::RKG4_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::RKG4_Integrator::~RKG4_Integrator()	{

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
				for(i=0;	i<5	;	i++)	{
								if	(state_ws[i])	INTEG_FREE(state_ws[i]);
				}
				if	(state_ws)	INTEG_FREE(state_ws);
}

/**
	*/
int	Trick::RKG4_Integrator::integrate()	{

				int	i;
				double	tm;
				double	cx;

				static	const	double	b1	=		0.5857864376269049;		//	2-sqrt(2)
				static	const	double	b2	=		3.414213562373095;			//	2+sqrt(2)
				static	const	double	c1	=		0.1213203435596426;		//	(sqrt(2)-1)/(2+sqrt(2))
				static	const	double	c2	=	-4.121320343559642;			//
				static	const	double	d1	=		0.1666666666666667;		//	1/6
				static	const	double	d2	=	-0.3333333333333333;		//	-1/3

				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	and	compute	time	increments	*/
												time_0	=	time;
												dt	=	dt;

												/*	Save	initial	state	and	compute	state	at	t	=	t	+	dt/2	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[4][i]	=	dt	*	deriv[0][i];
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	state_ws[4][i]	*	0.5;
												}
												intermediate_step	=	1;
												break;

								case	1:
												/*	Compute	time	increments	*/
												dt	=	dt;
												cx	=	0.5	*	b1;

												/*	Compute	adjusted	state	at	t	=	t	+	dt/2	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																tm	=	dt	*	deriv[1][i];
																state_ws[2][i]	=	state_ws[1][i]	+	cx	*	(tm	-	state_ws[4][i]);
																state_ws[4][i]	=	b1	*	tm	+	c1	*	state_ws[4][i];
												}
												intermediate_step	=	2;
												break;

								case	2:

												/*	Compute	time	increments	*/
												dt	=	dt;
												cx	=	0.5	*	b2;

												/*	Compute	state	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																tm	=	dt	*	deriv[2][i];
																state_ws[3][i]	=	state_ws[2][i]	+	cx	*	(tm	-	state_ws[4][i]);
																state_ws[4][i]	=	b2	*	tm	+	c2	*	state_ws[4][i];
												}
												intermediate_step	=	3;
												break;

								case	3:
												/*	Compute	time	increments	*/
												dt	=	dt;

												/*	Compute	adjusted	state	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)	{
																tm	=	dt	*	deriv[3][i];
																state_ws[0][i]	=	state_ws[3][i]	+	d1	*	tm	+	d2	*	state_ws[4][i];
												}

												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}

				return(intermediate_step);
}
