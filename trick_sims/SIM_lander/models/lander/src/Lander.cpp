/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Simulate	a	satellite	orbiting	the	Earth.	)
LIBRARY	DEPENDENCY:
				((Lander.o))
*******************************************************************************/
#include	"lander/include/Lander.hh"

#include	"trick/integrator_c_intf.h"
#include	<math.h>
#include	<iostream>

#define	RAD_PER_DEG	(M_PI	/	180.0)
#define	DEG_PER_RAD	(180.0/	M_PI)

int	Lander::default_data()	{

				pos[0]	=	0.0;
				pos[1]	=	1.8;
				vel[0]	=	0.0;
				vel[1]	=	0.0;
				pitch	=	0.0;
				pitchDot	=	0.0;
				pitchDDot	=	0.0;

				thrust_max	=	6500;
				rcs_thrust	=	50;
				rcs_offset	=	1.15;
				mass	=	2000;
				moment_of_inertia	=	2000;
				main_engine_offset	=	1.0;
				g	=	1.62;																		//	Gravitional	acceleration	on	the	moon.

				RCS_pitch_dot_limit	=	(RAD_PER_DEG	*	5.0);

				nozzle_angle	=	0.0;
				throttle	=	0;
				rcs_command	=	0;

				//	Controls
				manual_throttle_change_command	=	0;
				manual_rcs_command	=	0;
				altitudeCtrlEnabled	=	0;
				downRangeCtrlEnabled	=	0;

				//	Auto	Control	Parameters
				x_cmd	=	40.0;
				y_cmd	=	0.0;

				return	(0);
}

int	Lander::state_init()	{

				RCS_pitch_ddot_limit	=	(rcs_thrust	*	rcs_offset)	/	moment_of_inertia;

				//	=========================================================================
				//		Horizontal	Position	Controllers

				//	Horizontal	Position	Controller
				//	Generates	x-axis	(horizontal)	velocity	commands.
				posxCntrl		=	new	PIDController(	0.08,	0.002,	0.20,	10.0,	-10.0,		0.10,	0.10	);

				//	Horizontal	Velocity	Controller
				//	Generates	x-axis	(horizontal)	acceleration	commands.
				double	ax_limit	=	0.30	*	(thrust_max	/	mass);
				velxCntrl		=	new	PIDController(	0.18,	0.0,	0.0,	ax_limit,	-ax_limit,		0.10,	0.10	);

				//	=========================================================================
				//	Vertical	Position	Controllers	(Below	100	meters)

				//	Vertical	Position	Controller
				//	Generates	y-axis	(vertical)	velocity	commands.
				posyCntrl		=	new	PIDController(	0.4,	0.0001,	0.2,	5.0,		-2.0,		0.10,	0.10	);

				//	Vertical	Velocity	Controller
				//	Generates	y-axis	(vertical)	acceleration	commands.
				double	ay_limit	=	0.95	*	(thrust_max	/	mass);
				velyCntrl		=	new	PIDController(	0.5,	0.1,	0.0,	ay_limit,	-ay_limit,		0.10,	0.10	);

				//	=========================================================================
				//	Vertical	Position	Controller	(Above	100	meters)
				//	Generates	y-axis	(vertical)	acceleration	commands.
				//	Setpoint:	Potential	Energy	per	Kg	due	to	gravity	between	the	commanded
				//	altitude	and	the	surface.
				//	Measured	value:	Total	Energy	per	Kg	(Potential	+	Kinetic)	due	to	the
				//	current	altitude	and	altitude	rate.
				energy_controller		=	new	PIDController(	0.1,	0.0001,	0.2,	30.0,		0.00,			0.10,	0.10	);

				//	=========================================================================
				//	Pitch	Controllers	(when	throttle	>	10)

				//	Pitch	Controller
				//	Generates	pitch	rate	commands	(radians	/	sec)
				nozzlePitchController	=	new	PIDController(	0.50,	0.0,	0.0,	RAD_PER_DEG	*	20.0,	-RAD_PER_DEG	*	20.0,	0.10	,	0.10);

				//	Pitch	Rate	Controller
				//	Generates	pitch	angular	acceleration	commands	(radians	/sec^2)
				nozzlePitchRateController	=	new	PIDController(	0.50,	0.0,	0.0,	RAD_PER_DEG	*	10.0,	-RAD_PER_DEG	*	10.0,	0.10	,	0.10);

				//	=========================================================================
				//	Pitch	Controller	(when	throttle	<	10)

				//	RCS	Pitch	Controller
				//	Generates	pitch	rate	commands	(radians	/	sec)	for	the	RCS.
				RCSpitchController	=	new	PIDController(	0.30,	0.0,	0.0,	RCS_pitch_dot_limit,	-RCS_pitch_dot_limit,	0.10	,	0.10);

				//	RCS	Pitch	Rate	Controller
				//	Generates	pitch	rate	commands	(radians	/	sec)	for	the	RCS.
				RCSpitchRateController	=	new	PIDController(	0.60,	0.0,	0.0,	RCS_pitch_ddot_limit,	-RCS_pitch_ddot_limit,	0.10	,	0.10);

				return	(0);
}

int	Lander::control()	{

				rcs_command	=	0;

				if	(altitudeCtrlEnabled)	{

								double	Eppkg	=	g	*	y_cmd;																																//	Potential	Energy	per	kg.
								double	Etpkg	=	g	*	pos[1]	+	0.5	*	fabs(vel[1])	*	vel[1];	//	Total	Energy	per	kg.
								double	Ediff	=	fabs(Eppkg	-	Etpkg);

								if	((pos[1]	<	100.0)	&&	(Ediff	<	50.0)	){
													vy_cmd	=	posyCntrl->getOutput(y_cmd,		pos[1]);
													ay_cmd	=	velyCntrl->getOutput(vy_cmd,	vel[1]);
								}	else	{
												//	This	controller	is	intended	to	be	more	energy	efficient.
													ay_cmd	=	energy_controller->getOutput(
																		g	*	y_cmd,																																			//	Potential	Energy
																		g	*	pos[1]	+	0.5	*	fabs(vel[1])	*	vel[1]					//	Kinetic	Energy
														);
								}

								if	((downRangeCtrlEnabled)	&&	(throttle	>=	10))	{

												vx_cmd	=	posxCntrl->getOutput(x_cmd,	pos[0]);
												ax_cmd	=	velxCntrl->getOutput(vx_cmd,	vel[0]);

												if	(ay_cmd	>	0.0)	{
																pitch_cmd	=	-atan(	ax_cmd	/	ay_cmd	);
												}	else	{
																pitch_cmd	=	0.0;
												}

												pitch_dot_cmd		=	nozzlePitchController->getOutput(pitch_cmd,	pitch);
												pitch_ddot_cmd	=	nozzlePitchRateController->getOutput(pitch_dot_cmd,	pitchDot);
												double	thrust	=	(throttle	/	100.0)	*	thrust_max;
												//	Calculate	the	nozzle	angle	that	would	give	us	the	commanded	pitch	angular	acceleration.
												nozzle_angle	=	asin(	(moment_of_inertia	*	pitch_ddot_cmd)/	(main_engine_offset	*	thrust)	);


								}	else	{
												vx_cmd	=	0.0;
												ax_cmd	=	0.0;
												nozzle_angle	=	0.0;

												pitch_cmd	=	0.0;
												pitch_dot_cmd	=	RCSpitchController->getOutput(pitch_cmd,	pitch);
												pitch_ddot_cmd	=	RCSpitchRateController->getOutput(pitch_dot_cmd,	pitchDot);
												if	(	pitch_ddot_cmd	>	0.1	*	RCS_pitch_ddot_limit)	{
																rcs_command	=	1;
												}	else	if	(pitch_ddot_cmd	<	-0.1	*	RCS_pitch_ddot_limit)	{
																rcs_command	=	-1;
												}	else	{
																rcs_command	=	0;
												}
								}

								double	a_cmd	=	sqrt(ax_cmd	*	ax_cmd	+	ay_cmd	*	ay_cmd);
								throttle	=	(a_cmd	*	mass	*	100)	/	thrust_max;

				}

				throttle	+=	manual_throttle_change_command;
				if	(throttle	>	100	)	throttle	=	100;
				if	(throttle	<			0)	throttle	=	0;

				rcs_command	+=	manual_rcs_command;
				if	(rcs_command	>		1)	rcs_command	=		1;
				if	(rcs_command	<	-1)	rcs_command	=	-1;

				return	(0);
}

int	Lander::state_deriv()	{

			double	thrust	=	(throttle	/	100.0)	*	thrust_max;

				acc[0]	=	(thrust	*	-sin(pitch))	/	mass;
				acc[1]	=	(thrust	*		cos(pitch))	/	mass	-	g;

				double	torque_main_engine	=	main_engine_offset	*	thrust	*	sin(	nozzle_angle	);
				double	torque_rcs	=	rcs_command	*	rcs_thrust	*	rcs_offset;
				double	torque_total	=	torque_main_engine	+	torque_rcs;

				pitchDDot	=	torque_total	/	moment_of_inertia;

				return(0);
}

int	Lander::state_integ()	{

			int	integration_step;

			load_state	(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	&pitch,	&pitchDot,	(double*)0);
			load_deriv	(	&vel[0],	&vel[1],	&acc[0],	&acc[1],	&pitchDot,	&pitchDDot,	(double*)0);

			integration_step	=	integrate();

			unload_state(	&pos[0],	&pos[1],	&vel[0],	&vel[1],	&pitch,	&pitchDot,	(double*)0);

			return(integration_step);
}

int	Lander::check_ground_contact()	{
				if	(pos[1]	<	1.8)	{
								pos[1]	=	1.8;
								vel[0]	=	0.0;
								vel[1]	=	0.0;
								pitch	=	0.0;
								pitchDot	=	0.0;
				}
				return(0);
}
