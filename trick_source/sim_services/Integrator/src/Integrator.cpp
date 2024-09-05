#include	"trick/Integrator.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	<cstdarg>
#include	<iostream>

/**
	*/
Trick::Integrator::Integrator()	{

			num_state	=	0;
			first_step_deriv	=	1;
			last_step_deriv		=	0;
			intermediate_step	=	0;
			is_2nd_order_ODE_technique	=	0;
			use_deriv2	=	0;
			dt	=	0.01;
			target_integ_time	=	dt;
			state	=	NULL;
			deriv	=	NULL;
			deriv2	=	NULL;
			state_ws	=	NULL;
#ifndef	USE_ER7_UTILS_INTEGRATORS
			state_origin	=	NULL;
#endif
			time	=	0.0;
			time_0	=	0.0;
			verbosity	=	0	;
}

/**
	*/
int	Trick::Integrator::integrate_1st_order_ode	(
				double	const*	derivs_in,	double*	state_in_out)	{
				int	rc;

				if	(is_2nd_order_ODE_technique)	{
								message_publish(MSG_ERROR,
																								"Integrator	ERROR:	"
																								"Current	integrator	is	a	2nd	order	technique.\n");
								return	0;
				}

				for	(int	ii	=	0;	ii	<	num_state;	++ii)	{
								state[ii]	=	state_in_out[ii];
								deriv[intermediate_step][ii]	=	derivs_in[ii];
				}

				rc	=	integrate	();

				for	(int	ii	=	0;	ii	<	num_state;	++ii)	{
								state_in_out[ii]	=	state_ws[intermediate_step][ii];
				}

				return	rc;
}

/**
	*/
int	Trick::Integrator::integrate_2nd_order_ode	(
				double	const*	accel,	double*	velocity,	double*	position)	{
				int	rc;
				int	half_size	=	num_state	/	2;

				if	(use_deriv2)	{
								for	(int	ii	=	0;	ii	<	half_size;	++ii)	{
												state[ii]	=	position[ii];
												deriv[intermediate_step][ii]		=	velocity[ii];
												deriv2[intermediate_step][ii]	=	accel[ii];
								}
				}

				else	{
								for	(int	ii	=	0;	ii	<	half_size;	++ii)	{
												state[ii]											=	position[ii];
												state[ii+half_size]	=	velocity[ii];
												deriv[intermediate_step][ii]											=	velocity[ii];
												deriv[intermediate_step][ii+half_size]	=	accel[ii];
								}
				}

				rc	=	integrate	();

				for	(int	ii	=	0;	ii	<	half_size;	++ii)	{
								position[ii]	=	state_ws[intermediate_step][ii];
								velocity[ii]	=	state_ws[intermediate_step][ii+half_size];
				}

				return	rc;
}

void	Trick::Integrator::state_reset	()	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
#else
				if	(intermediate_step	==	0)	{

								if	(verbosity)	message_publish(MSG_DEBUG,	"STATE	RESET");
								for	(int	i=0;	i<num_state	;	i++)	{
												*state_origin[i]	=	state[i];
								}
				}
#endif
}

#ifdef	USE_ER7_UTILS_INTEGRATORS
void	Trick::Integrator::state_in	(double*	arg1,	va_list	argp)	{
				int	i	=	0;
				double*	next_arg	=	arg1;
				if	(verbosity)	message_publish(MSG_DEBUG,	"LOAD	STATE:	");
				while	(next_arg	!=	(double*)	NULL)	{
								state[i]	=	*next_arg;
								if	(verbosity)	message_publish(MSG_DEBUG,	"		%g",	*next_arg);
								next_arg	=	va_arg(argp,	double*);
								i++;
				}
				if	(verbosity)	message_publish(MSG_DEBUG,	"\n");
}
#else
void	Trick::Integrator::state_in	(double*	arg1,	va_list	argp)	{
				if	(intermediate_step	==	0)	{
								int	i	=	0;
								double*	next_arg	=	arg1;
								state_origin[i]	=	next_arg;
								if	(verbosity)	std::cout	<<	"LOAD	STATE:	\n";
								while	(next_arg	!=	(double*)	NULL)	{
												state_origin[i]	=	next_arg;
												state[i]	=	*next_arg;
												if	(verbosity)	std::cout	<<	"		"	<<	*next_arg	<<	"\n";
												next_arg	=	va_arg(argp,	double*);
												i++;
								}
								state_origin[i]	=	(double*)NULL;
								if	(verbosity)	std::cout	<<	std::endl;
				}
}

void	Trick::Integrator::state_element_in	(unsigned	int	index	,	double*	state_p)	{
				if	(intermediate_step	==	0)	{
								state_origin[index]	=	state_p;
								state[index]	=	*state_p;
				}
}

#endif

void	Trick::Integrator::state_in	(double*	arg1,	...)	{
				va_list	argp;
				va_start(argp,	arg1);
				state_in(arg1,	argp);
				va_end(argp);
}

void	Trick::Integrator::deriv_in	(	double*	arg1,	va_list	argp)	{
				int	i	=	0;
				double*	next_arg	=	arg1;
				if	(verbosity)	message_publish(MSG_DEBUG,	"LOAD	DERIV:	");
				while	(next_arg	!=	(double*)	NULL)	{
								deriv[intermediate_step][i]	=	*next_arg;
								if	(verbosity)	message_publish(MSG_DEBUG,"		%g",	*next_arg);
								next_arg	=	va_arg(argp,	double*);
								i++;
				}
				if	(verbosity)	message_publish(MSG_DEBUG,"\n");
}

void	Trick::Integrator::deriv_in	(	double*	arg1,	...)	{
				va_list	argp;
				va_start(argp,	arg1);
				deriv_in(arg1,	argp);
				va_end(argp);
}

void	Trick::Integrator::deriv2_in	(	double*	arg1,	va_list	argp)	{
				int	i	=	0;
				double*	next_arg	=	arg1;
				if	(verbosity)	message_publish(MSG_DEBUG,	"LOAD	DERIV2:	");
				while	(next_arg	!=	(double*)	NULL)	{
								deriv2[intermediate_step][i]	=	*next_arg;
								if	(verbosity)	message_publish(MSG_DEBUG,"		%g",	*next_arg);
								next_arg	=	va_arg(argp,	double*);
								i++;
				}
				if	(verbosity)	message_publish(MSG_DEBUG,"\n");
}

void	Trick::Integrator::deriv2_in	(	double*	arg1,	...)	{
				va_list	argp;
				va_start(argp,	arg1);
				deriv2_in(arg1,	argp);
				va_end(argp);
}

void	Trick::Integrator::state_out	(double*	arg1,	va_list	argp)	{
				int	i	=	0;
				double*	next_arg	=	arg1;
				if	(verbosity)	message_publish(MSG_DEBUG,	"UNLOAD	STATE:	");
				while	(next_arg	!=	(double*)	NULL)	{
								*next_arg	=	state_ws[intermediate_step][i];
								if	(verbosity)	message_publish(MSG_DEBUG,"		%g",	*next_arg);
								next_arg	=	va_arg(argp,	double*);
								i++;
				}
				if	(verbosity)	message_publish(MSG_DEBUG,"\n");
}

void	Trick::Integrator::state_out	(double*	arg1,	...)	{
				va_list	argp;
				va_start(argp,	arg1);
				state_out(arg1,	argp);
				va_end(argp);
}

bool	Trick::Integrator::get_first_step_deriv()	{
				return	(first_step_deriv);
}

void	Trick::Integrator::set_first_step_deriv(bool	first_step)	{
				first_step_deriv	=	first_step;
}

bool	Trick::Integrator::get_last_step_deriv()	{
				return	(last_step_deriv);
}

void	Trick::Integrator::set_last_step_deriv(bool	last_step)	{
				last_step_deriv	=	last_step;
}

void	Trick::Integrator::set_verbosity(int	level)	{
				verbosity	=	level;
}
