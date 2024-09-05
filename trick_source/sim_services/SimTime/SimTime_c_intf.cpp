
#include	"trick/SimTime.hh"
#include	"trick/simtime_proto.h"

extern	Trick::SimTime	*	the_simtime	;

extern	"C"	double	get_rettime(void)	{
				return	(the_simtime->get_rettime())	;
}

extern	"C"	double	get_gmttime(void)	{
				return	(the_simtime->get_gmttime())	;
}

extern	"C"	double	get_mettime(void)	{
				return	(the_simtime->get_mettime())	;
}

extern	"C"	GMTTIME	*	get_rettime_struct(void)	{
				return	(the_simtime->get_rettime_struct())	;
}

extern	"C"	GMTTIME	*	get_gmttime_struct(void)	{
				return	(the_simtime->get_gmttime_struct())	;
}

extern	"C"	GMTTIME	*	get_mettime_struct(void)	{
				return	(the_simtime->get_mettime_struct())	;
}

