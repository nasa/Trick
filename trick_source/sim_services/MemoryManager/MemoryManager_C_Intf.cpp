#include	<string>
#include	<iostream>
#include	"trick/memorymanager_c_intf.h"
#include	"trick/MemoryManager.hh"

/*	Global	singleton	pointer	to	the	memory	manager.	*/
extern	Trick::MemoryManager*	trick_MM;

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	6	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	and	return	value	are	the	same	except	that	std::string	args	are	replaced	with	const	char	*	args.
	*/
extern	"C"	void*	TMM_declare_var(	TRICK_TYPE	type,	const	char*class_name,	int	n_stars,	const	char*	var_name,	int	n_cdims,	int	*cdims)	{
				if	(trick_MM	!=	NULL)	{
								std::string	sclass_name	=	class_name;
								std::string	svar_name	=	var_name;
								return	(	trick_MM->declare_var(	type,	sclass_name,	n_stars,	svar_name,	n_cdims,	cdims));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_var()	called	before	MemoryManager	instantiation.	Returning	NULL.")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	2	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	and	return	value	are	the	same.
	*/
extern	"C"	void*	TMM_declare_var_1d(	const	char*	enh_type_spec,	int	e_elems)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->declare_var(	enh_type_spec,	e_elems));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_var_1d()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	1	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	and	return	value	are	the	same.
	*/
extern	"C"	void*	TMM_declare_var_s(	const	char*	declaration)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->declare_var(	declaration));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_var_s()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	a	C	Language	interface	to	the	2	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	are	the	same	but	switched	in	order	and	the	return	value	is	the	same.
	It's	here	because	I	caved	to	Alex's	whine	about	backwards	compatibility.

	*/
extern	"C"	void*	alloc_type(	int	e_elems,	const	char*	enh_type_spec)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->declare_var(	enh_type_spec,	e_elems));
				}	else	{
								Trick::MemoryManager::emitError("alloc_type()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	6	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	and	return	value	are	the	same	except	that	std::string	args	are	replaced	with	const	char	*	args.
	*/
extern	"C"	void*	TMM_declare_operatornew_var(	const	char	*	class_name,	unsigned	int	alloc_size	,	unsigned	int	element_size	)	{
				if	(trick_MM	!=	NULL)	{
								return	trick_MM->declare_operatornew_var(	std::string(class_name),	alloc_size,	element_size	)	;
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_var()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(void*)NULL	;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	7	argument	version	of	Trick::MemoryManager::declare_ext_var.
	The	arguments	and	return	value	are	the	same	except	that	std::string	args	are	replaced	with	const	char	*	args.
	*/
extern	"C"	void*	TMM_declare_ext_var(	void*	addr,	TRICK_TYPE	type,	const	char*class_name,	int	n_stars,	const	char*	var_name,	int	n_cdims,	int	*cdims)	{
				if	(trick_MM	!=	NULL)	{
								std::string	sclass_name	=	class_name;
								std::string	svar_name	=	var_name;
								return	(	trick_MM->declare_extern_var(	addr,	type,	sclass_name,	n_stars,	svar_name,	n_cdims,	cdims));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_ext_var()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	4	argument	version	of	Trick::MemoryManager::declare_var.
	The	arguments	and	return	value	are	the	same.
	*/
extern	"C"	void*	TMM_declare_ext_var_1d(	void*	addr,	const	char*	elem_decl,	int	n_elems)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->declare_extern_var(	addr,	elem_decl,	n_elems));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_ext_var_1d()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	the	1	argument	version	of	Trick::MemoryManager::declare_extern_var.
	The	arguments	and	return	value	are	the	same.
	*/
extern	"C"	void*	TMM_declare_ext_var_s(	void*	addr,	const	char*	declaration)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->declare_extern_var(addr,	declaration));
				}	else	{
								Trick::MemoryManager::emitError("TMM_declare_ext_var_s()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language,	"identify	the	array	by	address",	version	of	Trick::MemoryManager::resize_array.
	Resize	each	of	the	dimensions	of	the	array	at	the	given	address.	n_cdims	specifies	the	number
	of	dimensions	and	must	match	those	of	the	array	being	resized.	cdims	specifies	the	new	sizes
	for	each	of	the	dimensions.
	*/
void*	TMM_resize_array_a(void	*address,	int	n_cdims,	int	*cdims){
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->resize_array(address,	n_cdims,	cdims));
				}	else	{
								Trick::MemoryManager::emitError("TMM_resize_array_a()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language,	"identify	the	array	by	name",	version	of	Trick::MemoryManager::resize_array.
	Resize	each	of	the	dimensions	of	the	array	whose	name	is	given.	n_cdims	specifies	the	number
	of	dimensions	and	must	match	those	of	the	array	being	resized.	cdims	specifies	the	new	sizes
	for	each	of	the	dimensions.
	*/
void*	TMM_resize_array_n(const	char	*name,	int	n_cdims,	int	*cdims){
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->resize_array(name,	n_cdims,	cdims));
				}	else	{
								Trick::MemoryManager::emitError("TMM_resize_array_n()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::resize_array(address,	num).
	Resize	the	one	dimensional	array	at	the	given	address.
	*/
void*	TMM_resize_array_1d_a(void*	address,	int	num){
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->resize_array(address,	num));
				}	else	{
								Trick::MemoryManager::emitError("TMM_resize_array_1d_a()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::resize_array(name,	num).
	Resize	the	one	dimensional	array	whose	name	is	given.
	*/
void*	TMM_resize_array_1d_n(const	char	*name,	int	num){
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->resize_array(name,	num));
				}	else	{
								Trick::MemoryManager::emitError("TMM_resize_array_1d_n()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::mm_strdup(	str).
	Duplicate	and	catalog	the	given	character	string.
	*/
extern	"C"	char*	TMM_strdup(char	*str)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->mm_strdup(	str));
				}	else	{
								Trick::MemoryManager::emitError("TMM_strdup	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return(	(char*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::var_exists().
	Test	whether	the	given	variable_name	is	in	use.
	*/
extern	"C"	int	TMM_var_exists(	const	char*	var_name)	{
				if	(trick_MM	!=	NULL)	{
								std::string	svar_name	=	var_name;
								return	(	trick_MM->var_exists(	svar_name));
				}	else	{
								Trick::MemoryManager::emitError("TMM_var_exists()	called	before	MemoryManager	instantiation.\n")	;
								return	(0);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::var_exists().
	Test	whether	the	given	address	is	in	Trick	managed	memory.		C	interface	to	Trick::MemoryManager::is_alloced(	addr).
	*/
extern	"C"	int	TMM_is_alloced(char	*addr)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->is_alloced(	addr));
				}	else	{
								Trick::MemoryManager::emitError("TMM_is_alloced()	called	before	MemoryManager	instantiation.\n")	;
								return	(0);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::set_debug_level(level).
	Set	the	verbosity	of	MemoryManager	debug	messages.		C	interface	to	Trick::MemoryManager::set_debug_level(	level).
	*/
extern	"C"	void	TMM_set_debug_level(int	level)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->set_debug_level(	level);
				}	else	{
								Trick::MemoryManager::emitError("TMM_set_debug_level()	called	before	MemoryManager	instantiation.\n")	;
				}
}

/**
	This	is	the	C	Language	version	of	Trick::MemoryManager::set_reduced_checkpoint(	yesno).
	@relates	Trick::MemoryManager
	*/
extern	"C"	void	TMM_reduced_checkpoint(int	yesno)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->set_reduced_checkpoint(	yesno!=0	);
				}	else	{
								Trick::MemoryManager::emitError("TMM_reduced_checkpoint()	called	before	MemoryManager	instantiation.\n")	;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::set_hexfloat_checkpoint(	yesno).
	*/
extern	"C"	void	TMM_hexfloat_checkpoint(int	yesno)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->set_hexfloat_checkpoint(	yesno!=0	);
				}	else	{
								Trick::MemoryManager::emitError("TMM_hexfloat_checkpoint()	called	before	MemoryManager	instantiation.\n")	;
				}
}




/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::clear_var(	addr).
	*/
extern	"C"	void	TMM_clear_var_a(void	*addr)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->clear_var(	addr);
				}	else	{
								Trick::MemoryManager::emitError("TMM_clear_var_a()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::clear_var(	name).
	*/
extern	"C"	void	TMM_clear_var_n(	const	char*	name)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->clear_var(	name);
				}	else	{
								Trick::MemoryManager::emitError("TMM_clear_var_n()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::delete_var(	addr).
	*/
extern	"C"	void	TMM_delete_var_a(void	*addr)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->delete_var(	addr);
				}	else	{
								Trick::MemoryManager::emitError("TMM_delete_var_a()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::delete_var(	name).
	*/
extern	"C"	void	TMM_delete_var_n(	const	char*	name)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->delete_var(	name);
				}	else	{
								Trick::MemoryManager::emitError("TMM_delete_var_n()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::delete_extern_var(	addr).
	*/
extern	"C"	void	TMM_delete_extern_var_a(void	*addr)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->delete_extern_var(	addr);
				}	else	{
								Trick::MemoryManager::emitError("TMM_delete_extern_var_a()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::delete_extern_var(	name).
	*/
extern	"C"	void	TMM_delete_extern_var_n(	const	char*	name)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->delete_extern_var(	name);
				}	else	{
								Trick::MemoryManager::emitError("TMM_delete_extern_var_n()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::write_checkpoint(	filename).
	*/
extern	"C"	void	TMM_write_checkpoint(const	char*	filename)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->write_checkpoint(	filename));
				}	else	{
								Trick::MemoryManager::emitError("TMM_write_checkpoint_fn()	called	before	MemoryManager	instantiation.\n")	;
								return;
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::read_checkpoint(	filename).
	*/
extern	"C"	int	TMM_read_checkpoint(const	char*	filename)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->read_checkpoint(	filename	));
				}	else	{
								Trick::MemoryManager::emitError("TMM_read_checkpoint()	called	before	MemoryManager	instantiation.\n")	;
								return(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::read_checkpoint_from_string(	str).
	*/
extern	"C"	int	TMM_read_checkpoint_from_string(const	char*	str)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->read_checkpoint_from_string(	str	));
				}	else	{
								Trick::MemoryManager::emitError("TMM_read_checkpoint_from_string()	called	before	MemoryManager	instantiation.\n")	;
								return(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::init_from_checkpoint(	filename).
	*/
extern	"C"	int	TMM_init_from_checkpoint(const	char*	filename)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->init_from_checkpoint(	filename	));
				}	else	{
								Trick::MemoryManager::emitError("TMM_init_from_checkpoint()	called	before	MemoryManager	instantiation.\n")	;
								return(1);
				}
}

extern	"C"	int	TMM_set_stl_restore	(int	on_off)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->set_restore_stls_default(	(bool)	on_off	));
				}	else	{
								Trick::MemoryManager::emitError("TMM_set_stl_restore()	called	before	MemoryManager	instantiation.\n")	;
								return(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::add_shared_library_symbols(	filename).
	*/
extern	"C"	int	TMM_add_shared_library_symbols(const	char*	filename)	{
				if	(trick_MM	!=	NULL)	{
								return	(	trick_MM->add_shared_library_symbols(	filename));
				}	else	{
								Trick::MemoryManager::emitError("TMM_add_shared_library_symbols()	called	before	MemoryManager	instantiation.\n")	;
								return(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::add_var(	type,	stype,	var_declare,	units).
	*/
extern	"C"	void*	add_var(	TRICK_TYPE	type,	const	char*	stype,	VAR_DECLARE*	var_declare,	char*	units)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->add_var(type,	stype,	var_declare,	units	));
				}	else	{
								Trick::MemoryManager::emitError("add_var()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	((void*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::add_vars(	type,	stype,	var_list,	units).
	*/
extern	"C"	int	add_vars(	TRICK_TYPE	type,	const	char*	stype,	VAR_LIST*	var_list,	char*	units)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->add_vars(type,	stype,	var_list,	units	));
				}	else	{
								Trick::MemoryManager::emitError("add_vars()	called	before	MemoryManager	instantiation.\n")	;
								return	(1);
				}
}

/**
	@relates	Trick::MemoryManager::ref_allocate
	This	is	the	C	Language	version	of	Trick::MemoryManager::ref_allocate(	R,	num).
	*/
extern	"C"	int	ref_allocate(REF2	*R,	int	num)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->ref_allocate(	R,	num));
				}	else	{
								Trick::MemoryManager::emitError("ref_allocate()	called	before	MemoryManager	instantiation.\n")	;
								return	(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::ref_attributes(	name).
	*/
extern	"C"	REF2*	ref_attributes(const	char*	name)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->ref_attributes(	name));
				}	else	{
								Trick::MemoryManager::emitError("ref_attributes()	called	before	MemoryManager	instantiation.	Returning	NULL.\n")	;
								return	(	(REF2*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::ref_assignment(	REF2*,	V_TREE*).
	*/
extern	"C"	int	ref_assignment(REF2*	R,	V_TREE*	V)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->ref_assignment(	R,	V));
				}	else	{
								Trick::MemoryManager::emitError("ref_assignment()	called	before	MemoryManager	instantiation.\n")	;
								return	(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::ref_var(	REF2*,	name).
	*/
extern	"C"	int	ref_var(REF2	*R,	char*	name)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->ref_var(	R,	name));
				}	else	{
								Trick::MemoryManager::emitError("ref_var()	called	before	MemoryManager	instantiation.\n")	;
								return	(1);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::ref_var(	REF2*,	name).
	*/
extern	"C"	int	get_size(void	*addr)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->get_size(	addr));
				}	else	{
								Trick::MemoryManager::emitError("get_size()	called	before	MemoryManager	instantiation.\n")	;
								return	(0);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::get_truncated_size(	addr).
	*/
extern	"C"	int	get_truncated_size(void	*addr)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->get_truncated_size(	addr));
				}	else	{
								Trick::MemoryManager::emitError("get_truncated_size()	called	before	MemoryManager	instantiation.\n")	;
								return	(0);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::get_truncated_size(	ptr).
	*/
extern	"C"	int	io_get_fixed_truncated_size(char	*ptr	__attribute__	((unused)),
																																											ATTRIBUTES	*	A	__attribute__	((unused)),
																																											char	*str	__attribute__	((unused)),
																																											int	dims	__attribute__	((unused)),
																																											ATTRIBUTES	*	left_type	__attribute__	((unused)))	{
				Trick::MemoryManager::emitError("io_get_fixed_truncated_size()	is	not	implemented	yet.\n")	;
				return(0);
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::get_alloc_info_of(	addr).
	*/
extern	"C"	ALLOC_INFO*	get_alloc_info_of(void	*	addr)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->get_alloc_info_of(	addr));
				}	else	{
								Trick::MemoryManager::emitError("get_alloc_info_of()	called	before	MemoryManager	instantiation.\n")	;
								return	(	(ALLOC_INFO*)NULL);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::get_alloc_info_at(	addr).
	*/
extern	"C"	ALLOC_INFO*	get_alloc_info_at(void	*	addr)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->get_alloc_info_at(	addr));
				}	else	{
								Trick::MemoryManager::emitError("get_alloc_info_at()	called	before	MemoryManager	instantiation.\n")	;
								return	(	(ALLOC_INFO*)NULL);
				}
}

extern	"C"	int	set_alloc_name_at(void	*	addr,	const	char	*	name	)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->set_name_at(addr,	name));
				}	else	{
								Trick::MemoryManager::emitError("get_alloc_info_at()	called	before	MemoryManager	instantiation.\n")	;
								return	(	-1	);
				}
}

/**
	@relates	Trick::MemoryManager
	This	is	the	C	Language	version	of	Trick::MemoryManager::get_enumerated(const	char*	name,	V_DATA*	v_data).
	*/
extern	"C"	int	get_enumerated(const	char*	name,	V_DATA*	v_data)	{
				if	(trick_MM	!=	NULL)	{
								return(	trick_MM->get_enumerated(name,	v_data	));
				}	else	{
								Trick::MemoryManager::emitError("get_enumerated()	called	before	MemoryManager	instantiation.\n")	;
								return	1	;
				}
}

extern	"C"	void	TMM_add_checkpoint_alloc_dependency(const	char	*	name)	{
				if	(trick_MM	!=	NULL)	{
								trick_MM->add_checkpoint_alloc_dependency(name)	;
				}
				return	;
}

