
/*
			PURPOSE:	(	These	routines	get	the	sizes	of	memory	areas	pointed	to	by	ptr	)

			REFERENCE:

			ASSUMPTIONS	AND	LIMITATIONS:

			PROGRAMMERS:	(	(Alex	Lin)	(NASA)	(3/17/03)	)
	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"trick/MemoryManager.hh"

/*	This	routine	only	returns	the	size	if	ptr	points	to	the	beginning	of	the	allocation	area	*/
int	Trick::MemoryManager::get_size(void	*addr)	{

				ALLOC_INFO	*node;
				int	size	=	0;

				pthread_mutex_lock(&mm_mutex);
				node	=	get_alloc_info_at(addr);
				if	(node	!=	NULL)	{
								size	=	node->num;
				}
				pthread_mutex_unlock(&mm_mutex);

				return	(size);
}

/*	This	routine	returns	the	remaining	space	in	front	of	addr	in	the	current	allocation	area	*/
int	Trick::MemoryManager::get_truncated_size(void	*addr)	{

				ALLOC_INFO	*node;
				int	size	=	0;

				pthread_mutex_lock(&mm_mutex);
				node	=	get_alloc_info_of(addr);
				if	(node	!=	NULL)	{
								size	=	((int)	node->num	-	(((long)	addr	-	(long)	node->start)	/	node->size));
				}
				pthread_mutex_unlock(&mm_mutex);

				return	(size);
}

//	********************************************************
//	FIXME:	Is	the	following	routine	actually	used	somewhere?
//	********************************************************

/*	This	routine	returns	the	remaining	space	in	front	of	ptr	within	a	fixed	matrix	area	*/
int	Trick::MemoryManager::io_get_fixed_truncated_size(char	*ptr,	ATTRIBUTES	*	A,	char	*str,	int	dims,	ATTRIBUTES	*	left_type)
{

#if	1
				int	i	=	0,	j;
				long	offset;
				int	my_index[9];
				int	ret;
				int	size,	last_size;
				int	num_fixed_dims	=	0;

				/*	Find	the	variable	which	contains	the	address	pointed	to	by	ptr	*/
				while	((A[i].name[0]	!=	'\0')
											&&	(long)	ptr	>=	(long)	((long)	str	+	A[i].offset))	{
								i++;
				}

				i--;

				if	(A[i].attr	==	NULL)	{
								/*	If	ptr	is	pointing	to	an	array	determine	the	dimensions	to	get	to	the	element	pointed	to	by	ptr	*/
								if	(A[i].num_index	!=	0)	{

												offset	=	(long)	ptr	-	((long)	str	+	A[i].offset);
												size	=	last_size	=	A[i].size;

												/*	We	can	only	do	this	for	fixed	size	arrays.	*/
												for	(j	=	0;	j	<	A[i].num_index;	j++)	{
																if	(A[i].index[j].size	>	0)	{
																				num_fixed_dims++;
																}
												}

												if	(!num_fixed_dims)	{
																/*	This	is	a	pointer.	*/
																return	(1);
												}

												for	(j	=	num_fixed_dims	-	1;	j	>=	0;	j--)	{
																size	*=	A[i].index[j].size;
																my_index[j]	=	(int)	((offset	%	size)	/	last_size);
																offset	-=	last_size	*	my_index[j];
																last_size	=	size;
												}

												if	(A[i].num_index	-	dims	-	1	<	0)	{
																printf("Big	problems:	index	=	%i\n",	A[i].num_index	-	dims	-	1);
																exit(-1);
												}
												if	(A[i].index[A[i].num_index	-	dims	-	1].size	-	my_index[A[i].num_index	-	dims	-	1]	<	0)	{
																printf("More	trouble\n");
																exit(-1);
												}
												return	(A[i].index[A[i].num_index	-	dims	-	1].size	-	my_index[A[i].num_index	-	dims	-	1]);
								}	else	{
												/*	If	variable	is	a	single	value	return	1	*/
												if	((long)	ptr	==	(long)	((long)	str	+	A[i].offset))	{
																return	(1);
												}
												return	(0);
								}

				}	else	{

								/*	Variable	is	an	struct,	continue	to	call	io_get_var_name	*/
								if	(A[i].num_index	==	0)	{

												/*	If	looking	for	this	struct	type,	don't	bother	continuing	*/
												if	(A[i].attr	==	left_type->attr)	{
																return	(1);
												}
												/*	If	we	are	pointing	to	a	non-arrayed	struct	*/
												ret	=	io_get_fixed_truncated_size(ptr,	(ATTRIBUTES	*)	A[i].attr,	str	+	A[i].offset,	dims,	left_type);
								}	else	{
												/*	If	we	are	pointing	to	an	arrayed	struct	*/

												offset	=	(long)	ptr	-	((long)	str	+	A[i].offset);
												size	=	last_size	=	A[i].size;

												/*	We	can	only	do	this	for	fixed	size	arrays.	*/
												for	(j	=	A[i].num_index	-	1;	j	>=	0;	j--)	{
																size	*=	A[i].index[j].size;
																my_index[j]	=	(int)	((offset	%	size)	/	last_size);
																offset	-=	last_size	*	my_index[j];
																last_size	=	size;
												}

												/*	If	we	are	looking	for	this	struct	type	don't	bother	continuing	*/
												if	(A[i].attr	==	left_type->attr)	{
																return	(A[i].index[dims].size	-	my_index[dims]);
												}

												/*	Get	the	offset	into	the	array	that	ptr	points	to	*/
												offset	=	A[i].size;
												for	(j	=	0;	j	<	A[i].num_index;	j++)	{
																offset	*=	my_index[j];
												}

												ret	=
																io_get_fixed_truncated_size(ptr,	(ATTRIBUTES	*)	A[i].attr,	str	+	A[i].offset	+	offset,	dims,	left_type);
								}

								return	(ret);

				}

#endif
				return	(0);

}
