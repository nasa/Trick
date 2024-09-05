
#include	<sstream>
//	Provides	stringstream.
#include	<dlfcn.h>
//	Provides	dlsym().
#include	<string.h>
#include	"trick/MemoryManager.hh"

//	MEMBER	FUNCTION:	void*	Trick::MemoryManager::io_src_allocate_class(const	char*	class_name,	int	num);

/**
	*	For	every	class	processed	by	ICG	(Interface	Code	Generator),
	*	the	generated	io_src	code	contains	a	function	whose	name	is
	*	of	the	form:	io_src_allocate_<class_name>.
	*
	*	The	purpose	of	this	io_src	function	is	to	allocate	one	or	more	instances	of	the	class.
	*	The	purpose	of	Trick::MemoryManager::io_src_allocate_class()	is	to	call	the	io_src	function.
	*	-	This	requires	that	the	io_src	code	be	linked	into	the	executable.
	*	-	Then	given	the	name	of	the	class,	we	can	prepend	the	string	"io_src_allocate_"
	*	to	form	the	name	of	the	function.
	*	-	Then	we	look	up	the	address	of	the	function	by	name,	using	dlsym().
	*	-	If	we	find	it,	then	we	call	it	and	return	the	address	of	the	allocation.
	*	-	Otherwise	we	whine	that	we	couldn't	find	it	and	return	NULL.
	*/
void*	Trick::MemoryManager::io_src_allocate_class(const	char*	class_name,	int	num)	{

				char	alloc_fn_name[512];
				void*	(*construct)(int)	=	NULL	;
				void*	addr	=	NULL;
				unsigned	int	ii;

				alloc_fn_name[0]	=	'\0';
				strcat(alloc_fn_name,	"io_src_allocate_");
				strcat(alloc_fn_name,	class_name);

				for	(ii	=	0;	ii	<	strlen(alloc_fn_name);	ii++)	{
								if	(	alloc_fn_name[ii]	==	':')	{
												alloc_fn_name[ii]	=	'_';
								}
				}

				for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	construct	==	NULL	;	ii++	)	{
								construct	=	(void*(*)(int))	dlsym(	dlhandles[ii],	alloc_fn_name);
				}
				if	(	construct	!=	NULL)	{
								addr	=	(*construct)(num)	;
				}	else	{
								const	char*	msg	=	dlerror();
								addr	=	NULL;
								std::stringstream	message;
								message	<<	"Couldn't	find	function	\""	<<	alloc_fn_name	<<	"()\".	"
																<<	"dlerror=	"	<<	msg;
								emitError(message.str());
				}
				return	(addr);
}

//	MEMBER	FUNCTION:	void	Trick::MemoryManager::io_src_destruct_class(ALLOC_INFO	*	alloc_info);
void	Trick::MemoryManager::io_src_destruct_class(ALLOC_INFO	*	alloc_info)	{

				char	destruct_fn_name[512];
				void*	(*destruct)(void*,	int)	=	NULL	;

				if	(	(alloc_info->type	==	TRICK_STRUCTURED)	&&
								(alloc_info->language	==	Language_CPP)	&&
								((alloc_info->num_index	==	0)	||	(alloc_info->index[alloc_info->num_index-1]	!=	0)))	{

								destruct_fn_name[0]	=	'\0';
								strcat(destruct_fn_name,	"io_src_destruct_");
								strcat(destruct_fn_name,	alloc_info->user_type_name	);

								for	(size_t	ii	=	0;	ii	<	strlen(destruct_fn_name);	ii++)	{
												if	(	destruct_fn_name[ii]	==	':')	{
																destruct_fn_name[ii]	=	'_';
												}
								}

								for	(size_t		ii	=	0	;	ii	<	dlhandles.size()	&&	destruct	==	NULL	;	ii++	)	{
												destruct	=	(void*(*)(void*,int))	dlsym(	dlhandles[ii],	destruct_fn_name);
								}
								if	(	destruct	!=	NULL)	{
												(*destruct)(alloc_info->start,	alloc_info->num)	;
								}	else	{
												const	char*	msg	=	dlerror();
												std::stringstream	message;
												message	<<	"Couldn't	find	function	\""	<<	destruct_fn_name	<<	"()\".	"
																				<<	"dlerror=	"	<<	msg;
												emitError(message.str());
								}
				}
}

//	MEMBER	FUNCTION:	void	Trick::MemoryManager::io_src_delete_class(ALLOC_INFO	*	alloc_info);
void	Trick::MemoryManager::io_src_delete_class(ALLOC_INFO	*	alloc_info)	{
				char	delete_fn_name[512];
				void*	(*delete_fn)(void*)	=	NULL	;

				if	(	(alloc_info->type	==	TRICK_STRUCTURED)	&&
								(alloc_info->language	==	Language_CPP)	&&
								((alloc_info->num_index	==	0)	||	(alloc_info->index[alloc_info->num_index-1]	!=	0)))	{

								delete_fn_name[0]	=	'\0';
								strcat(delete_fn_name,	"io_src_delete_");
								strcat(delete_fn_name,	alloc_info->user_type_name	);

								for	(size_t	ii	=	0;	ii	<	strlen(delete_fn_name);	ii++)	{
												if	(	delete_fn_name[ii]	==	':')	{
																delete_fn_name[ii]	=	'_';
												}
								}

								for	(size_t		ii	=	0	;	ii	<	dlhandles.size()	&&	delete_fn	==	NULL	;	ii++	)	{
												delete_fn	=	(void*(*)(void*))	dlsym(	dlhandles[ii],	delete_fn_name);
								}
								if	(	delete_fn	!=	NULL)	{
												(*delete_fn)(alloc_info->start)	;
								}	else	{
												const	char*	msg	=	dlerror();
												std::stringstream	message;
												message	<<	"Couldn't	find	function	\""	<<	delete_fn_name	<<	"()\".	"
																				<<	"dlerror=	"	<<	msg;
												emitError(message.str());
								}
				}
}

//	MEMBER	FUNCTION:	size_t	Trick::MemoryManager::io_src_sizeof_user_type(	const	char*	user_type_name);
/**
	*	There	should	be	a	function	whose	name	is	of	the	form:
	*						io_src_sizeof_<user_type>
	*	linked	into	the	simulation	executable.	We	need	to	find
	*	it	and	call	it.
	*/
size_t	Trick::MemoryManager::io_src_sizeof_user_type(	const	char*	user_type_name)	{

				char	size_fn_name[512];
				size_t	(*size_fn)(void)	=	NULL	;
				size_t	class_size	=	0;
				unsigned	int	ii;

				size_fn_name[0]	=	'\0';
				strcat(size_fn_name,	"io_src_sizeof_");
				strcat(size_fn_name,	user_type_name);

				for	(ii	=	0;	ii	<	strlen(size_fn_name);	ii++)	{
								if	(	size_fn_name[ii]	==	':')	{
												size_fn_name[ii]	=	'_';
								}
				}

				for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	size_fn	==	NULL	;	ii++	)	{
								size_fn	=	(size_t	(*)(void))	dlsym(	dlhandles[ii],	size_fn_name);
				}
				if	(	size_fn	!=	NULL)	{
								class_size	=	(*size_fn)()	;
				}	else	{
								const	char*	msg	=	dlerror();
								class_size	=	0;
								std::stringstream	message;
								message	<<	"Couldn't	find	function	\""	<<	size_fn_name	<<	"()\".	"
																<<	"dlerror=	"	<<	msg;
								emitError(message.str());
				}
				return	(class_size);
}

//	PRIVATE	MEMBER	FUNCTION
void	Trick::MemoryManager::debug_write_alloc_info(	ALLOC_INFO	*alloc_info)	{

				int	ii;
				std::cout	<<	"alloc_info->start	=	"	<<	alloc_info->start	<<	std::endl;
				std::cout	<<	"alloc_info->end			=	"	<<	alloc_info->end	<<	std::endl;
				std::cout	<<	"alloc_info->num			=	"	<<	alloc_info->num	<<	std::endl;
				std::cout	<<	"alloc_info->size		=	"	<<	alloc_info->size	<<	std::endl;
				std::cout	<<	"alloc_info->type		=	"	<<	alloc_info->type	<<	std::endl;
				std::cout	<<	"alloc_info->stcl	=	"	;
				if	(alloc_info->stcl	==	TRICK_LOCAL)	{	std::cout	<<	"TRICK_LOCAL"	<<	std::endl;	}
				if	(alloc_info->stcl	==	TRICK_EXTERN)	{	std::cout	<<	"TRICK_EXTERN"	<<	std::endl;	}
				std::cout	<<	"alloc_info->language	=	";
				if	(alloc_info->language	==	Language_C		)	{	std::cout	<<	"Language_C"	<<	std::endl;	}
				if	(alloc_info->language	==	Language_CPP)	{	std::cout	<<	"Language_CPP"	<<	std::endl;	}
				std::cout	<<	std::endl;
				if	(alloc_info->user_type_name)	{
								std::cout	<<	"alloc_info->user_type_name	=	"	<<	alloc_info->user_type_name	<<	std::endl;
				}
				std::cout	<<	"alloc_info->index	=	"	;
				for	(ii=0;	ii<alloc_info->num_index;	ii++)	{
								std::cout	<<	"["	<<	alloc_info->index[ii]	<<	"]";
				}
				std::cout	<<	std::endl;
				std::cout.flush();
}

