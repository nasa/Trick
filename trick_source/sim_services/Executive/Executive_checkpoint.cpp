
#include	<iostream>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"
#include	"trick/memorymanager_c_intf.h"

/**
@details
-#	Save	the	number	of	jobs	in	the	scheduler
-#	Copy	all	of	the	job	information	in	a	checkpointable	array
*/
int	Trick::Executive::checkpoint()	{

				unsigned	int	ii	;

				/*
							The	all_jobs_vector	contains	memory	that	is	not	known	to	the	memory	manager.		We	need	to
							copy	the	information	into	memory	that	is	declared	to	the	memory	manager.
					*/
				/*	save	the	number	of	jobs	in	the	scheduler	*/
				num_all_jobs	=	all_jobs_vector.size()	;

				/*	copy	all	of	the	job	information	in	a	checkpointable	array	*/
				if	(	num_all_jobs	>	0	)	{
								all_jobs_for_checkpoint	=	(Trick::JobData	*)TMM_declare_var(	TRICK_STRUCTURED,	"Trick::JobData",
									0	,	"all_jobs"	,	1	,	(int	*)&num_all_jobs)	;
								for	(	ii	=	0	;	ii	<	num_all_jobs	;	ii++	)	{
												all_jobs_for_checkpoint[ii]	=	*(all_jobs_vector[ii])	;
								}
				}

				return(0)	;

}
