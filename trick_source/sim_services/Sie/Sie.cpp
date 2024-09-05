
#include	<iostream>
#include	<algorithm>
#include	<fstream>
#include	<string.h>
#include	<cstring>

#include	"trick/Sie.hh"
#include	"trick/SimObject.hh"
#include	"trick/attributes.h"
#include	"trick/reference.h"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.hh"
#include	"trick/command_line_protos.h"
#include	"trick/MemoryManager.hh"
#include	"trick/parameter_types.h"

Trick::Sie	*	the_sie	=	NULL	;

Trick::Sie::Sie()	{
				//	Call	the	attribute_map	function	which	will	instantiate	the	singleton	maps
				class_attr_map	=	Trick::AttributesMap::attributes_map()	;
				enum_attr_map	=	Trick::EnumAttributesMap::attributes_map()	;

				move_runtime_generation	=	false;

				the_sie	=	this	;
}

void	copy_file	(const	std::string&	original_filename,	const	std::string&	copy_filename)	{
				std::ifstream	original;
				std::ofstream	copy;

				original.open(original_filename.c_str(),	std::ios::binary);
				copy.open(copy_filename.c_str(),	std::ios::binary);

				copy	<<	original.rdbuf();

				original.close();
				copy.close();
}

//	Helper	function	for	copying	S_sie.resource	from	default	dir	to	output	dir
void	copy_sie_resource()	{
				std::string	original_sie_filename	=	std::string(command_line_args_get_default_dir())	+	"/"	+	"S_sie.resource";
				std::string	copy_sie_filename	=	std::string(command_line_args_get_output_dir())	+	"/"	+	"S_sie.resource";
				copy_file(original_sie_filename,	copy_sie_filename);
}

int	Trick::Sie::process_sim_args()	{

				int	argc	;
				char	**	argv	;

				argc	=	command_line_args_get_argc()	;
				argv	=	command_line_args_get_argv()	;

				if	(argc	>=	2)	{
								if	(!strcmp(argv[1],	"sie"))	{
												/*	If	main	is	being	invoked	by	the	configuration	processor	(cp)	to	generate	the	sie	resource	file...	*/
												/*	Generate	the	sie	resource	file	*/
												sie_print_xml();
												sie_print_json();

												//	Silently	exit	the	sim	without	printing	the	termination	message
												exit(0)	;
								}

								bool	read_only	=	false;
								bool	oo_dir	=	false;
								bool	o_dir	=	false;

								//	Otherwise,	go	through	the	rest	of	the	sim	args	and	look	for	--read-only-sim
								for	(int	i	=	1;	i	<	argc;	i++)	{
												if	((strcmp("--read-only-sim",	argv[i])	==	0))		{
																//	Set	this	flag	to	move	runtime	generation	of	sie	into	the	output	directory	
																move_runtime_generation	=	true;
																read_only	=	true;
																break;
												}
								}

								//	Set	oo_dir	and	o_dir	flags	accordingly
								for	(int	i	=	1;	i	<	argc;	i++)	{
												if	(strncmp("-OO",	argv[i],	(size_t)	3)	==	0)	{
																oo_dir	=	true;
												}	else	if	(strncmp("-O",	argv[i],	(size_t)	2)	==	0)	{
																o_dir	=	true;
												}
								}

								//	If	--read-only-sim	is	provided	without	either	-OO	or	-O,	exit	with	error	message
								//	For	-OO,	save	S_sie.resource	to	the	output	directory	with	runtime	jobs	data
								if	(read_only	&&	!oo_dir	&&	!o_dir)	{
												std::cerr	<<	"\nERROR:	Missing	-O	or	-OO	argument	with	--read-only-sim	flag"	<<	std::endl;
												exit(1);
								}	else	if	(oo_dir)	{
												copy_sie_resource();
												move_runtime_generation	=	true;
								}
				}

				return(0)	;
}

void	Trick::Sie::top_level_objects_print(std::ofstream	&	sie_out)	{
				Trick::VARIABLE_MAP_ITER	vit	;
				int	jj	;

				for	(	vit	=	trick_MM->variable_map_begin()	;	vit	!=	trick_MM->variable_map_end()	;	++vit	)	{
								ALLOC_INFO	*	alloc_info	=	(*vit).second	;

								if	(	alloc_info	!=	NULL	)	{
												sie_out	<<	"		<top_level_object"	;
												sie_out	<<	"\n			name=\""	<<	vit->first	<<	"\""	;
												sie_out	<<	"\n			type=\""	;
												std::string	type	=	trickTypeCharString(alloc_info->type,	alloc_info->user_type_name	);
												std::replace(type.begin(),	type.end(),	':',	'_');
												sie_out	<<		type	<<	"\"\n"	;
												sie_out	<<	"			alloc_memory_init=\""	<<	alloc_info->alloced_in_memory_init	<<	"\"";
												sie_out	<<	">\n"	;
												if	(	alloc_info->num_index	>	0	)	{
																for	(jj	=	0;	jj	<	alloc_info->num_index;	jj++)	{
																				sie_out	<<	"								<dimension>"	<<	alloc_info->index[jj]	<<	"</dimension>\n"	;
																}
												}
												sie_out	<<	"		</top_level_object>\n\n"	;
								}
				}
}

void	Trick::Sie::runtime_objects_print(std::fstream	&	sie_out)	{
				Trick::VARIABLE_MAP_ITER	vit	;
				int	jj	;

				for	(	vit	=	trick_MM->variable_map_begin()	;	vit	!=	trick_MM->variable_map_end()	;	++vit	)	{
								ALLOC_INFO	*	alloc_info	=	(*vit).second	;

								if	(	alloc_info	!=	NULL	&&	alloc_info->alloced_in_memory_init	==	0)	{
												sie_out	<<	"		<top_level_object"	;
												sie_out	<<	"\n			name=\""	<<	vit->first	<<	"\""	;
												sie_out	<<	"\n			type=\""	;
												std::string	type	=	trickTypeCharString(alloc_info->type,	alloc_info->user_type_name	);
												std::replace(type.begin(),	type.end(),	':',	'_');
												sie_out	<<		type	<<	"\"\n"	;
												sie_out	<<	"			alloc_memory_init=\""	<<	alloc_info->alloced_in_memory_init	<<	"\"";
												sie_out	<<	">\n"	;
												if	(	alloc_info->num_index	>	0	)	{
																for	(jj	=	0;	jj	<	alloc_info->num_index;	jj++)	{
																				sie_out	<<	"								<dimension>"	<<	alloc_info->index[jj]	<<	"</dimension>\n"	;
																}
												}
												sie_out	<<	"		</top_level_object>\n\n"	;
								}
				}
}

void	Trick::Sie::top_level_objects_json(std::ofstream	&	sie_out)	{
				Trick::VARIABLE_MAP_ITER	vit	;
				int	jj	;
				sie_out	<<	"		\"top_level_objects\":	[\n";
				for	(	vit	=	trick_MM->variable_map_begin()	;	vit	!=	trick_MM->variable_map_end()	;	++vit	)	{
								ALLOC_INFO	*	alloc_info	=	(*vit).second	;

								if	(	alloc_info	!=	NULL	)	{
												sie_out	<<	"				{\n"	;
												sie_out	<<	"						\"name\":	\""	<<	vit->first	<<	"\",\n"	;
												sie_out	<<	"						\"type\":	\""	;
												std::string	type	=	trickTypeCharString(alloc_info->type,	alloc_info->user_type_name	);
												std::replace(type.begin(),	type.end(),	':',	'_')	;
												sie_out	<<		type	<<	"\",\n"	;
												sie_out	<<	"						\"alloc_memory_init\":	\""	<<	alloc_info->alloced_in_memory_init	<<	"\"";
												if	(	alloc_info->num_index	>	0	)	{
																sie_out	<<	",\n								\"dimensions\":	["	;
																for	(jj	=	0;	jj	<	alloc_info->num_index	-	1;	jj++)	{
																				sie_out	<<	"	\""	<<	alloc_info->index[jj]	<<	"\","	;
																}
																sie_out	<<	"	\""	<<	alloc_info->index[alloc_info->num_index	-	1]	<<	"\"	"	;
																sie_out	<<	"]\n"	;
																}	else	{
																				sie_out	<<	'\n'	;
																}
												sie_out	<<	"				}"	;
												if(std::next(vit,	1)	!=	trick_MM->variable_map_end())	{
																sie_out	<<	','	;
												}
												sie_out	<<	'\n';
								}
				}
				sie_out	<<	"		]\n";
}

void	Trick::Sie::sie_print_xml()	{
				std::ofstream	sie_out	;
				std::string	file_name	=	std::string(get_runtime_sie_dir())	+	"/"	+	"S_sie.resource"	;
				sie_out.open(file_name.c_str())	;
				sie_out	<<	"<?xml	version=\"1.0\"?>\n\n"	;
				sie_out	<<	"<sie>\n\n"	;
				class_attr_map->print_xml(sie_out)	;
				enum_attr_map->print_xml(sie_out)	;
				top_level_objects_print(sie_out)	;
				sie_out	<<	"</sie>\n"	;
				sie_out.close()	;
}

void	Trick::Sie::sie_append_runtime_objs()	{
				std::fstream	sie_out	;

				if	(move_runtime_generation)	{
								copy_sie_resource();
				}

				std::string	sie_filename	=	get_runtime_sie_dir()	+	"/"	+	"S_sie.resource"	;
				sie_out.open(sie_filename.c_str(),	std::fstream::in	|	std::fstream::out)	;

				const	char	*	comment	=	"<!--\nRuntime	Allocations\nDo	not	edit	this	comment	or	file	content	past	this	point\n-->\n";
				int	comment_len	=	strlen(comment);

				int	curr_offset	=	1;
				int	mem_size	=	1000000;
				char	*	buff	=	new	char[mem_size	+	1];
				char	*	find_str	=	NULL;

				//	initial	read	use	the	whole	buffer
				sie_out.get(buff,	mem_size,	'\0');
				find_str	=	strstr(buff,	comment);

				//	loop	through	the	file	looking	for	the	marker	comment.		The	comment	could	straddle	2	reads.		Copy	the	last	bytes	of
				//	the	previous	read	to	start	the	next	search.
				while	(	!find_str	&&	!sie_out.eof()	)	{
								curr_offset	+=	mem_size	-	comment_len	-	1;
								strncpy(buff,	&buff[mem_size-comment_len-1],	comment_len);
								sie_out.get(&buff[comment_len],	mem_size-comment_len,	'\0');
								find_str	=	strstr(buff,	comment);
				}

				if	(	!find_str	)	{
								std::cerr	<<	"Warning:	Cannot	add	runtime/dynamic	allocations	to	S_sie.resource.	S_sie.resource	is	corrupted,	outdated,	or	missing.	Please	be	sure	that	SIM_*/S_sie.resource	is	preserved	after	build	time	if	needed	at	runtime	for	trick-tv	or	other	variable	server	clients.	Please	also	rerun	trick-CP."	<<	std::endl;
								delete[]	buff;
								return;
				}

				//	calculate	the	position	of	the	marker	in	the	file	and	jump	to	the	point	after	the	comment	ends.
				int	pos	=	curr_offset	+	(find_str	-	buff)	+	comment_len	-	1;
				sie_out.clear();
				sie_out.seekp(pos);

				runtime_objects_print(sie_out);
				sie_out	<<	"</sie>\n";
				sie_out.close();
				delete[]	buff;
}

std::string	Trick::Sie::get_runtime_sie_dir()	{
				if	(move_runtime_generation)	{
								return	std::string(command_line_args_get_output_dir());
				}	else	{
								return	std::string(command_line_args_get_default_dir());
				}
}

void	Trick::Sie::sie_print_json()	{
				std::ofstream	sie_out	;
				std::string	file_name	=	std::string(get_runtime_sie_dir())	+	"/"	+	"S_sie.json"	;
				sie_out.open(file_name.c_str())	;
				sie_out	<<	"{\n"	;
				class_attr_map->print_json(sie_out)	;
				enum_attr_map->print_json(sie_out)	;
				top_level_objects_json(sie_out)	;
				sie_out	<<	"}\n"	;
				sie_out.close()	;
}


void	Trick::Sie::class_attr_map_print_xml()	{
				std::ofstream	sie_out	;
				std::string	file_name	=	std::string(get_runtime_sie_dir())	+	"/"	+	"S_sie_class.xml"	;
				sie_out.open(file_name.c_str())	;
				sie_out	<<	"<?xml	version=\"1.0\"?>\n\n"	;
				sie_out	<<	"<sie>\n"	;
				class_attr_map->print_xml(sie_out)	;
				sie_out	<<	"</sie>\n"	;
				sie_out.close()	;
}

void	Trick::Sie::enum_attr_map_print_xml()	{
				std::ofstream	sie_out	;
				std::string	file_name	=	std::string(get_runtime_sie_dir())	+	"/"	+	"S_sie_enum.xml"	;
				sie_out.open(file_name.c_str())	;
				sie_out	<<	"<?xml	version=\"1.0\"?>\n\n"	;
				sie_out	<<	"<sie>\n"	;
				enum_attr_map->print_xml(sie_out)	;
				sie_out	<<	"</sie>\n"	;
				sie_out.close()	;
}

void	Trick::Sie::top_level_objects_print_xml()	{
				std::ofstream	sie_out	;
				std::string	file_name	=	std::string(get_runtime_sie_dir())	+	"/"	+	"S_sie_top_level_objects.xml"	;
				sie_out.open(file_name.c_str())	;
				sie_out	<<	"<?xml	version=\"1.0\"?>\n\n"	;
				sie_out	<<	"<sie>\n"	;
				top_level_objects_print(sie_out)	;
				sie_out	<<	"</sie>\n"	;
				sie_out.close()	;
}

