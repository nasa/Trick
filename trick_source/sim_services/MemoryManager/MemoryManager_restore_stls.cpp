
#include	<algorithm>

#include	"trick/MemoryManager.hh"
#include	"trick/attributes.h"

//	MEMBER	FUNCTION
void	Trick::MemoryManager::restore_stls(	ALLOC_INFO*	alloc_info	)	{

				ATTRIBUTES*	reference_attr;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(alloc_info	==	NULL)	{
								std::cerr	<<	"ERROR:	Trick::MemoryManager::restore_stls	called	with	alloc_info	==	NULL."	<<	std::endl;
								std::cout.flush();
								return;
				}

				//	If	there	is	no	name	associated	with	this	alloc_info,	we	won't	be	able	to	find	any	stls	associated	with	it.
				if	(alloc_info->name	==	NULL)	{
								return;
				}

				if	(debug_level)	{
								std::cout	<<	__FUNCTION__	<<		":	Begin	scan	of	allocation	@"	<<	(void*)alloc_info->start	<<	"	for	dependencies."	<<	std::endl;
				}

				reference_attr	=	make_reference_attr(	alloc_info	);

				//	Get	the	dependencies	of	the	allocation
				if	(alloc_info->type	==	TRICK_STRUCTURED)	{
								if	(reference_attr->num_index	>	0)	{
												restore_stls_in_arrayed_class(alloc_info->name,	(char*)(alloc_info->start),	reference_attr,	0,	0)	;
								}	else	{
												restore_stls_in_class(alloc_info->name,	(char*)(alloc_info->start),	alloc_info->attr)	;
								}
				}	else	{
								restore_stls_in_intrinsic(alloc_info->name,	alloc_info->start,	reference_attr,	0,	0	);
				}

				if	(debug_level)	{
								std::cout	<<		__FUNCTION__	<<	":	End	scan	of	allocation	@"	<<	(void*)alloc_info->start	<<	"."	<<	std::endl;
				}

				free_reference_attr(	reference_attr	);

}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::restore_stls_in_class(	std::string	name,	char*	address,	ATTRIBUTES*	attr)	{
				int	ii;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(attr	==	NULL)	{
								std::cerr	<<	"ERROR:	Trick::MemoryManager::restore_stls	called	with	name	=	"	<<	name	<<	"	and	attr	=	NULL."	<<	std::endl;
								return;
				}

				for	(ii	=	0;	attr[ii].name[0]	!=	'\0';	ii++)	{
								if	(currentCheckPointAgent->input_perm_check(&attr[ii]))	{
												char	*elem_addr;
												if	(attr[ii].mods	&	2)	{
																elem_addr	=	(char	*)	attr[ii].offset;
												}	else	{
																elem_addr	=	address	+	attr[ii].offset;
												}

												if	(attr[ii].type	==	TRICK_STRUCTURED)	{
																if	(attr[ii].num_index	!=	0)	{
																				restore_stls_in_arrayed_class(	name	+	"."	+	attr[ii].name,	elem_addr,	&attr[ii],	0,	0);
																}	else	{
																				restore_stls_in_class(	name	+	"."	+	attr[ii].name,	elem_addr,	(ATTRIBUTES*)(attr[ii].attr));
																}
												}	else	if	(attr[ii].type	==	TRICK_STL)	{
																(*attr[ii].restore_stl)(elem_addr,	name.c_str(),	attr[ii].name)	;
												}	else	{
																restore_stls_in_intrinsic(	name	+	"."	+	attr[ii].name	,	elem_addr,	&(attr[ii]),	0,	0);
												}
								}
				}
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::restore_stls_in_arrayed_class(
																																																std::string	name,
																																																char*							address,
																																																ATTRIBUTES*	attr,
																																																int									curr_dim,
																																																int									offset)	{
				int	ii;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(attr	==	NULL)	{
								std::cerr	<<	"Trick::MemoryManager::restore_stls_in_arrayed_class:	ERROR:	attr	=	NULL."	<<	std::endl;
								return;
				}

				if	(attr->index[curr_dim].size	==	0)	{

								void*	pointer	=	*(void**)((char*)address	+	offset	*	sizeof(void*));

								if	(pointer	!=	NULL)	{
												ALLOC_INFO	*alloc_info;
												alloc_info	=	get_alloc_info_of(	pointer);
												if	(alloc_info	!=	NULL)	{

																if	(debug_level)	{
																				std::cout	<<	__FUNCTION__	<<	":	@"	<<	(void*)address	<<	"."	<<	attr->name	<<	"["	<<	offset	<<	"]	points	to	@"	<<	(void*)pointer	<<	std::endl;
																}
																//restore_stls(alloc_info);
												}	else	{
																if	(debug_level)	{
																				std::cout	<<	__FUNCTION__	<<	":	Pointer	("<<	(void*)pointer	<<")	refers	to	memory	that	the	MemoryManager	doesn't	know	about."	<<	std::endl;
																				std::cout.flush();
																}
												}
								}	else	{
												if	(debug_level)	{
																std::cout	<<	__FUNCTION__	<<	":	Allocation	reference	is	NULL."	<<	std::endl;
																std::cout.flush();
												}
								}

				}	else	{
								int	curr_dim_size;
								curr_dim_size	=	attr->index[curr_dim].size;
								for	(ii	=	0;	ii	<	curr_dim_size;	ii++)	{
												char	index[16]	;
												snprintf(index,	sizeof(index),	"[%d]",	ii)	;
												if	(curr_dim	<	attr->num_index	-	1)	{
																restore_stls_in_arrayed_class(	name	+	index,	address,	attr,	curr_dim	+	1,	offset	*	curr_dim_size	+	ii);
												}	else	{
																char*	elem_addr;
																elem_addr	=	address	+	(offset	*	curr_dim	+	ii)	*	attr->size	;
																restore_stls_in_class(	name	+	index,	elem_addr,	(ATTRIBUTES*)attr->attr	);
												}
								}
				}
}


//	MEMBER	FUNCTION
void	Trick::MemoryManager::restore_stls_in_intrinsic(	std::string	name	,	void*	address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				//	If	the	variable	that	we	are	pointing	to	is	an	arrayed
				if	(curr_dim	<	attr->num_index)	{

								//	If	it	is	an	unconstrained	array	(a	pointer)
								if	(attr->index[curr_dim].size	==	0)	{

												void*	pointer	=	*(void**)((char*)address	+	offset	*	sizeof(void*));

												if	(pointer	!=	NULL)	{
																ALLOC_INFO	*alloc_info;
																alloc_info	=	get_alloc_info_of(	pointer);
																if	(alloc_info	!=	NULL)	{
																				if	(debug_level)	{
																								std::cout	<<	__FUNCTION__	<<		":	@"	<<	(void*)address	<<	"."	<<	attr->name	<<	"["	<<	offset	<<	"]	points	to	@"	<<	(void*)pointer	<<	std::endl;
																				}
																				//restore_stls(alloc_info);
																}	else	{
																				if	(debug_level)	{
																								std::cout	<<	__FUNCTION__	<<		":	Pointer	("<<	(void*)pointer	<<")	refers	to	memory	that	the	MemoryManager	doesn't	know	about."	<<	std::endl;
																								std::cout.flush();
																				}
																}
												}	else	{
																if	(debug_level)	{
																				std::cout	<<	__FUNCTION__	<<		":	Allocation	reference	is	NULL."	<<	std::endl;
																				std::cout.flush();
																}
												}

								}	else	{	//	otherwise	its	a	constrained	array.
												int	ii;
												for	(ii=0	;	ii<	attr->index[curr_dim].size	;	ii++)	{
																char	index[16]	;
																snprintf(index,	sizeof(index),	"[%d]",	ii)	;
																//restore_stls_in_intrinsic(	name	+	index	,	address,	attr,	curr_dim	+	1,	offset	*	attr->index[curr_dim].size	+	ii);
												}
								}
				}
}

