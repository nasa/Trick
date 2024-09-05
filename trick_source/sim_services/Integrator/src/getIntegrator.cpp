/*******************************************************************************

Purpose:
		(Implement	free	function	TrickgetIntegrator.)

Programmers:
		(((Alex	Lin)					(NASA)	(2009))
			((John	Penn)				(L3)	(2010))
			((Donna	Panter)	(L3)	(2012))
			((David	Hammen)	(Odyssey)	(July	2012)	(JEOD_2.2	#462)	(Joint	Trick/JEOD	integration	framework))
		)

*******************************************************************************/

//	System	includes
#include	<iostream>

//	Trick	includes
#include	"trick/memorymanager_c_intf.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

#ifdef	USE_ER7_UTILS_INTEGRATORS
//	Interface	includes
#include	"er7_utils/interface/include/alloc.hh"

//	Integration	includes
#include	"er7_utils/integration/core/include/integrator_constructor_factory.hh"
#endif

//	Local	includes
#include	"trick/Integrator.hh"
#include	"trick/IntegAlgorithms.hh"

#ifdef	USE_ER7_UTILS_INTEGRATORS
/**
	*	Allocate	an	integrator	of	type	T.
	*/
template	<typename	T>
static	T*	allocate_integrator	(
			unsigned	int	state_size,
			double	delta_t)
{
			return	er7_utils::alloc::allocate_object<T,	unsigned	int,	double>	(	state_size,	delta_t);
}
#endif

/**
	*	Create	an	integrator	of	the	specified	type.
	*	Note	well:	The	caller	is	responsible	for	deleting	the	allocated	integrator.
	*	@param[in]	alg									Integration	technique
	*	@param[in]	state_size		Number	of	elements	in	the	state	vector
	*	@param[in]	delta_t					Simulation	engine	time	step
	*/
Trick::Integrator*
Trick::getIntegrator	(	Integrator_type	alg,	unsigned	int	state_size,	double	delta_t)	{
			switch(alg)	{

			case	Euler	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	er7_utils::alloc::allocate_object<Trick::Euler_Integrator,unsigned	int,	double>(	state_size,	delta_t);
#else
						Trick::Euler_Integrator*	I	=	INTEG_NEW(	Trick::Euler_Integrator);
						I	=	new	(I)	Trick::Euler_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Euler_Cromer	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::Euler_Cromer_Integrator>	(	state_size,	delta_t);
#else
						Trick::Euler_Cromer_Integrator*	I	=	INTEG_NEW(	Trick::Euler_Cromer_Integrator);
						I	=	new	(I)	Trick::Euler_Cromer_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Nystrom_Lear_2	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::NL2_Integrator>	(
																state_size,	delta_t);
#else
						Trick::NL2_Integrator*	I	=	INTEG_NEW(	Trick::NL2_Integrator);
						I	=	new	(I)	Trick::NL2_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Runge_Kutta_2	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::RK2_Integrator>	(
																state_size,	delta_t);
#else
						Trick::RK2_Integrator*	I	=	INTEG_NEW(	Trick::RK2_Integrator);
						I	=	new	(I)	Trick::RK2_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Modified_Midpoint_4	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::MM4_Integrator>	(
																state_size,	delta_t);
#else
						Trick::MM4_Integrator*	I	=	INTEG_NEW(	Trick::MM4_Integrator);
						I	=	new	(I)	Trick::MM4_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Runge_Kutta_4	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::RK4_Integrator>	(
																state_size,	delta_t);
#else
						Trick::RK4_Integrator*	I	=	INTEG_NEW(	Trick::RK4_Integrator);
						I	=	new	(I)	Trick::RK4_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Runge_Kutta_Gill_4	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::RKG4_Integrator>	(
																state_size,	delta_t);
#else
						Trick::RKG4_Integrator*	I	=	INTEG_NEW(	Trick::RKG4_Integrator);
						I	=	new	(I)	Trick::RKG4_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Runge_Kutta_Fehlberg_45	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::RKF45_Integrator>	(
																state_size,	delta_t);
#else
						Trick::RKF45_Integrator*	I	=	INTEG_NEW(	Trick::RKF45_Integrator);
						I	=	new	(I)	Trick::RKF45_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	Runge_Kutta_Fehlberg_78	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::RKF78_Integrator>	(
																state_size,	delta_t);
#else
						Trick::RKF78_Integrator*	I	=	INTEG_NEW(	Trick::RKF78_Integrator);
						I	=	new	(I)	Trick::RKF78_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			case	ABM_Method	:	{
#ifdef	USE_ER7_UTILS_INTEGRATORS
						return	allocate_integrator<Trick::ABM_Integrator>	(
																state_size,	delta_t);
#else
						Trick::ABM_Integrator*	I	=	INTEG_NEW(	Trick::ABM_Integrator);
						I	=	new	(I)	Trick::ABM_Integrator(	state_size,	delta_t);
						return	I;
#endif
			}	break;

			default	:
						message_publish	(MSG_ERROR,
																							"Integ	getIntegrator	ERROR:	No	such	Integrator.\n")	;
						return	(NULL);
			}
}

#if	0
/**
	*	Create	an	integrator	of	the	specified	type.
	*	Note	well:	The	caller	is	responsible	for	deleting	the	allocated	integrator.
	*	@param[in]	alg									Integration	technique
	*	@param[in]	state_size		Number	of	elements	in	the	state	vector
	*	@param[in]	delta_t					Simulation	engine	time	step
	*/
Trick::Integrator*
Trick::getEr7Integrator	(
			er7_utils::Integration::Technique	alg,
			unsigned	int	state_size,
			double	delta_t)
{
			er7_utils::IntegratorConstructor	*	integ_constructor	=
						er7_utils::IntegratorConstructorFactory::create	(alg);

			if	(integ_constructor->is_second_order_ode_technique())	{
						bool	use_deriv2	=	alg	==	er7_utils::Integration::SymplecticEuler;
						return	er7_utils::alloc::allocate_object<
																			er7_utils::TrickSecondOrderOdeIntegrator,
																			er7_utils::IntegratorConstructor&,
																			unsigned	int,	double,	bool>	(
																*integ_constructor,	state_size,	delta_t,	use_deriv2);
			}
			else	{
						return	er7_utils::alloc::allocate_object<
																			er7_utils::TrickFirstOrderOdeIntegrator,
																			er7_utils::IntegratorConstructor&,
																			unsigned	int,	double>	(
																*integ_constructor,	state_size,	delta_t);
			}
}
#endif
