#include	"trick/MemoryManager.hh"
#include	"trick/attributes.h"

//	MEMBER	FUNCTION
void	Trick::MemoryManager::get_alloc_deps_in_allocation(ALLOC_INFO*	alloc_info	)	{

				ATTRIBUTES*	reference_attr;

				bool	found	=	false;
				int	n_depends	=	dependencies.size();

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(alloc_info	==	NULL)	{
								std::cerr	<<	"ERROR:	Trick::MemoryManager::get_alloc_deps_in_allocation	called	with	alloc_info	==	NULL."	<<	std::endl;
								std::cout.flush();
								return;
				}

				if	(debug_level	>	1)	{
								debug_write_alloc_info(alloc_info);
				}

				if	(debug_level)	{
								std::cout	<<	__FUNCTION__	<<	":	Allocation	@"	<<	(void*)alloc_info->start	;
								if	(alloc_info->name	!=	NULL)	{
												std::cout	<<	"	is	named	\""	<<	alloc_info->name	<<	"\"."	<<	std::endl;
								}	else	{
												std::cout	<<	"	is	anonymous."	<<	std::endl;
								}
				}

				std::cout.flush();

				for	(int	ii	=	0	;	ii	<	n_depends	;	ii	++)	{
								if	(alloc_info	==	dependencies[ii])	{
												found	=	true;
												if	(debug_level)	{
																std::cout	<<	__FUNCTION__	<<	":	Dependencies	of	allocation	@"<<	(void*)alloc_info->start	<<"	are	already	known."	<<	std::endl;
												}
												return;
								}
				}

				if	(!found)	{

								if	(debug_level)	{
												std::cout	<<	__FUNCTION__	<<		":	Begin	scan	of	allocation	@"	<<	(void*)alloc_info->start	<<	"	for	dependencies."	<<	std::endl;
								}

								//	The	allocation	is	dependent	upon	itself.
								dependencies.push_back(alloc_info);

								reference_attr	=	make_reference_attr(	alloc_info);

								//	Get	the	dependencies	of	the	allocation
								if	(alloc_info->type	==	TRICK_STRUCTURED)	{
												if	(reference_attr->num_index	>	0)	{
																get_alloc_deps_in_arrayed_class(	(char*)(alloc_info->start),	reference_attr,	0,	0)	;
												}	else	{
																get_alloc_deps_in_class(	(char*)(alloc_info->start),	alloc_info->attr)	;
												}
								}	else	{
												get_alloc_deps_in_intrinsic(	alloc_info->start,	reference_attr,	0,	0	);
								}

								if	(debug_level)	{
												std::cout	<<		__FUNCTION__	<<	":	End	scan	of	allocation	@"	<<	(void*)alloc_info->start	<<	"."	<<	std::endl;
								}

								free_reference_attr(	reference_attr);
				}

}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::get_alloc_deps_in_allocation(	const	char*	var_name	)	{

				VARIABLE_MAP::iterator	pos;
				ALLOC_INFO	*alloc_info;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				pos	=	variable_map.find(	var_name);

				if	(pos	!=	variable_map.end())	{
								alloc_info	=	pos->second;
								get_alloc_deps_in_allocation(	alloc_info	);
				}	else	{
								std::cerr	<<	"ERROR:	get_alloc_deps_in_allocation(\""<<	var_name
																		<<"\")	failed	because	the	given	name	is'nt	known	by	the	MemoryManager."
																		<<	std::endl;
								std::cerr.flush();
				}
}


//	MEMBER	FUNCTION
void	Trick::MemoryManager::get_alloc_deps_in_class(	char*	address,	ATTRIBUTES*	attr)	{
				int	ii;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(attr	==	NULL)	{
								std::cerr	<<	"ERROR:	Trick::MemoryManager::get_alloc_deps_in_class	called	with	attr	=	NULL."	<<	std::endl;
								return;
				}

				for	(ii	=	0;	attr[ii].name[0]	!=	'\0';	ii++)	{
								if	(currentCheckPointAgent->output_perm_check(&attr[ii]))	{
												char	*elem_addr;
												if	(attr[ii].mods	&	2)	{
																elem_addr	=	(char	*)	attr[ii].offset;
												}	else	{
																elem_addr	=	address	+	attr[ii].offset;
												}

												if	(attr[ii].type	==	TRICK_STRUCTURED)	{
																if	(attr[ii].num_index	!=	0)	{
																				get_alloc_deps_in_arrayed_class(	elem_addr,	&attr[ii],	0,	0);
																}	else	{
																				get_alloc_deps_in_class(	elem_addr,	(ATTRIBUTES*)(attr[ii].attr));
																}
												}	else	{
																get_alloc_deps_in_intrinsic(	elem_addr,	&(attr[ii]),	0,	0);
												}
								}
				}
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::get_alloc_deps_in_arrayed_class(
																																																char*							address,
																																																ATTRIBUTES*	attr,
																																																int									curr_dim,
																																																int									offset)	{
				int	ii;

				if	(debug_level	>	1)	{
										std::cout	<<	"DEBUG:	Entered	function:"	<<		__FUNCTION__	<<	std::endl;
				}

				if	(attr	==	NULL)	{
								std::cerr	<<	"Trick::MemoryManager::get_alloc_deps_in_arrayed_class:	ERROR:	attr	=	NULL."	<<	std::endl;
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
																get_alloc_deps_in_allocation(alloc_info);
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
												if	(curr_dim	<	attr->num_index	-	1)	{
																get_alloc_deps_in_arrayed_class(	address,	attr,	curr_dim	+	1,	offset	*	curr_dim_size	+	ii);
												}	else	{
																char*	elem_addr;
																elem_addr	=	address	+	(offset	*	curr_dim	+	ii)	*	attr->size	;
																get_alloc_deps_in_class(	elem_addr,	(ATTRIBUTES*)attr->attr	);
												}
								}
				}
}


//	MEMBER	FUNCTION
void	Trick::MemoryManager::get_alloc_deps_in_intrinsic(	void*	address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

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
																				get_alloc_deps_in_allocation(alloc_info);
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
																get_alloc_deps_in_intrinsic(	address,	attr,	curr_dim	+	1,	offset	*	attr->index[curr_dim].size	+	ii);
												}
								}
				}
}

