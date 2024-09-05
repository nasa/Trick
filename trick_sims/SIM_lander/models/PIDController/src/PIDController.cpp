
#include	"PIDController.hh"

PIDController::PIDController(	double	kp,	double	ki,	double	kd,	double	omax,	double	omin,	double	dt,	double	tc	)	{
				Kp=kp;
				Ki=ki;
				Kd=kd;
				Dt=dt;
				k	=	dt/tc;
				integral	=	0.0;
				out_max	=	omax;
				out_min	=	omin;
				error	=	0.0;
				previous_error	=	0.0;
				prev_setpoint_value	=	0.0;
				integration_enabled	=	true;
}

double	PIDController::getOutput(double	setpoint_value,	double	measured_value)	{

				double	error_unfiltered	=	setpoint_value	-	measured_value;

				//	Low	pass	filter	for	our	error	signal.
				error	=	error	+	k	*	(error_unfiltered	-	error);

				//	If	we	suddenly	change	the	set	point	value	then	we	need	to	reset
				//	our	previous_error,	otherwise	we'll	get	a	spike	in	our	derivative.
				if	(prev_setpoint_value	!=	setpoint_value)	{
								previous_error	=	error;
				}

				//	Integration	is	normally	enabled	unless	we	are	mitigating	integral	windup.
				if	(integration_enabled)	{
								integral	=	integral	+	error	*	Dt;
				}

				double	derivative	=	(error	-	previous_error)	/	Dt;

				double	pterm	=	Kp	*	error;
				double	iterm	=	Ki	*	integral;
				double	dterm	=	Kd	*	derivative;
				double	output	=	pterm	+	iterm	+	dterm;

				bool	same_sign;
				//	If	the	signs	of	the	error	and	the	output	are	the	same,
				//	then	the	output	isn't	reducing	the	error.
				if	((error	*	output)	>	0.0)	{
								same_sign	=	1;
				}	else	{
								same_sign	=	0;
				}

				bool	output_limited;
				//	Check	for	saturation.
				if	(output	>	out_max)	{
								output	=	out_max;
								output_limited	=	1;
				}
				if	(output	<	out_min)	{
								output	=	out_min;
								output_limited	=	1;
				}

				//	If	the	output	isn't	reducing	the	error,	and	is	being	limited,
				//	then	we	need	to	inhibit	integration	to	prevent	integral	windup.
				if	(same_sign	&&	output_limited)	{
								integration_enabled	=	false;
				}	else	{
								integration_enabled	=	true;
				}

				previous_error	=	error;
				prev_setpoint_value	=	setpoint_value;

				return	(	output	);
}
