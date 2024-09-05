#include	"trick/RKF78_Integrator.hh"
#include	"trick/message_proto.h"

/**
	*/
void	Trick::RKF78_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	12;

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
Trick::RKF78_Integrator::RKF78_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::RKF78_Integrator()\n");
				}
				initialize(State_size,	Dt);
}

/**
	*/
Trick::RKF78_Integrator::~RKF78_Integrator()	{

				const	int	n_steps	=	12;
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
int	Trick::RKF78_Integrator::integrate()	{

				int	i;
				double	b_1,	b_2,	b_3,	b_4,	b_5,	b_6,	b_7;			/*	temporary	dt	values	*/
				double	c_1,	c_2,	c_3,	c_4;		/*	temporary	dt	values	*/

				static	double	ch_78[]	=	{	0.0,	0.0,	0.0,	0.0,	0.0,	0.0,	34.0	/	105.0,	9.0	/	35.0,	9.0	/	35.0,	9.0	/	280.0,
																														9.0	/	280.0,	0.0,	41.0	/	840.0,	41.0	/	840.0	};
				static	double	b2_78[]	=	{	0.0,	2.0	/	27.0	};
				static	double	b3_78[]	=	{	0.0,	1.0	/	36.0,	1.0	/	12.0	};
				static	double	b4_78[]	=	{	0.0,	1.0	/	24.0,	0.0,	1.0	/	8.0	};
				static	double	b5_78[]	=	{	0.0,	5.0	/	12.0,	0.0,	-25.0	/	16.0,	25.0	/	16.0	};
				static	double	b6_78[]	=	{	0.0,	1.0	/	20.0,	0.0,	0.0,	1.0	/	4.0,	1.0	/	5.0	};
				static	double	b7_78[]	=	{	0.0,	-25.0	/	108.0,	0.0,	0.0,	125.0	/	108.0,	-65.0	/	27.0,	125.0	/	54.0	};
				static	double	b8_78[]	=	{	0.0,	31.0	/	300.0,	0.0,	0.0,	0.0,	61.0	/	225.0,	-2.0	/	9.0,	13.0	/	900.0	};
				static	double	b9_78[]	=	{	0.0,	2.0,	0.0,	0.0,	-53.0	/	6.0,	704.0	/	45.0,	-107.0	/	9.0,	67.0	/	90.0,	3.0	};
				static	double	b10_78[]	=	{	0.0,	-91.0	/	108.0,	0.0,	0.0,	23.0	/	108.0,	-976.0	/	135.0,	311.0	/	54.0,
																															-19.0	/	60.0,	17.0	/	6.0,	-1.0	/	12.0	};
				static	double	b12_78[]	=	{	0.0,	3.0	/	205.0,	0.0,	0.0,	0.0,	0.0,	-6.0	/	41.0,	-3.0	/	205.0,	-3.0	/	41.0,
																															3.0	/	41.0,	6.0	/	41.0	};
				static	double	b13_78[]	=	{	0.0,	-1777.0	/	4100.0,	0.0,	0.0,	-341.0	/	164.0,	4496.0	/	1025.0,	-289.0	/	82.0,
																															2193.0	/	4100.0,	51.0	/	82.0,	33.0	/	164.0,	12.0	/	41.0,	0.0,	1.0	};

				switch	(intermediate_step)	{
								case	0:
												/*	Save	initial	time	*/
												time_0	=	time;

												/*	Save	initial	state	and	compute	the	next	step	state	*/
												b_1	=	b2_78[1]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)	{
																state_ws[0][i]	=	state[i];
																state_ws[1][i]	=	state[i]	+	deriv[0][i]	*	b_1;
												}
												intermediate_step	=	1;
												break;

								case	1:
												/*	Compute	the	next	state	*/
												b_1	=	b3_78[1]	*	dt;
												b_2	=	b3_78[2]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[2][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[1][i]	*	b_2;
												intermediate_step	=	2;
												break;

								case	2:
												/*	Compute	the	next	state	*/
												b_1	=	b4_78[1]	*	dt;
												b_3	=	b4_78[3]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[3][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[2][i]	*	b_3;
												intermediate_step	=	3;
												break;

								case	3:
												/*	Compute	the	next	state	*/
												b_1	=	b5_78[1]	*	dt;
												b_3	=	b5_78[3]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[4][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	(deriv[2][i]	-	deriv[3][i])	*	b_3;
												intermediate_step	=	4;
												break;

								case	4:
												/*	Compute	the	next	state	*/
												b_1	=	b6_78[1]	*	dt;
												b_4	=	b6_78[4]	*	dt;
												b_5	=	b6_78[5]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[5][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[3][i]	*	b_4	+	deriv[4][i]	*	b_5;
												intermediate_step	=	5;
												break;

								case	5:
												/*	Compute	the	next	state	*/
												b_1	=	b7_78[1]	*	dt;
												b_4	=	b7_78[4]	*	dt;
												b_5	=	b7_78[5]	*	dt;
												b_6	=	b7_78[6]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[6][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[3][i]	*	b_4	+	deriv[4][i]	*	b_5	+	deriv[5][i]	*	b_6;
												intermediate_step	=	6;
												break;

								case	6:
												/*	Compute	the	next	state	*/
												b_1	=	b8_78[1]	*	dt;
												b_5	=	b8_78[5]	*	dt;
												b_6	=	b8_78[6]	*	dt;
												b_7	=	b8_78[7]	*	dt;
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[7][i]	=	state_ws[0][i]
																				+	deriv[0][i]	*	b_1	+	deriv[4][i]	*	b_5	+	deriv[5][i]	*	b_6	+	deriv[6][i]	*	b_7;
												intermediate_step	=	7;
												break;

								case	7:
												/*	Compute	the	next	state	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[8][i]	=	state_ws[0][i]	+	(deriv[0][i]	*	b9_78[1]
																																																									+	deriv[3][i]	*	b9_78[4]
																																																									+	deriv[4][i]	*	b9_78[5]
																																																									+	deriv[5][i]	*	b9_78[6]
																																																									+	deriv[6][i]	*	b9_78[7]
																																																									+	deriv[7][i]	*	b9_78[8])	*	dt;
												intermediate_step	=	8;
												break;

								case	8:
												/*	Compute	the	next	state	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[9][i]	=	state_ws[0][i]	+	(deriv[0][i]	*	b10_78[1]
																																																									+	deriv[3][i]	*	b10_78[4]
																																																									+	deriv[4][i]	*	b10_78[5]
																																																									+	deriv[5][i]	*	b10_78[6]
																																																									+	deriv[6][i]	*	b10_78[7]
																																																									+	deriv[7][i]	*	b10_78[8]
																																																									+	deriv[8][i]	*	b10_78[9])	*	dt;
												intermediate_step	=	9;
												break;

								case	9:
												/*	Compute	the	next	state	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[10][i]	=	state_ws[0][i]	+	(deriv[0][i]	*	b12_78[1]
																																																										+	deriv[5][i]	*	b12_78[6]
																																																										+	deriv[6][i]	*	b12_78[7]
																																																										+	deriv[7][i]	*	b12_78[8]
																																																										+	deriv[8][i]	*	b12_78[9]
																																																										+	deriv[9][i]	*	b12_78[10])	*	dt;
												intermediate_step	=	10;
												break;

								case	10:
												/*	Compute	the	next	state	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[11][i]	=	state_ws[0][i]	+	(deriv[0][i]	*	b13_78[1]
																																																										+	deriv[3][i]	*	b13_78[4]
																																																										+	deriv[4][i]	*	b13_78[5]
																																																										+	deriv[5][i]	*	b13_78[6]
																																																										+	deriv[6][i]	*	b13_78[7]
																																																										+	deriv[7][i]	*	b13_78[8]
																																																										+	deriv[8][i]	*	b13_78[9]
																																																										+	deriv[9][i]	*	b13_78[10]
																																																										+	deriv[10][i])	*	dt;
												intermediate_step	=	11;
												break;

								case	11:
												/*	Set	coefficients	for	final	state	computation	*/
												c_1	=	ch_78[6]	*	dt;
												c_2	=	ch_78[7]	*	dt;
												c_3	=	ch_78[9]	*	dt;
												c_4	=	ch_78[12]	*	dt;

												/*	Compute	state	at	t	=	t	+	dt	*/
												for	(i	=	0;	i	<	num_state;	i++)
																state_ws[0][i]	+=	(deriv[5][i]	*	c_1
																																						+	(deriv[6][i]	+
																																									deriv[7][i])	*	c_2	+
																																						(deriv[8][i]	+
																																							deriv[9][i])	*	c_3	+	(deriv[10][i]	+	deriv[11][i])	*	c_4);

												/*	Update	time	*/
												time	=	time_0	+	dt;
												intermediate_step	=	0;
												break;
				}
				return	(	intermediate_step);
}
