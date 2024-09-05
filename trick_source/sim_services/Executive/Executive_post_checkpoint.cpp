
#include	<iostream>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"
#include	"trick/memorymanager_c_intf.h"

int	Trick::Executive::post_checkpoint()	{

				TMM_delete_var_a(all_jobs_for_checkpoint)	;
				all_jobs_for_checkpoint	=	NULL	;
				return(0)	;

}
