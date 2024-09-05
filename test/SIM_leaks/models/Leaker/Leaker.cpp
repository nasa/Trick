#include	<vector>
#include	<stdlib.h>
#include	<string.h>

/*	Model	include	files.	*/
#include	"sim_services/MemoryManager/include/memorymanager_c_intf.h"

std::vector<	int	>	dv	;

					/*	ENTRY	POINT	*/
int	Leaker()	{

			int	*	i_malloc	=	(int	*)malloc(	sizeof(int)	)	;

			int	*	i_calloc	=	(int	*)calloc(	1	,	sizeof(int)	)	;

#if	0
			int	*	i_realloc	=	NULL	;
			i_realloc	=	(int	*)realloc(	i_realloc,	sizeof(int)	)	;
#endif

			int	*	i_new	=	new	int	;

			int	*	ia_new	=	new	int[10]	;

			char	*	str	=	strdup("hello")	;

			int	ii	;
			for	(	ii	=	0	;	ii	<	100	;	ii++	)	{
							dv.push_back(ii)	;
			}

			/*	RETURN	*/
			return(	0	);

}

