
#include	<stdlib.h>
#include	<string.h>
#include	"trick/collect_macros.h"

/**
@brief
Adds	the	addess,	collectee,	to	the	list	pointed	to	by	collector.		Returns	a	new	pointer
to	the	new	list.		The	old	list	a	collector	will	be	freed.
@details
-#	Get	the	current	list	length
-#	Allocate	enough	space	to	accomodate	the	current	list	+	1.	(+1	for	the	size	of	the	list)
-#	Set	the	size	of	the	new	list	in	the	first	slot	of	the	new	list.
-#	Copy	the	old	list	to	the	new.
-#	Append	the	new	item	to	the	new	list.
-#	Delete	the	old	list
-#	Return	the	new	list
*/
void	**	add_collect(	void	**	collector	,	void	*	collectee	)	{

				int	collect_size	=	NUM_COLLECT(collector)	;
				void	**	new_list	;
				long	*	lp	;

				//	create	a	new	list
				new_list	=	(void**)malloc((collect_size	+	2)	*	sizeof(void*));

				//	set	the	list	size
				lp	=	(	long	*)new_list	;
				*lp	=	collect_size	+	1	;

				//	copy	old	list
				new_list++	;
				if	(	collect_size	>	0	)	{
								memcpy(	new_list	,	collector	,	collect_size	*	sizeof(void	*))	;
				}
				new_list[collect_size]	=	collectee	;

				//	remove	old	list
				if	(	collector	)	{
								free(collector	-	1)	;
				}

				return	new_list	;
}

/**
@brief
Deletes	the	addess,	collectee,	from	the	list	pointed	to	by	collector.		Returns	a	new	pointer
to	the	new	list.		The	old	list	a	collector	will	be	freed.
@details
-#	Get	the	current	list	length
-#	Count	the	number	of	items	in	the	list	that	do	not	match	the	collectee.
-#	Allocate	enough	space	to	accomodate	the	new	list.	(+1	for	the	size	of	the	list)
-#	Set	the	size	of	the	new	list	in	the	first	slot	of	the	new	list.
-#	Copy	all	of	the	items	that	do	not	match	collectee	to	the	new	list
-#	Delete	the	old	list
-#	Return	the	new	list
*/
void	**	delete_collect(	void	**	collector	,	void	*	collectee	)	{

				int	collect_size	=	NUM_COLLECT(collector)	;
				int	new_count	=	0	;
				void	**	new_list	;
				long	*	lp	;
				int	ii	,	jj	;


				for	(	ii	=	0	;	ii	<	collect_size	;	ii++	)	{
								if	(	collector[ii]	!=	collectee	)	{
												new_count++	;
								}
				}

				if	(	new_count	>	0	)	{
								//	create	a	new	list
								new_list	=	(void**)malloc((new_count	+	1)	*	sizeof(void*));

								//	set	the	list	size
								lp	=	(	long	*)new_list	;
								*lp	=	new_count	;

								//	copy	old	list
								new_list++	;
								jj	=	0	;
								for	(	ii	=	0	;	ii	<	collect_size	;	ii++	)	{
												if	(	collector[ii]	!=	collectee	)	{
																new_list[jj++]	=	collector[ii]	;
												}
								}
				}	else	{
								new_list	=	NULL	;
				}

				//	remove	old	list
				if	(	collector	)	{
								free(collector	-	1)	;
				}

				return	new_list	;
}
