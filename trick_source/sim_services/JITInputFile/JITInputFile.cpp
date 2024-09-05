
#include	<iostream>
#include	<fstream>
#include	<sstream>
#include	<cstring>
#include	<string>

#include	<string.h>
#include	<dlfcn.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<errno.h>


#include	"trick/JITInputFile.hh"
#include	"trick/command_line_protos.h"
#include	"trick/message_type.h"
#include	"trick/message_proto.h"
#include	"trick/env_proto.h"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"
#include	"trick/TrickConstant.hh"

Trick::JITInputFile	*	the_jit	=	NULL	;

//TODO:	Move	JITLibInfo	code	into	the	JITLibInfo	object.

Trick::JITInputFile::JITInputFile()	{
				the_jit	=	this	;

				JITLibInfo	li	;
				li.library_name	=	"S_main"	;
				li.handle	=	dlopen(	NULL	,	RTLD_LAZY	)	;
				file_to_libinfo_map["S_main"]	=	li	;

}

/**
@details
-#	Test	if	the	input	file	on	the	command	line	ends	in	.cpp	or	.cc
-#	If	it	does	save	the	input	file	locally	and	clear	the	command	line	argument	so
			the	cpp	input	file	will	not	go	through	the	input	processor.
*/
int	Trick::JITInputFile::process_sim_args()	{

				std::string	cpp	=	".cpp"	;
				std::string	cc	=	".cc"	;

				std::string	&	ifr	=	command_line_args_get_input_file_ref()	;
				if	(	!	ifr.empty()	)	{
								if	(!	ifr.compare(	ifr.length()	-	cpp.length()	,	cpp.length(),	cpp)	or
												!	ifr.compare(	ifr.length()	-		cc.length()	,		cc.length(),		cc)	)	{
												input_file	=	ifr	;
												ifr.clear()	;
								}
				}

				return	0	;
}

/**
@details
-#	If	the	incoming	file	name	has	already	been	successfully	compiled,	return	0
-#	Get	the	basename	of	the	input	file
-#	Get	the	root	name	of	the	input	file	(no	extension)
-#	Set	all	of	the	output	file	names	based	on	the	output	directory	and	the	input	file	root	name
-#	Make	the	output	directory
-#	Open	the	makefile	for	writing
-#	Create	a	makefile	that	will	compile	the	input	file	to	a	shared	library
-#	Call	make	to	compile	the	library	file.
-#	Extract	the	return	status	from	the	system	call.
-#	If	the	compilation	was	unsuccessful,	exec_terminate
-#	The	library	compile	successfully.		Add	library	name	to	map
*/
int	Trick::JITInputFile::compile(std::string	file_name)	{

				std::ofstream	outfile	;
				std::ostringstream	ss	;

				std::string	output_directory	;
				std::string	object_fullpath_name	;
				std::string	library_fullpath_name	;
				std::string	makefile_fullpath_name	;
				std::string	input_file_rootname	;
				std::string	input_file_basename	;
				std::string	dep_file_name	;
				struct	stat	output_dir_stat	;
				int	ret	;
				size_t	pos	;

				//	If	the	incoming	file	name	has	already	been	successfully	compiled,	return	now.
				if	(	file_to_libinfo_map.find(file_name)	!=	file_to_libinfo_map.end()	)	{
								return	0	;
				}

				//	get	the	basename	of	the	input	file
				if	(	(pos	=	file_name.find_last_of("/"))	!=	std::string::npos	)	{
								input_file_basename	=	file_name.substr(pos	+	1	)	;
				}	else	{
								input_file_basename	=	file_name	;
				}

				//	get	the	root	name	of	the	input	file	(no	extension)
				if	(	(pos	=	input_file_basename.find_last_of("."))	!=	std::string::npos	)	{
								input_file_rootname	=	input_file_basename.substr(0,	pos)	;
				}	else	{
								input_file_rootname	=	input_file_basename	;
				}

				//	set	all	of	the	output	file	names	based	on	the	output	directory	and	the	input	file	root	name
				output_directory	=	std::string(command_line_args_get_output_dir())	+	"/jitlib"	;
				makefile_fullpath_name	=	output_directory	+	"/Makefile."	+	input_file_rootname	;
				object_fullpath_name	=	output_directory	+	"/"	+	input_file_rootname	+	".o"	;
				library_fullpath_name	=	output_directory	+	"/lib"	+	input_file_rootname	+	".so"	;
				dep_file_name	=	output_directory	+	"/"	+	input_file_rootname	+	".d"	;

				//	make	the	output	directory
				ret	=	stat(	output_directory.c_str(),	&output_dir_stat)	;
				if	(	ret	==	-1	and	errno	==	ENOENT	)	{
								mkdir(output_directory.c_str(),	0775)	;
				}	else	if	(	ret	==	0	and	!	S_ISDIR(output_dir_stat.st_mode))	{
								std::string	error_message	=	"JITInputfile	could	not	create	directory	"	+	output_directory	;
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	error_message.c_str()	)	;
				}

				//	open	the	makefile	for	writing
				outfile.open(makefile_fullpath_name.c_str())	;

				//	create	a	makefile	that	will	compile	the	input	file	to	a	shared	library
				outfile	<<	"#	JITInputFile	auto	generated	Makefile.		Paths	are	relative	to	the	simulation	directory"	<<	std::endl	;
				outfile	<<	"#	To	manually	use	this	file	go	to	sim	directory	and	use	\"make	-f	"	<<	makefile_fullpath_name	<<	"	<makefile_target>\""	;
				outfile	<<	std::endl	<<	std::endl	;
				//	rule	to	link	shared	library
				outfile	<<	library_fullpath_name	<<	":	"	<<	object_fullpath_name	<<	std::endl	;
#ifdef	__APPLE__
				outfile	<<	"\t"	<<	get_trick_env((char	*)"TRICK_CXX")	<<	"	"	<<	get_trick_env((char	*)"TRICK_LDFLAGS")	<<	"	-shared	-undefined	dynamic_lookup	-o	$@	$<	"	<<	std::endl	<<	std::endl	;
#else
				outfile	<<	"\t"	<<	get_trick_env((char	*)"TRICK_CXX")	<<	"	"	<<	get_trick_env((char	*)"TRICK_LDFLAGS")	<<	"	-shared	-o	$@	$<	"	<<	std::endl	<<	std::endl	;
#endif
				//	rule	to	compile	cpp	file
				outfile	<<	object_fullpath_name	<<	":	"	<<	file_name	<<	std::endl	;
				outfile	<<	"\t"	<<	get_trick_env((char	*)"TRICK_CXX")	<<	"	"	<<	get_trick_env((char	*)"TRICK_CXXFLAGS")	;
				outfile	<<	"	"	<<	get_trick_env((char	*)"TRICK_SYSTEM_CXXFLAGS")	;
				outfile	<<	"	-MMD	-c	-fPIC	-o	$@	$<"	<<	std::endl	<<	std::endl	;
				//	rule	to	clean
				outfile	<<	"clean:"	<<	std::endl	;
				outfile	<<	"\t	rm	-f	"	<<	object_fullpath_name	<<	"	"	<<	library_fullpath_name	;
				outfile	<<	"	"	<<	dep_file_name	<<	std::endl	<<	std::endl	;
				//	dependency	file
				outfile	<<	"-include	"	<<	dep_file_name	<<	std::endl	;

				outfile.close()	;

				//	call	make	to	compile	the	library	file.
				ss	<<	"make	-s	-f	"	<<	makefile_fullpath_name	<<	std::endl	;
				ret	=	system(ss.str().c_str())	;

				//	extract	the	return	status	from	the	system	call.
				ret	=	WEXITSTATUS(ret)	;

				//	If	the	compilation	was	unsuccessful,	exec_terminate
				if	(	ret	!=	0	)	{
								std::string	error_message	=	std::string("JITInputFile	shared	library	creation	failed:	")
																																		+	std::strerror(errno);
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	error_message.c_str());
				}

				//	The	library	compile	successfully.		Add	library	name	to	map
				JITLibInfo	li	;
				li.library_name	=	library_fullpath_name	;
				file_to_libinfo_map[file_name]	=	li	;

				return	0	;

}

/**
@details
-#	Check	if	we	have	compiled	this	library	yet
				-#	If	we	have	not	compiled	the	library	then	exec_terminate
-#	If	the	library	handle	is	NULL,	try	and	open	the	library
				-#	If	the	library	open	fails,	exec_terminate
-#	Look	up	the	symbol	run_function
				-#	If	the	symbol	is	not	found	exec_terminate.
-#	Call	the	function
*/
int	Trick::JITInputFile::run(std::string	file_name	,	std::string	run_function	)	{
				//	Check	if	we	have	compiled	this	library	yet.
				if	(	file_to_libinfo_map.find(file_name)	==	file_to_libinfo_map.end()	)	{
								//	If	we	have	not	compiled	the	library	then	exec_terminate
								std::string	error_message	=	"JITInputfile	library	not	found	for	"	+	file_name	;
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	error_message.c_str()	)	;
				}

				JITLibInfo	&	li	=	file_to_libinfo_map.find(file_name)->second	;
				//	if	the	library	handle	is	NULL,	try	and	open	the	library.
				if	(	li.handle	==	NULL	)	{
								li.handle	=	dlopen(	li.library_name.c_str()	,	RTLD_LAZY	)	;
								//	if	the	library	open	fails,	exec_terminate
								if	(	li.handle	==	NULL	)	{
												std::string	error_message	=	"JITInputfile	dlopen	failed	on	"	+	li.library_name	;
												exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	error_message.c_str()	)	;
								}
				}

				//	Look	up	the	symbol	name
				int	(*call_me)(void)	=	(int	(*)(void))dlsym(	li.handle	,	run_function.c_str())	;
				if	(	call_me	==	NULL	)	{
								std::string	error_message	=	"JITInputfile	could	not	find	function	"	+	run_function	;
								return	exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	error_message.c_str()	)	;
				}	else	{
								//	We	found	the	function,	call	it!
								return	(*call_me)()	;
				}
}

/**
@details
-#	If	the	file_name	is	not	empty
				-#	Compile	the	file_name
				-#	If	the	file	compiled	successfully
								-#	Execute	the	run_function
*/
int	Trick::JITInputFile::compile_and_run(std::string	file_name,	std::string	run_function)	{

				int	ret	=	0	;

				//	If	the	file	name	is	not	empty
				if	(	!	file_name.empty()	)	{
								//	Attempt	to	compile	the	library.
								ret	=	compile(file_name)	;
								if	(	ret	==	0	)	{
												//	If	the	compilation	was	successful	execute	run_function.
												ret	=	run(	file_name	,	run_function	)	;
								}
				}

				//	Return	0	or	the	return	value	of	compile/run.
				return	ret	;
}

/**
@details
-#	If	the	incoming	file	name	has	already	been	added,	return	0
-#	Add	library	name	to	map
*/
int	Trick::JITInputFile::add_library(std::string	lib_name)	{

				//	If	the	incoming	file	name	has	already	been	added,	return	0
				if	(	file_to_libinfo_map.find(lib_name)	!=	file_to_libinfo_map.end()	)	{
								return	0	;
				}

				//	Add	library	name	to	map
				JITLibInfo	li	;
				li.library_name	=	lib_name	;
				li.handle	=	dlopen(	li.library_name.c_str()	,	RTLD_LAZY	)	;
				file_to_libinfo_map[lib_name]	=	li	;

				return	0	;
}

void	*	Trick::JITInputFile::find_symbol(std::string	sym)	{
				std::map<	std::string	,	JITLibInfo	>::iterator	it	;
				for	(	it	=	file_to_libinfo_map.begin()	;	it	!=	file_to_libinfo_map.end()	;	++it	)	{
								void	*	ret	=	(*it).second.find_symbol(sym)	;
								if	(ret	!=	NULL)	{
												return	ret	;
								}
				}
				return	NULL	;
}

/**
@details
-#	Call	compile_and_run	with	the	input_file	from	the	command	line
*/
int	Trick::JITInputFile::init()	{
				//	Compile	and	run	the	input	file.		It's	ok	if	input_file	is	empty.
				return	compile_and_run(input_file)	;
}

