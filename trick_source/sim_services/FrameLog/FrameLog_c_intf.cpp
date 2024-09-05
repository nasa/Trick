#include	<stdio.h>
#include	"trick/FrameLog.hh"

/*	Global	singleton	pointer	to	the	framelog	class	*/
extern	Trick::FrameLog	*	the_fl	;

/*************************************************************************/
/*	These	routines	are	the	"C"	interface	to	framelog	instrumentation					*/
/*************************************************************************/

/**
	*	@relates	Trick::FrameLog
	*	@copydoc	Trick::FrameLog::framelog_on
	*	C	wrapper	for	Trick::FrameLog::framelog_on
	*/
extern	"C"	int	frame_log_on(void)	{
				if	(the_fl	!=	NULL)	{
								return	the_fl->framelog_on()	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::FrameLog
	*	@copydoc	Trick::FrameLog::framelog_off
	*	C	wrapper	for	Trick::FrameLog::framelog_off
	*/
extern	"C"	int	frame_log_off(void)	{
				if	(the_fl	!=	NULL)	{
								return	the_fl->framelog_off()	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::FrameLog
	*	@copydoc	Trick::FrameLog::set_max_samples
	*	C	wrapper	for	Trick::FrameLog::set_max_samples
	*/
extern	"C"	int	frame_log_set_max_samples(int	num)	{
				if	(the_fl	!=	NULL)	{
								return	the_fl->set_max_samples(num)	;
				}
				return(0)	;
}
