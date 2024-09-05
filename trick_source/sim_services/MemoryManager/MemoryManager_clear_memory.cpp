#include	"trick/MemoryManager.hh"
#include	"trick/bitfield_proto.h"
#include	<sstream>

void	Trick::MemoryManager::clear_rvalue(	void*	base_address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

				char*	final_address;
				int	remaining_dimensions	=	attr->num_index	-	curr_dim;

				/**	@par	Detailed	Description:	*/

				/**	@par
								If	we're	referencing	a	singleton	then	calculate	the	address
								from	the	(base)	address,	the	offset	and	the	data-type.	Then
								set	the	value	at	that	address	to	nil.
					*/
				if	(remaining_dimensions	==0)	{

								switch	(attr->type)	{
											case	TRICK_CHARACTER	:
											case	TRICK_UNSIGNED_CHARACTER	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(char);
															*(char*)final_address	=	'\0';
															break;
											case	TRICK_BOOLEAN:
															final_address	=	(char*)base_address	+	offset	*	sizeof(bool);
															*(bool*)final_address	=	false;
															break;
											case	TRICK_WCHAR	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(wchar_t);
															*(wchar_t*)final_address	=	0;
															break;
											case	TRICK_SHORT	:
											case	TRICK_UNSIGNED_SHORT	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(short);
															*(short*)final_address	=	0;
															break;
											case	TRICK_INTEGER	:
											case	TRICK_UNSIGNED_INTEGER	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(int);
															*(int*)final_address	=	0;
															break;
											case	TRICK_ENUMERATED	:
															if	((size_t)attr->size	==	sizeof(int))	{
																			final_address	=	(char*)base_address	+	offset	*	sizeof(int);
																			*(int*)final_address	=	0;
															}	else	if	((size_t)attr->size	==	sizeof(short))	{
																			final_address	=	(char*)base_address	+	offset	*	sizeof(short);
																			*(short*)final_address	=	0;
															}	else	{
																			emitError("INTERNAL-ERROR	-	Unexpected	size	of	ENUMERATION	type.")	;
															}
															break;
											case	TRICK_LONG	:
											case	TRICK_UNSIGNED_LONG	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(long);
															*(long*)final_address	=	0;
															break;
											case	TRICK_FLOAT	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(float);
															*(float*)final_address	=	0.0;
															break;
											case	TRICK_DOUBLE	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(double);
															*(double*)final_address	=	0.0;
															break;
											case	TRICK_LONG_LONG	:
											case	TRICK_UNSIGNED_LONG_LONG	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(long	long);
															*(long	long*)final_address	=	0;
															break;
											case	TRICK_BITFIELD	:
											case	TRICK_UNSIGNED_BITFIELD	:
															final_address	=	(char*)base_address	+	offset	*	(size_t)attr->size;
															if	(attr->size	==	sizeof(int))	{
																			*(unsigned	int*)final_address	=	insert_bitfield_any(
																							*(unsigned	int*)final_address,	0,	attr->size,	attr->index[0].start,	attr->index[0].size);
															}	else	if	(attr->size	==	sizeof(short))	{
																			*(unsigned	short*)final_address	=	insert_bitfield_any(
																							*(unsigned	short*)final_address,	0,	attr->size,	attr->index[0].start,	attr->index[0].size);
															}	else	if	(attr->size	==	sizeof(char))	{
																			*(unsigned	char*)final_address	=	insert_bitfield_any(
																							*(unsigned	char*)final_address,	0,	attr->size,	attr->index[0].start,	attr->index[0].size);
															}	else	{
																			std::stringstream	message;
																			message	<<	"INTERNAL	-	Unhandled	bitfield	struct	size	("	<<	attr->size	<<	")	in	bitfield	assignment.";
																			emitError(message.str());
															}
															break;
											case	TRICK_FILE_PTR	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(void*);
															*(void**)final_address	=	(void*)NULL;
															break;
											case	TRICK_STRING	:
															final_address	=	(char*)base_address	+	offset	*	sizeof(std::string);
															*(std::string*)final_address	=	"";
															break;
											case	TRICK_STL	:
															final_address	=	(char*)base_address	+	offset	*	attr->size	;
															if	(	attr->clear_stl	)	{
																			(*attr->clear_stl)(final_address)	;
															}
															break;
											default	:
															std::stringstream	message;
															message	<<	"Unhandled	Type	("	<<	(int)attr->type	<<	").";
															emitError(message.str());
															break;
								}

				/**	@par
								If	on	the	otherhand	we	are	referencing	an	array,	then	we	must	consider	two	cases.
					*/
				}	else	if	(remaining_dimensions	>	0)	{
							int	extent;

							extent	=	attr->index[curr_dim].size	;

							/**		@par
												If	the	array	is	unconstrained	(i.e.,	it's	a	pointer)	then	we	just	need	to	check
												whether	the	pointer	is	NULL.
								*/
							if	(	extent	==	0)	{
											final_address	=	(char*)base_address;
											*(void**)final_address	=	NULL;

							/**	@par
											If	the	array	(at	this	dimension)	is	constrained	(i.e.,	it's	a	fixed	array	)
											then	it	is	nil	if	and	only	if	each	of	it's	sub-elements	(at	the	next	dimension,
											which	can	themselves	be	arrays)	are	nil.	So,	for	each	of	the	elements	in	current
											dimension,	we	recursively	call	is_nil_valued()	on	each	of	the	sub-elements	to
											find	out	whether	this	array	is	nil	valued	and	return	the	result.
											*/
							}	else	{
											int	ii;

											for	(ii=0;	ii	<	extent;	ii++)	{
															clear_rvalue(	base_address,	attr,	curr_dim	+	1,	offset	*	extent	+	ii);
											}
							}

				}	else	{
								std::stringstream	message;
								message	<<	"This	is	bad.	Remaining	dimensions	are	negative!?.";
								emitError(message.str());
				}
}

void	Trick::MemoryManager::clear_class(	char	*address,	ATTRIBUTES	*	A)	{

				int	jj;
				char	*element_addr;

				/**	@par	Design	Details:
					This	function	is	implemented	using	the	following	algorithm:
					*/

				for	(jj	=	0;	A[jj].name[0]	!=	'\0';	jj++)	{
								/*
									*	For	all	the	parameters	in	this	data	structure...
									*/

								if	(currentCheckPointAgent->input_perm_check(&A[jj]))	{

												/**
													Determine	the	address	of	the	structure/class	element.	If	the	element	is	a
													C++	static	variable,	then	the	element's	ATTRIBUTES.offset	contains	the
													it's	address.		Otherwise	the	element's	address	is	the	structure/class
													base	address	+	the	element's	ATTRIBUTES.offset.
													*/
												if	(A[jj].mods	&	2)	{
																element_addr	=	(char	*)	A[jj].offset;
												}	else	{
																element_addr	=	address	+	A[jj].offset;
												}

												if	(A[jj].type	==	TRICK_STRUCTURED)	{
																if	(A[jj].num_index	!=	0)	{
																				clear_arrayed_class(	element_addr,	&A[jj],	0,	0);
																}	else	{
																				if	(	A[jj].attr	!=	NULL	)	{
																								clear_class(	element_addr,	(ATTRIBUTES*)(A[jj].attr));
																				}
																}
												}	else	{
																clear_rvalue(	element_addr,	&(A[jj]),	0,	0);
												}

								}
				}
				return;
}

void	Trick::MemoryManager::clear_arrayed_class(	char*	address,	ATTRIBUTES*	A,	int	curr_dim,	int	offset)	{

				int	ii,	extent;

				/**	@par	Design	Details:
					This	function	is	implemented	using	the	following	algorithm:
					*/

				/**	If	we	are	looking	at	the	pointer	then	clear	it..*/
				if	(A->index[curr_dim].size	==	0)	{
								clear_rvalue(	address,	A,	curr_dim,	offset);
				}	else	{
								extent	=	A->index[curr_dim].size;

								for	(ii	=	0;	ii	<	extent;	ii++)	{

												if	(curr_dim	<	A->num_index	-	1)	{
																clear_arrayed_class(	address,	A,	curr_dim	+	1,	offset	*	extent	+	ii);
												}	else	{
																char*	element_addr;
																element_addr	=	address	+	(offset	*	extent	+	ii)	*	A->size	;
																clear_class(	element_addr,	(ATTRIBUTES*)A->attr);
												}
								}

				}
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::clear_var_critical_section(	void*	address)	{
				ATTRIBUTES*	reference_attr;
				ALLOC_INFO*	alloc_info;
				alloc_info	=	get_alloc_info_at(	address);
				if	(alloc_info	!=	NULL)	{
								reference_attr	=	make_reference_attr(	alloc_info);
								if	(alloc_info->type	==	TRICK_STRUCTURED	)	{
												if	(alloc_info->num_index	!=	0)	{
																clear_arrayed_class(	(char*)(alloc_info->start),	reference_attr,	0,	0)	;
												}	else	{
																clear_class(	(char*)(alloc_info->start),	alloc_info->attr)	;
												}
								}	else	{
												clear_rvalue(	alloc_info->start,	reference_attr,	0,	0	);
								}
								free_reference_attr(	reference_attr);
				}	else	{
								std::stringstream	message;
								message	<<	"Cannot	clear	the	variable	at	address	"	<<	address
																<<	"because	memory	manager	knows	nothing	about	it."	;
								emitError(message.str());
				}
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::clear_var(	void*	address)	{
				pthread_mutex_lock(&mm_mutex);
				clear_var_critical_section(address);
				pthread_mutex_unlock(&mm_mutex);
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::clear_var(	const	char*	name)	{

				VARIABLE_MAP::iterator	pos;
				ALLOC_INFO	*alloc_info;

				pthread_mutex_lock(&mm_mutex);
				pos	=	variable_map.find(	name);

				if	(pos	!=	variable_map.end())	{
								alloc_info	=	pos->second;
								clear_var_critical_section(	alloc_info->start);
				}	else	{
								std::stringstream	message;
								message	<<	"Can't	clear	variable	\""	<<	name
																<<	"\"	because	it	doesn't	exist.";
								emitError(message.str());
				}
				pthread_mutex_unlock(&mm_mutex);
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::clear_all_vars()	{

				ALLOC_INFO_MAP::iterator	pos;
				ALLOC_INFO*	alloc_info;

				pthread_mutex_lock(&mm_mutex);
				for	(pos=alloc_info_map.begin()	;	pos!=alloc_info_map.end()	;	pos++	)	{
								alloc_info	=	pos->second;
								clear_var_critical_section(	alloc_info->start);
				}
				pthread_mutex_unlock(&mm_mutex);
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::reset_memory()	{

				ALLOC_INFO_MAP::iterator	pos;
				ALLOC_INFO*	alloc_info;
				std::vector<void*>	deletion_list;
				int	ii,	n_addrs;

				//	Clear	the	deleted_addr_list	at	the	start	of	reset	memory.	As	delete_var()	is	called,
				//	and	resetting_memory	==	true,	the	deleted	address	will	be	added	to	the	deleted_addr_list.
				resetting_memory	=	true;
				deleted_addr_list.clear();

				pthread_mutex_lock(&mm_mutex);
				for	(pos=alloc_info_map.begin()	;	pos!=alloc_info_map.end()	;	pos++	)	{
								alloc_info	=	pos->second;

								if	(alloc_info->stcl	==	TRICK_LOCAL)	{
												deletion_list.push_back(	alloc_info->start);
								}
				}
				pthread_mutex_unlock(&mm_mutex);

				n_addrs	=	deletion_list.size();
				for	(ii	=	0	;	ii	<	n_addrs	;	ii	++)	{
								delete_var(	deletion_list[ii]);
				}

				resetting_memory	=	false;
				deleted_addr_list.clear();

				//	reset	counter	to	100mil.		This	(hopefully)	ensures	all	alloc'ed	ids	are	after	external	variables.
				alloc_info_map_counter	=	100000000	;
}
