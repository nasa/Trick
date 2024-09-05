
#include	"DPM_time_constraints.hh"
#include	"float.h"

void	DPM_time_constraints::Initialize()	{

		start_time	=	0.0;
		stop_time	=	DBL_MAX;
		frequency	=	DBL_MAX;
}

//	CONSTRUCTOR.
DPM_time_constraints::DPM_time_constraints()	{
		Initialize();
}

//	CONSTRUCTOR.
DPM_time_constraints::DPM_time_constraints(double	start,
										double	stop,
										double	freq)	{
		Initialize();
		if	(start	>=	0.0)	{
				start_time	=	start;
		}
		if	(stop	>	start_time)	{
				stop_time	=	stop;
		}
		if	(freq	>=	0.0)	frequency	=	freq;
}

//	MEMBER	FUNCTION
double	DPM_time_constraints::getStart()	{
		return(	start_time);
}

//	MEMBER	FUNCTION
double	DPM_time_constraints::getStop()	{
		return(	stop_time);
}

//	MEMBER	FUNCTION
double	DPM_time_constraints::getPeriod()	{
		if	(frequency	!=	0.0)	{
				return	(	1.0/frequency);
		}
		return	(0.0);
}

//	MEMBER	FUNCTION
double	DPM_time_constraints::getFrequency()	{
		return	(frequency);
}

//	MEMBER	FUNCTION
DPM_time_constraints	operator+	(	DPM_time_constraints	&tc1,
						DPM_time_constraints	&tc2)	{

		DPM_time_constraints	tc_out;

		if	(	tc1.start_time	>	tc2.start_time	)	{
				tc_out.start_time	=	tc1.start_time;
		}	else	{
				tc_out.start_time	=	tc2.start_time;
		}

		if	(	tc1.stop_time	<	tc2.stop_time	)	{
				tc_out.stop_time	=	tc1.stop_time;
		}	else	{
				tc_out.stop_time	=	tc2.stop_time;
		}

		if	(	tc1.frequency	<	tc2.frequency	)	{
				tc_out.frequency	=	tc1.frequency;
		}	else	{
				tc_out.frequency	=	tc2.frequency;
		}

		if	(tc_out.start_time	>	tc_out.stop_time)	{
				tc_out.start_time	=	tc_out.stop_time;
		}

		return	(tc_out);
}
