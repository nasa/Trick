
#include	"trick/exec_proto.h"
#include	"trick/SimTime.hh"

#define	GET_TIME(	GMT	,	offset	)	\
	{	\
		double	temp	;	\
	\
		/*	Convert	the	met	to	a	gmt	format	*/	\
		temp	=	(	exec_get_sim_time()	+	offset	)	/	86400.0	;	\
		GMT.day		=	(int)temp	;	\
		temp	=	(temp	-	(double)(GMT.day))	*	24.0	;	\
		GMT.hour	=	(int)temp	;	\
		temp	=	(temp	-	(double)(GMT.hour))	*	60.0	;	\
		GMT.min		=	(int)temp	;	\
		GMT.sec	=	(temp	-	(double)(GMT.min))	*	60.0	;	\
}

Trick::SimTime	*	the_simtime	;

Trick::SimTime::SimTime()	{

				gmt_ref.day	=	0	;
				gmt_ref.hour	=	0	;
				gmt_ref.min	=	0	;
				gmt_ref.sec	=	0	;

				met_ref.day	=	0	;
				met_ref.hour	=	0	;
				met_ref.min	=	0	;
				met_ref.sec	=	0	;

				the_simtime	=	this	;
}

int	Trick::SimTime::init_times()	{

				gmt_sec_ref	=	gmt_ref.day	*	86400
																+	gmt_ref.hour	*	3600
																+	gmt_ref.min	*	60
																+	gmt_ref.sec;

				met_sec_ref	=	met_ref.day	*	86400
																+	met_ref.hour	*	3600
																+	met_ref.min	*	60
																+	met_ref.sec;

				return	0	;
}

double	Trick::SimTime::get_rettime()	{

				return(exec_get_sim_time());

}

double	Trick::SimTime::get_gmttime()	{

				return(exec_get_sim_time()	+	gmt_sec_ref);

}

double	Trick::SimTime::get_mettime()	{

				return(exec_get_sim_time()	+	met_sec_ref);

}

TIME_OFFSET	*	Trick::SimTime::get_rettime_struct()	{

				GET_TIME(ret_time,	0.0);

				return(&ret_time)	;
}

TIME_OFFSET	*	Trick::SimTime::get_gmttime_struct()	{

				GET_TIME(gmt_time,	gmt_sec_ref);

				return(&gmt_time)	;
}

TIME_OFFSET	*	Trick::SimTime::get_mettime_struct()	{

				GET_TIME(met_time,	met_sec_ref);

				return(&met_time)	;
}

