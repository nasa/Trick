
#include	"trick/integrator_c_intf.h"
#include	"trick/Integrator.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	<stdarg.h>
#include	<iostream>

/*	GLOBAL	Integrator.	*/
extern	Trick::Integrator*	trick_curr_integ	;

extern	"C"	int	integrate()	{
				return	(trick_curr_integ->integrate());
}

extern	"C"	int	integrate_1st_order_ode(const	double*	deriv,	double*	state)	{
				return	(trick_curr_integ->integrate_1st_order_ode(deriv,	state));
}

extern	"C"	int	integrate_2nd_order_ode(const	double*	acc,	double*	vel,	double	*	pos)	{
				return	(trick_curr_integ->integrate_2nd_order_ode(acc,	vel,	pos));
}

extern	"C"	double	get_integ_time()	{
	return	(trick_curr_integ->time);
}

extern	"C"	double	get_integ_dt(void)	{
	return	(trick_curr_integ->dt);
}

extern	"C"	double	get_integ_target_time(void)	{
	return	(trick_curr_integ->target_integ_time);
}

extern	"C"	void	set_integ_time(double	time_value)	{
				trick_curr_integ->time	=	time_value;
				trick_curr_integ->target_integ_time	=	time_value;
}

extern	"C"	void	reset_state()	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
#else
				trick_curr_integ->state_reset();
#endif
}

extern	"C"	void	load_state(double*	arg1,	...	)	{
				va_list	argp;
				if	(trick_curr_integ	!=	NULL)	{
								va_start(argp,	arg1);
								trick_curr_integ->state_in(arg1,	argp);
								va_end(argp);
				}	else	{
							message_publish(MSG_ERROR,	"Integ	load_state	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	void	load_indexed_state(unsigned	int	index	,	double	state)	{

				if	(trick_curr_integ	!=	NULL)	{
								if	(trick_curr_integ->verbosity)	message_publish(MSG_DEBUG,"	LOAD	INDEXED	STATE:	%f\n",	state);
								trick_curr_integ->state[index]	=	state	;
				}	else	{
							message_publish(MSG_ERROR,	"Integ	load_indexed_state	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

#ifndef	USE_ER7_UTILS_INTEGRATORS
//	Warning:	state_p	should	never	point	to	an	automatic	local	variable.
extern	"C"	void	load_state_element(unsigned	int	index	,	double*	state_p)	{

				if	(trick_curr_integ	!=	NULL)	{
								trick_curr_integ->state_element_in	(index,	state_p);
				}	else	{
							message_publish(MSG_ERROR,	"Integ	load_indexed_state	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}
#endif

extern	"C"	void	load_deriv(	double*	arg1,	...)	{

				va_list	argp;
				if	(trick_curr_integ	!=	NULL)	{
								va_start(argp,	arg1);
								trick_curr_integ->deriv_in(arg1,	argp);
								va_end(argp);
				}	else	{
							message_publish(MSG_ERROR,	"Integ	load_deriv	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	void	load_indexed_deriv(unsigned	int	index	,	double	deriv)	{

				if	(trick_curr_integ	!=	NULL)	{
								if	(trick_curr_integ->verbosity)	message_publish(MSG_DEBUG,"LOAD	INDEXED	DERIV:	%f\n",	deriv);
								trick_curr_integ->deriv[trick_curr_integ->intermediate_step][index]	=	deriv	;
				}	else	{
								message_publish(MSG_ERROR,	"Integ	load_indexed_deriv	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	void	load_deriv2(	double*	arg1,	...)	{

				va_list	argp;
				if	(trick_curr_integ	!=	NULL)	{
								va_start(argp,	arg1);
								trick_curr_integ->deriv2_in(arg1,	argp);
								va_end(argp);
				}	else	{
							message_publish(MSG_ERROR,	"Integ	load_deriv2	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	void	load_indexed_deriv2(unsigned	int	index	,	double	deriv2)	{

				if	(trick_curr_integ	!=	NULL)	{
								if	(trick_curr_integ->verbosity)	message_publish(MSG_DEBUG,"LOAD	INDEXED	DERIV2:	%f\n",	deriv2);
								trick_curr_integ->deriv2[trick_curr_integ->intermediate_step][index]	=	deriv2	;
				}	else	{
								message_publish(MSG_ERROR,	"Integ	load_indexed_deriv2	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	void	unload_state	(double*	arg1,	...)	{

				va_list	argp;
				if	(trick_curr_integ	!=	NULL)	{
								va_start(argp,	arg1);
								trick_curr_integ->state_out(arg1,	argp);
								va_end(argp);
				}	else	{
							message_publish(MSG_ERROR,	"Integ	unload_state	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}
}

extern	"C"	double	unload_indexed_state	(unsigned	int	index)	{

				if	(trick_curr_integ	!=	NULL)	{
								if	(trick_curr_integ->verbosity)	message_publish(MSG_DEBUG,"UNLOAD	INDEXED	STATE:	%u\n",	index);
								return(trick_curr_integ->state_ws[trick_curr_integ->intermediate_step][index])	;
				}	else	{
								message_publish(MSG_ERROR,	"Integ	unload_indexed_state	ERROR:	trick_curr_integ	is	not	set.\n")	;
				}

				return	0.0	;
}

extern	"C"	int	get_intermediate_step()	{
				return(	trick_curr_integ->intermediate_step);
}

extern	"C"	void	set_intermediate_step(int	intermediate_step_value)	{
				trick_curr_integ->intermediate_step	=	intermediate_step_value;
}

extern	"C"	int	get_integ_type()	{
				return(	trick_curr_integ->get_Integrator_type());
}
