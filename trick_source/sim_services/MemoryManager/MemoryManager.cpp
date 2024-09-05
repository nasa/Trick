#include	<dlfcn.h>
#include	<stdlib.h>
#include	"trick/MemoryManager.hh"
#include	"trick/ClassicCheckPointAgent.hh"
//	Global	pointer	to	the	(singleton)	MemoryManager	for	the	C	language	interface.
Trick::MemoryManager	*	trick_MM	=	NULL;

//	CLASS	VARIABLE	INITIALIZATION
int	Trick::MemoryManager::instance_count	=	0;
bool	Trick::MemoryManager::restore_stls_default	=	true;

//	CONSTRUCTOR
Trick::MemoryManager::MemoryManager()
{


				//	The	MemoryManager	is	a	singleton.	That	is	only	one	instance	of	it	is	allowed.
				if	(instance_count	!=	0)	{
								throw	std::logic_error("Only	one	instance	of	the	Memory	Manager	is	allowed.");
				}
				instance_count	++;

				debug_level	=	0;
				hexfloat_checkpoint	=	0;
				reduced_checkpoint		=	1;
				resetting_memory	=	false;
				expanded_arrays		=	0;
				//	start	counter	at	100mil.		This	(hopefully)	ensures	all	alloc'ed	ids	are	after	external	variables.
				alloc_info_map_counter	=	100000000	;
				//	start	counter	at	0.		This	forces	extern	vars	to	appear	in	front	of	actual	allocations	in	checkpoint.
				extern_alloc_info_map_counter	=	0	;
				pthread_mutex_init(&mm_mutex,	NULL);

				defaultCheckPointAgent	=	new	ClassicCheckPointAgent(	this);
				defaultCheckPointAgent->set_reduced_checkpoint(	reduced_checkpoint);
				defaultCheckPointAgent->set_hexfloat_checkpoint(	hexfloat_checkpoint);
				defaultCheckPointAgent->set_debug_level(	debug_level);

				currentCheckPointAgent	=	defaultCheckPointAgent;

				dlhandles.push_back(dlopen(	NULL,	RTLD_LAZY))	;

				local_anon_var_prefix	=	"trick_anon_local_";
				extern_anon_var_prefix	=	"trick_anon_extern_";

				primitive_types.insert(std::string("char"))	;
				primitive_types.insert(std::string("unsigned	char"))	;
				primitive_types.insert(std::string("short"))	;
				primitive_types.insert(std::string("unsigned	short"))	;
				primitive_types.insert(std::string("int"))	;
				primitive_types.insert(std::string("unsigned	int"))	;
				primitive_types.insert(std::string("long"))	;
				primitive_types.insert(std::string("unsigned	long"))	;
				primitive_types.insert(std::string("long	long"))	;
				primitive_types.insert(std::string("unsigned	long	long"))	;
				primitive_types.insert(std::string("double"))	;
				primitive_types.insert(std::string("float"))	;
				primitive_types.insert(std::string("bool"))	;
				//	add	stdint.h	types
				primitive_types.insert(std::string("int8_t"))	;
				primitive_types.insert(std::string("uint8_t"))	;
				primitive_types.insert(std::string("int16_t"))	;
				primitive_types.insert(std::string("uint16_t"))	;
				primitive_types.insert(std::string("int32_t"))	;
				primitive_types.insert(std::string("uint32_t"))	;
				primitive_types.insert(std::string("int64_t"))	;
				primitive_types.insert(std::string("uint64_t"))	;

				trick_MM	=	this;
				return	;
}

Trick::MemoryManager::~MemoryManager()	{

				ALLOC_INFO_MAP::iterator	ait	;

				if	(instance_count	>	0)	{
								instance_count	--;
				}

				delete	defaultCheckPointAgent	;

				for	(	ait	=	alloc_info_map.begin()	;	ait	!=	alloc_info_map.end()	;	++ait	)	{
								ALLOC_INFO	*	ai_ptr	=	(*ait).second	;
								if	(ai_ptr->stcl	==	TRICK_LOCAL)	{
												if	(	ai_ptr->alloc_type	==	TRICK_ALLOC_MALLOC	)	{
																free((char	*)ai_ptr->start	-	ai_ptr->sentinel_bytes)	;
												}	else	if	(	ai_ptr->alloc_type	==	TRICK_ALLOC_NEW	)	{
																io_src_delete_class(	ai_ptr	);
												}
								}
								if	(ai_ptr->name)	{	free(ai_ptr->name);	}
								if	(ai_ptr->user_type_name)	{	free(ai_ptr->user_type_name);	}
								free(ai_ptr)	;
				}
				alloc_info_map.clear()	;
}

#include	<sstream>
void	Trick::MemoryManager::emitMessage(	std::string	message)	{
				std::cerr	<<	"MemoryManager:"	<<	message	<<	std::endl;
				std::cerr.flush();
}

void	Trick::MemoryManager::emitError(	std::string	message)	{
				std::stringstream	ss;
				ss	<<	"ERROR:"	<<	message	<<	std::endl;
				emitMessage(	ss.str()	);
}

void	Trick::MemoryManager::emitWarning(	std::string	message)	{
				std::stringstream	ss;
				ss	<<	"WARNING:"	<<	message	<<	std::endl;
				emitMessage(	ss.str()	);
}

