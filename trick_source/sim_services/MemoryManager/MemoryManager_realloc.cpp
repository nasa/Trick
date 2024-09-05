#include	"trick/MemoryManager.hh"
#include	<dlfcn.h>
#include	<stdlib.h>
#include	<sstream>
#include	<string.h>

//	PRIVATE	MEMBER	FUNCTION
void	Trick::MemoryManager::recursive_array_copy(	void	*s_base,
																																																	void	*d_base,
																																																	int	*s_cdims,
																																																	int	*d_cdims,
																																																	size_t	size,
																																																	int	n,
																																																	int	cur,
																																																	int	s_off,
																																																	int	d_off)	{
				char	*s_addr,	*d_addr;
				int	ii;

				if	(cur	==	n)	{
								s_addr	=	(char*)s_base	+	s_off	*	size;
								d_addr	=	(char*)d_base	+	d_off	*	size;
								memcpy(	d_addr,	s_addr,	size);
				}	else	{

								for	(ii=0	;	(ii<s_cdims[cur])&&(ii<d_cdims[cur])	;	ii++	)	{
														recursive_array_copy(	s_base,
																																				d_base,
																																				s_cdims,
																																				d_cdims,
																																				size,
																																				n,
																																				cur+1,
																																				s_off	*	s_cdims[cur]	+	ii,
																																				d_off	*	d_cdims[cur]	+	ii);
								}
				}
}


//	PRIVATE	MEMBER	FUNCTION
void	Trick::MemoryManager::array_copy(	void	*s_base,
																																							void	*d_base,
																																							int	*s_cdims,
																																							int	*d_cdims,
																																							size_t	size,
																																							int	n)	{

			recursive_array_copy(	s_base,	d_base,	s_cdims,	d_cdims,	size,	n,	0,	0,	0);
}


//	PUBLIC	MEMBER	FUNCTION
void*	Trick::MemoryManager::resize_array(	void	*address,	int	n_cdims,	int	*cdims)	{

				ALLOC_INFO	*alloc_info;
				int	ii;
				int	pre_n_cdims;
				int	new_n_elems;
				int	n_stars;
				void	*new_address;

				/**	@li	Find	the	ALLOC_INFO	record	for	the	object	at	the	given	address.*/
				pthread_mutex_lock(&mm_mutex);
				alloc_info	=	get_alloc_info_at(	address);

				if	(alloc_info	==	NULL)	{
								emitError("Address	passed	to	resize_array	is	NULL.")	;
								pthread_mutex_unlock(&mm_mutex);
								return	(	(void*)NULL);
				}

				if	(alloc_info->stcl	!=	TRICK_LOCAL)	{
								std::stringstream	message;
								message	<<	"Cannot	resize	the	array	at	["	<<	address	<<	"]	because	it's	EXTERN.";
								emitError(message.str());
								pthread_mutex_unlock(&mm_mutex);
								return	(	(void*)NULL);
				}

				if	(alloc_info->type	==	TRICK_STRING)	{
								std::stringstream	message;
								message	<<	"resize_array	doesn't	support	STL	strings.";
								emitError(message.str());
								pthread_mutex_unlock(&mm_mutex);
								return	(	(void*)NULL);
				}

				/**	@li	Determine	the	number	of	constrained	dimensions	in	the	existing	allocation.*/
				pre_n_cdims	=	0;
				for	(ii=0	;	ii<alloc_info->num_index	;	ii++)	{
							if	(alloc_info->index[ii]!=0)	{
											pre_n_cdims++;
							}
				}
				n_stars	=	alloc_info->num_index	-	pre_n_cdims;
				pthread_mutex_unlock(&mm_mutex);

				/**	@li	Verify	that	the	number	of	constrained	dimensions	in	the	existing	allocation
								are	the	same	as	those	in	the	proposed	re-allocation.*/
				if	(n_cdims	!=	pre_n_cdims)	{
								if	(pre_n_cdims	==	0)	{
												std::stringstream	message;
												message	<<	"resize_array:	The	object	at	address	["	<<	address
																				<<	"]	is	not	an	array.	If	you	want	to	be	able	to	resize	"
																				<<	"it	then	declare	it	as	an	array	(of	at	least	one).";
												emitError(message.str());
								}	else	{
												std::stringstream	message;
												message	<<	"resize_array:	The	number	of	dimensions	specified,	["	<<	n_cdims
																				<<	"],	doesn't	match	the	number	of	dimensions	of	the	object	at	"
																				<<	address	<<",	["	<<	pre_n_cdims	<<	"].";
												emitError(message.str());
								}
								return	(	(void*)NULL);
				}

				/**	@li	Calculate	the	new	number	of	elements	to	be	allocated.	*/
				new_n_elems	=	1;
				for	(ii	=	0;	ii	<	n_cdims	;	ii++	)	{
								new_n_elems	=	new_n_elems	*	cdims[ii];
				}
				if	(new_n_elems	==	0)	{
								emitError("One	or	more	of	the	constrained	dimensions	is	zero.");
								return	((void*)NULL);
				}

				/**	@li	Allocate	new	memory.	*/
				pthread_mutex_lock(&mm_mutex);
				if	(	(alloc_info->type	==	TRICK_STRUCTURED)	&&
									(alloc_info->language	==	Language_CPP)	&&
									(alloc_info->num_index	==	n_cdims)	)	{

								if	((new_address	=	io_src_allocate_class(	alloc_info->user_type_name,	new_n_elems))	==	NULL)	{
												std::stringstream	message;
												message	<<	"io_src_allocate_class("	<<	alloc_info->user_type_name	<<	","	<<	new_n_elems	<<	")	failed	to	allocate	any	memory.";
												emitError(message.str());
												pthread_mutex_unlock(&mm_mutex);
												return	((void*)NULL);
								}
				}	else	if	((alloc_info->type	==	TRICK_STRING)	&&	(n_stars	==	0	)	)	{
								new_address	=	new	(std::nothrow)	std::string[new_n_elems];
								if	(new_address	==	NULL)	{
												pthread_mutex_unlock(&mm_mutex);
												return	((void*)NULL);
								}
				}	else	{
								if	(	(new_address	=	calloc(	(size_t)new_n_elems,	(size_t)alloc_info->size	)	)	==	NULL)	{
												emitError("Out	of	memory.")	;
												pthread_mutex_unlock(&mm_mutex);
												return	((void*)NULL);
								}
				}

				/**	@li	Copy	the	data	from	the	previous	array	to	the	new	array,	constrained	by	the	common	extents.*/
				array_copy(	alloc_info->start,
																new_address,
																alloc_info->index,
																cdims,
																alloc_info->size,
																n_cdims);

				/**	@li	Delete	the	previous	memory	allocation.	*/
				free(	address);

				//	Remove	the	old	<address,	ALLOC_INFO*>	key-value	pair	from	the	alloc_info_map.
				alloc_info_map.erase(	address);

				/**	@li	Update	the	ALLOC_INFO	record	with	new	start	and	end	addresses,	with
												new	extents	and	with	the	new	number	of	elements.*/
				alloc_info->start	=	new_address;
				alloc_info->end			=	(	(char*)alloc_info->start)	+	(new_n_elems	*	alloc_info->size)	-	1;
				for	(ii=0	;	ii	<	n_cdims	;	ii++)	{
								alloc_info->index[ii]	=	cdims[ii];
				}
				alloc_info->num	=	new_n_elems;

				/**	@li	Insert	the	new	<address,	ALLOC_INFO>	key-value	pair	into	the	alloc_info_map.*/
				alloc_info_map[alloc_info->start]	=	alloc_info;
				pthread_mutex_unlock(&mm_mutex);

				/**	@li	If	debug	is	enabled,	show	what	happened.*/
				if	(debug_level)	{
								int	i;
								std::cout	<<	std::endl;
								std::cout	<<	"Re-allocation:	"	<<	alloc_info->num	<<	"	element(s)	of	type("	;
								std::cout	<<	trickTypeCharString(	alloc_info->type,	alloc_info->user_type_name	)	;
								for	(i=0;i<n_stars;i++)	std::cout	<<	"*";
								std::cout	<<	","	<<	alloc_info->type	<<	"),	size("	<<	alloc_info->size	<<	")	@	addr("	<<	alloc_info->start	<<	")."	;
								std::cout	<<	std::endl	<<	std::endl;
								std::cout.flush();
				}
				return	(new_address);
}


//	PUBLIC	MEMBER	FUNCTION
void*	Trick::MemoryManager::resize_array(	void*	address,	int	num)	{
				int	dimension	=	num;

				return(	resize_array(	address,	1,	&dimension));
}


//	PUBLIC	MEMBER	FUNCTION
void*	Trick::MemoryManager::resize_array(	const	char*	name,	int	n_cdims,	int	*cdims)	{

				VARIABLE_MAP::iterator	pos;
				ALLOC_INFO	*alloc_info;

				pthread_mutex_lock(&mm_mutex);
				pos	=	variable_map.find(	name);

				if	(pos	!=	variable_map.end())	{
								alloc_info	=	pos->second;
								pthread_mutex_unlock(&mm_mutex);
								return(	resize_array(	alloc_info->start,	n_cdims,	cdims));
				}	else	{
								std::stringstream	message;
								message	<<	"Cannot	resize	variable	\""	<<	name	<<	"\"	because	it	doesn't	exist.";
								emitError(message.str());
				}
				pthread_mutex_unlock(&mm_mutex);
				return	NULL	;
}


//	PUBLIC	MEMBER	FUNCTION
void*	Trick::MemoryManager::resize_array(	const	char*	name,	int	num)	{
				int	dimension	=	num;

				return(	resize_array(	name,	1,	&dimension));
}
