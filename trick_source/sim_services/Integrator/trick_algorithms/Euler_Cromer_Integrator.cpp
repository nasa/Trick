#include	"trick/Euler_Cromer_Integrator.hh"
#include	"trick/message_proto.h"
#include	<cstdarg>

/**
	*/
void	Trick::Euler_Cromer_Integrator::initialize(int	State_size,	double	Dt)	{

				int	i;
				const	int	n_steps	=	1;

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

				/**	Allocate	the	state	second	deriv	vector.*/
				deriv2	=	INTEG_ALLOC(	double*,	n_steps);
				for(i=0;	i<n_steps	;	i++)	{
								deriv2[i]	=	INTEG_ALLOC(	double,	num_state);
				}

				/**	Allocate	the	workspace.*/
				state_ws	=	INTEG_ALLOC(	double*,	n_steps);
				for(i=0;	i<n_steps	;	i++)	{
								state_ws[i]	=	INTEG_ALLOC(	double,	num_state);
				}
}

/**
	*/
Trick::Euler_Cromer_Integrator::Euler_Cromer_Integrator(int	State_size,	double	Dt)	{
				if	(verbosity)	{
								message_publish(MSG_DEBUG,	"Trick::Euler_Cromer_Integrator()\n");
				}
				initialize(	State_size,	Dt);
}

/**
	*/
Trick::Euler_Cromer_Integrator::~Euler_Cromer_Integrator()	{

				const	int	n_steps	=	1;
				int	i;

				if	(state_origin)	INTEG_FREE(state_origin);

				/**	Free	the	state	vector.*/
				if	(state)	INTEG_FREE(state);

				/**	Free	the	state	derivative	vector.*/
				for(i=0;	i<n_steps	;	i++)	{
								if	(deriv[i])	INTEG_FREE(deriv[i]);
				}
				if	(deriv)	INTEG_FREE(deriv);

				/**	Free	the	state	second	deriv	vector.*/
				for(i=0;	i<n_steps	;	i++)	{
								if	(deriv2[i])	INTEG_FREE(deriv2[i]);
				}
				if	(deriv2)	INTEG_FREE(deriv2);

				/**	Free	the	workspace.*/
				for(i=0;	i<n_steps	;	i++)	{
								if	(state_ws[i])	INTEG_FREE(state_ws[i]);
				}
				if	(state_ws)	INTEG_FREE(state_ws);

}

/**
	*/
void	Trick::Euler_Cromer_Integrator::deriv2_in	(	double*	arg1,	...)	{

				va_list	ap;
				int	i;
				double*	narg;

				va_start(ap,	arg1);

				i=0;
				narg	=	arg1;
				if	(verbosity)	message_publish(MSG_DEBUG,"LOAD	DERIV2:	");
				while	(narg	!=	(double*)NULL)	{
								deriv2[intermediate_step][i]	=	*narg;
								if	(verbosity)	message_publish(MSG_DEBUG,"		%g",*narg);
								narg	=	va_arg(ap,	double*);
								i++;
				};
				if	(verbosity)	message_publish(MSG_DEBUG,"\n");
				va_end(ap);
}

/**
	*/
int	Trick::Euler_Cromer_Integrator::integrate()	{

				int	i;
				int	ns;

				ns	=	num_state	/	2;

				switch	(intermediate_step)	{
								time_0	=	time;
								case	0:
												for	(i	=	0;	i	<	ns;	i++)	{
																deriv[0][i]	=	deriv[0][i]	+	deriv2[0][i]	*	dt;
																state_ws[0][i]	=	state[i]	+	deriv[0][i]	*	dt;
																state_ws[0][i	+	ns]	=	deriv[0][i];
												}

												time	=	time_0	+=	dt;
												intermediate_step	=	0;
												break;
				}
				return	(	intermediate_step);
}
