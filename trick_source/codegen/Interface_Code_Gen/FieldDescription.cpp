
#include	<iostream>
#include	<algorithm>
#include	<sstream>
#include	<set>
#include	<stack>

#include	<libgen.h>
#include	<regex.h>
#include	<string.h>

#include	"FieldDescription.hh"
#include	"Utilities.hh"

//	Provides	units	check	capability.
#include	"trick/map_trick_units_to_udunits.hh"

extern	llvm::cl::opt<	bool	>	units_truth_is_scary	;
extern	llvm::cl::opt<	int	>	debug_level	;

static	ut_system	*	get_u_system()	{

				ut_system	*	u_system	;

				/*	Initialize	the	udunits-2	library	*/
				ut_set_error_message_handler(ut_ignore)	;
				if(	(u_system	=	ut_read_xml(	NULL	))	==	NULL	)	{
								std::cerr	<<	"Error	initializing	udunits-2	unit	system"	<<	std::endl	;
								exit(-1);
				}
				ut_set_error_message_handler(ut_write_to_stderr)	;

				return	u_system	;
}

ut_system	*	FieldDescription::u_system	=	get_u_system()	;

FieldDescription::FieldDescription(
	std::string	in_container_class	)	:
		container_class(in_container_class)	,
		base_class_offset(0)	,
		field_offset(0)	,
		field_width(0)	,
		inherited(false)	,
		virtual_inherited(false)	,
		units("1")	,
		is_dashdash(false)	,
		line_no(0)	,
		io(15)	,
		type_enum_string("TRICK_VOID")	,
		is_bitfield(0)	,
		bitfield_width(0)	,
		bitfield_start_bit(0)	,
		bitfield_word_offset(0)	,
		is_enum(0)	,
		is_record(0)	,
		is_stl(0)	,
		has_stl_clear(1)	,
		is_static(0)	,
		is_reference(0)	,
		num_dims(0)	,
		array_sizes()	{}	;


std::map<std::string	,	unsigned	int	>	create_io_map()	{
				std::map<std::string	,	unsigned	int	>	io_map	;
				io_map[""]	=	3	;
				io_map["o"]	=	1	;
				io_map["*o"]	=	1	;
				io_map["*i"]	=	2	;
				io_map["i"]	=	2	;
				io_map["*oi"]	=	3	;
				io_map["oi"]	=	3	;
				io_map["*io"]	=	3	;
				io_map["io"]	=	3	;
				io_map["**"]	=	0	;
				//	The	"--"	is	a	bad	value,	but	we'll	take	it	for	now.
				//	TODO:	Make	this	an	error	instead	of	being	nice!
				io_map["--"]	=	4	;
				return	io_map	;
}

std::map<std::string	,	unsigned	int	>	FieldDescription::io_map	=	create_io_map()	;

std::string	FieldDescription::get_regex_field(std::string	input	,	const	char	*	expr	,	unsigned	int	index)	{
				int	ret	;
				regex_t	reg_expr	;
				regmatch_t	pmatch[10]	;
				memset(pmatch	,	0	,	sizeof(pmatch))	;
				regcomp(	&reg_expr	,	expr	,	REG_EXTENDED	)	;
				ret	=	regexec(	&reg_expr	,	input.c_str()	,	10	,	pmatch	,	0	)	;
				regfree(&reg_expr)	;
				if	(	ret	==	0	)	{
								if(debug_level	>=	4)	std::cout	<<	"pmatch	range	=	"	<<	pmatch[index].rm_so	<<	"	"	<<	pmatch[index].rm_eo	<<	std::endl	;
								if	(	pmatch[index].rm_so	!=	-1	)	{
												return	input.substr(pmatch[index].rm_so	,	pmatch[index].rm_eo	-	pmatch[index].rm_so	)	;
								}
				}	else	if	(debug_level	>=	4){
								char	error_msg[1024]	;
								regerror(	ret	,	&reg_expr	,	(char	*)error_msg	,	1024	)	;
								std::cerr	<<	error_msg	<<	std::endl	;
				}
				return	std::string()	;
}

//	global	set	of	all	units	processed.	hopefully	saves	time	from	reparsing	same	units	strings	over	and	over
std::set<	std::string	>	valid_units	;
extern	llvm::cl::opt<	int	>	debug_level	;

void	FieldDescription::parseComment(std::string	comment)	{
				std::string	ret_str	;

				bool	units_found	=	false	;
				bool	io_found	=	false	;
				bool	chkpnt_io_found	=	false	;
				unsigned	int	chkpnt_io	;

				if	(	comment.empty()	)	{
								//	If	the	comment	is	empty	default	all	I/O	enabled.
								io	=	15	;
								return	;
				}

				if	(	debug_level	>=	5	)	{
								std::cout	<<	"comment	before	"	<<	comment	<<	std::endl	;
				}

				//	remove	open	comment	chars
				comment	=	get_regex_field(comment	,	"^(//|/\\*)(.*)"	,	2)	;
				if(debug_level	>=	4)	std::cout	<<	"1.	"	<<	comment	<<	std::endl	;

				//	remove	optional	doxygen	comment	chars
				//	Note:	I	had	to	use	[	\t\n\r]	for	\s	because	the	Mac	don't	understand!
				comment	=	get_regex_field(comment	,	"^((\\*|!)<)?[	\t\n\r]*(.*)"	,	3)	;
				if(debug_level	>=	4)	std::cout	<<	"2.	"	<<	comment	<<	std::endl	;

				//	remove	optional	doxygen	keyword
				comment	=	get_regex_field(comment	,	"(\\\\[a-zA-Z0-9]+)?[	\t\n\r]*(.*)"	,	2)	;
				if(debug_level	>=	4)	std::cout	<<	"3.	"	<<	comment	<<	std::endl	;

				ret_str	=	get_regex_field(comment	,	"@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}]"	,	1)	;
				if	(	!	ret_str.empty())	{
								chkpnt_io	=	io_map[ret_str]	;
								if(debug_level	>=	4)	std::cout	<<	"go	for	trick_chkpnt_io	"	<<		io	<<	std::endl	;
								chkpnt_io_found	=	true	;
								comment	=	get_regex_field(comment	,	"(.*)@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}]"	,	1)	+
									get_regex_field(comment	,	"@?trick_chkpnt_io[\\({]([^\\)}]+)[\\)}](.*)"	,	2)	;
				}
				ret_str	=	get_regex_field(comment	,	"@?cio[\\({]([^\\)}]+)[\\)}]"	,	1)	;
				if	(	!	ret_str.empty())	{
								chkpnt_io	=	io_map[ret_str]	;
								if(debug_level	>=	4)	std::cout	<<	"go	for	cio	"	<<		io	<<	std::endl	;
								chkpnt_io_found	=	true	;
								comment	=	get_regex_field(comment	,	"(.*)@?cio[\\({]([^\\)}]+)[\\)}]"	,	1)	+
									get_regex_field(comment	,	"@?cio[\\({]([^\\)}]+)[\\)}](.*)"	,	2)	;
				}

				ret_str	=	get_regex_field(comment	,	"@?trick_io[\\({]([^\\)}]+)[\\)}]"	,	1)	;
				if	(	!	ret_str.empty())	{
								io	=	io_map[ret_str]	;
								if(debug_level	>=	4)	std::cout	<<	"go	for	trick_io	"	<<		io	<<	std::endl	;
								io_found	=	true	;
								comment	=	get_regex_field(comment	,	"(.*)@?trick_io[\\({]([^\\)}]+)[\\)}]"	,	1)	+
									get_regex_field(comment	,	"@?trick_io[\\({]([^\\)}]+)[\\)}](.*)"	,	2)	;
				}
				ret_str	=	get_regex_field(comment	,	"@?io[\\({]([^\\)}]+)[\\)}]"	,	1)	;
				if	(	!	ret_str.empty())	{
								io	=	io_map[ret_str]	;
								if(debug_level	>=	4)	std::cout	<<	"go	for	io	"	<<		io	<<	std::endl	;
								io_found	=	true	;
								comment	=	get_regex_field(comment	,	"(.*)@?io[\\({]([^\\)}]+)[\\)}]"	,	1)	+
									get_regex_field(comment	,	"@?io[\\({]([^\\)}]+)[\\)}](.*)"	,	2)	;
				}

				/*
							Units	can	include	parenthesis	now.		We	need	to	match	the	parenthesis	in
							trick_units()	to	get	the	whole	units	string.
					*/
				std::size_t	tu_string	=	comment.find("trick_units")	;
				if	(	tu_string	!=	std::string::npos	)	{
								std::size_t	ustart	=	tu_string	+	std::string("trick_units").length()	;
								std::size_t	uend	=	ustart	+	1	;
								std::stack<char>	parens	;
								parens.push(	comment[ustart]);
								while	(	!	parens.empty()	and	(uend	<	comment.length()))	{
												switch	(	comment[uend]	)	{
																case	'(':
																				parens.push('(')	;
																				break	;
																case	')':
																				if	(parens.top()	==	'(')	{
																								parens.pop()	;
																				}
																				break	;
																case	'}':
																				if	(parens.top()	==	'{')	{
																								parens.pop()	;
																				}
																				break	;
												}
												uend++	;
								}
								if	(	parens.empty()	)	{
												units	=	comment.substr(ustart	+	1	,	uend	-	ustart	-	2)	;
												units_found	=	true	;
												//	If	we	have	"@trick_units"	include	the	"@"	sign	for	erasure.
												if	(	tu_string	>	0	and	comment[tu_string-1]	==	'@'	)	{
																tu_string	-=	1	;
												}
												comment.erase(tu_string	,	uend	-	tu_string)	;
								}	else	{
												std::cout	<<	"unmatched	parenthesis	for	trick_units"	<<	std::endl	;
								}
				}

				if	(	!	io_found	)	{
								//	Note:	I	had	to	use	[	\t\n\r]	for	\s	because	the	Mac	don't	understand!
								ret_str	=	get_regex_field(comment	,	"^[	\t\n\r]*(\\*io|\\*oi|\\*i|\\*o|\\*\\*)"	,	1)	;
								if(debug_level	>=	4)	std::cout	<<	"3.	"	<<	ret_str	<<	std::endl	;
								if	(	!	ret_str.empty())	{
												io	=	io_map[ret_str]	;
												if(debug_level	>=	4)	std::cout	<<	"stand-alone	io	"	<<		io	<<	std::endl	;
												io_found	=	true	;
												comment	=	get_regex_field(comment	,	"^[	\t\n\r]*(\\*io|\\*oi|\\*i|\\*o|\\*\\*)[	\t\n\r]*(.*)"	,	2)	;
								}
				}

				if(debug_level	>=	4)	std::cout	<<	"3.	"	<<	comment	<<	std::endl	;
				if	(	!	units_found	)	{
								ret_str	=	get_regex_field(comment	,	"^[	\t\n\r]*\\(([^\\)]*)\\)"	,	1)	;
								if	(	!	ret_str.empty())	{
												units	=	ret_str	;
												if(debug_level	>=	4)	std::cout	<<	"stand-alone	units	"	<<	units	<<	std::endl	;
												units_found	=	true	;
												comment	=	get_regex_field(comment	,	"^[	\t\n\r]*\\(([^\\)]*)\\)(.*)"	,	2)	;
								}	else	{
												ret_str	=	get_regex_field(comment	,	"^[	\t\n\r]*([^	\t\n\r)]*)"	,	1)	;
												if	(	!	ret_str.empty())	{
																units	=	ret_str	;
																if(debug_level	>=	4)	std::cout	<<	"stand-alone	units	"	<<	units	<<	"	"	<<	comment	<<	std::endl	;
																units_found	=	true	;
																comment	=	get_regex_field(comment	,	"^[	\t\n\r]*([^	\t\n\r)]*)(.*)"	,	2)	;
												}
								}
				}


				//	Test	if	we	have	valid	units.		We	need	to	have	found	a	units	string	and	an	io	spec	not	zero
				//	Possible	todo	is	to	create	a	map	of	valid	units	so	we	don't	have	to	retest	each	string.
				if	(	units_found	and	io	!=	0	and	(valid_units.find(units)	==	valid_units.end()))	{
								//	remove	spaces
								units.erase(remove_if(units.begin(),	units.end(),	isspace),	units.end());
								if	(	!units.compare("--")	)	{
												units	=	"1"	;
												is_dashdash	=	true	;
								}	else	{
												//	map	old	unit	names	to	new	names
												std::string	new_units	=	map_trick_units_to_udunits(units)	;
												if	(	units.compare(new_units)	)	{
																std::cout	<<	bold(color(WARNING,	"Warning				")	+	file_name	+	":"	+	std::to_string(line_no))	<<	std::endl
																				<<	"											Units	converted	from	"	<<	quote(units)	<<	"	to	"	<<	quote(new_units)	<<	std::endl;
																units	=	new_units	;
												}
												ut_unit	*	test_units	=	ut_parse(u_system,	units.c_str()	,	UT_ASCII)	;
												if	(	test_units	==	NULL	)	{
																//	If	the	units	are	invalid	write	an	error	message	and	change	the	units	to	"1"
																std::cout	<<	bold(color(WARNING,	"Warning				")	+	file_name	+	":"	+	std::to_string(line_no))	<<	std::endl
																				<<	"											Invalid	units	specification.	Changing	"	<<	quote(units)	<<	"	to	"	<<	quote("1")	<<	std::endl;
																units	=	"1"	;
												}	else	{
																//	If	the	units	are	valid,	free	the	memory	allocated	by	new_units.
																ut_free(test_units)	;
																valid_units.insert(units)	;
												}
								}
				}

				if	(	io	==	4	)	{
								std::cout	<<	bold(color(WARNING,	"Warning				")	+	file_name	+	":"	+	std::to_string(line_no))	<<	std::endl
												<<	"											--	is	not	a	valid	trick_io	value.	Setting	to	*io"	<<	std::endl	;
								io	=	3	;
				}

				if	(	chkpnt_io_found	==	true	)	{
								//	If	a	checkpoint	I/O	spec	is	found	add	it	to	the	io	field.
								io	=	(chkpnt_io	<<	2	)	+	(	io	&	3	)	;
				}	else	if	(	io_found	==	true	)	{
								//	else	duplicated	the	io	field	to	the	chkpnt	io	field.
								io	|=	(io	<<	2	)	;
				}

				//	The	rest	of	the	comment	is	the	description	of	the	variable.

				//	remove	the	c	comment	end	marker.
				comment	=	get_regex_field(comment	,	"(.*)\\*/"	,	1)	;

				//	posix	c	regular	expressions	are	terrible.	the	regexes	above	will	leave	"@"	signs	because
				//	the	regular	expressions	are	so	greedy.
				comment	=	get_regex_field(comment	,	"^[	\t\n\r@]+(.*)"	,	1)	;

				//	remove	leading	and	trailing	whitespace
				comment	=	trim(comment)	;

				//	escape	special	characters,	convert	tabs	and	newlines	to	spaces,	remove	multiple	spaces.
				std::ostringstream	ss	;
				bool	is_space	=	false	;
				for	(std::string::iterator	it	=	comment.begin();	it	!=	comment.end();	++it)	{
								switch	(*it)	{
												case	'\\':	ss	<<	"\\\\";	is_space	=	false	;	break;
												case	'"':	ss	<<	"\\\"";	is_space	=	false	;	break;
												case	'\b':	ss	<<	"\\b";	is_space	=	false	;	break;
												case	'\f':	ss	<<	"\\f";	is_space	=	false	;	break;
												case	'\n':
												case	'\r':
												case	'\t':
												case	'	':	if	(	!	is_space	)	ss	<<	"	";	is_space	=	true	;	break;
												default:	ss	<<	*it;	is_space	=	false	;	break;
								}
				}

				description	=	ss.str()	;

}

std::string	FieldDescription::getContainerClass()	{
				return	container_class	;
}

void	FieldDescription::setContainerClass(std::string	in_name	)	{
				container_class	=	in_name	;
}

void	FieldDescription::setBaseClassOffset(unsigned	int	in_offset)	{
				base_class_offset	=	in_offset	;
}

unsigned	int	FieldDescription::getBaseClassOffset()	{
				return	base_class_offset	;
}

void	FieldDescription::setFieldOffset(unsigned	int	in_offset)	{
				field_offset	=	in_offset	;
}

unsigned	int	FieldDescription::getFieldOffset()	{
				return	field_offset	;
}

void	FieldDescription::setFieldWidth(unsigned	int	in_width)	{
				field_width	=	in_width	;
}

unsigned	int	FieldDescription::getFieldWidth()	{
				return	field_width	;
}

void	FieldDescription::setNonCanonicalTypeName(	std::string	in_val	)	{
				non_canonical_type_name	=	in_val	;
}

std::string	FieldDescription::getNonCanonicalTypeName()	{
				return	non_canonical_type_name	;
}

void	FieldDescription::setTypeName(	std::string	in_val	)	{
				type_name	=	in_val	;
}

std::string	FieldDescription::getTypeName()	{
				return	type_name	;
}

void	FieldDescription::setLineNo(	unsigned	int	in_val	)	{
				line_no	=	in_val	;
}

unsigned	int	FieldDescription::getLineNo()	{
				return	line_no	;
}

void	FieldDescription::setMangledTypeName(	std::string	in_val	)	{
				mangled_type_name	=	in_val	;
}

std::string	FieldDescription::getMangledTypeName()	{
				if	(	mangled_type_name.empty()	)	{
								return	type_name	;
				}
				return	mangled_type_name	;
}

std::string	FieldDescription::getFullyQualifiedMangledTypeName(const	std::string&	delimiter)	{
				std::ostringstream	oss	;
				printNamespacesAndContainerClasses(oss,	delimiter)	;
				oss	<<	getMangledTypeName()	;
				return	oss.str()	;
}

std::string	FieldDescription::getUnits()	{
				return	units	;
}

bool	FieldDescription::isDashDashUnits()	{
				return	is_dashdash	;
}

void	FieldDescription::setIO(unsigned	int	in_io)	{
				io	=	in_io	;
}

unsigned	int	FieldDescription::getIO()	{
				return	io	;
}

unsigned	int	FieldDescription::getChkpntIO()	{
				return	io	>>	2	&	3	;
}

bool	FieldDescription::isWriteable()	{
				return	io	&	1;
}

bool	FieldDescription::isReadable()	{
				return	io	&	2;
}

bool	FieldDescription::isCheckpointable()	{
				return	io	&	4;
}

bool	FieldDescription::isRestorable()	{
				return	io	&	8;
}

std::string	FieldDescription::getDescription()	{
				return	description	;
}

void	FieldDescription::setInherited(bool	in_inherited)	{
				inherited	=	in_inherited	;
}

bool	FieldDescription::isInherited()	{
				return	inherited	;
}

void	FieldDescription::setVirtualInherited(bool	in_inherited)	{
				virtual_inherited	=	in_inherited	;
}

bool	FieldDescription::isVirtualInherited()	{
				return	virtual_inherited	;
}

void	FieldDescription::setAccess(	clang::AccessSpecifier	in_val	)	{
				access	=	in_val	;
}

clang::AccessSpecifier	FieldDescription::getAccess()	{
				return	access	;
}

void	FieldDescription::addOffset(	unsigned	int	offset	)	{
				field_offset	+=	offset	;
}

void	FieldDescription::setEnumString(std::string	in_str)	{
				type_enum_string	=	in_str	;
}

std::string	FieldDescription::getEnumString()	{
				return	type_enum_string	;
}

void	FieldDescription::setBitField(bool	yes_no)	{
				is_bitfield	=	yes_no	;
}

bool	FieldDescription::isBitField()	{
				return	is_bitfield	;
}

void	FieldDescription::setBitFieldWidth(unsigned	int	len)	{
				bitfield_width	=	len	;
}

void	FieldDescription::setBitFieldStart(unsigned	int	sb)	{
				bitfield_start_bit	=	sb	;
}

void	FieldDescription::setBitFieldByteOffset(unsigned	int	wo)	{
				bitfield_word_offset	=	wo	;
}

unsigned	int	FieldDescription::getBitFieldWidth()	{
				return	bitfield_width	;
}

unsigned	int	FieldDescription::getBitFieldStart()	{
				return	bitfield_start_bit	;
}

unsigned	int	FieldDescription::getBitFieldByteOffset()	{
				return	bitfield_word_offset	;
}

void	FieldDescription::setEnum(bool	yes_no)	{
				is_enum	=	yes_no	;
}

bool	FieldDescription::isEnum()	{
				return	is_enum	;
}

void	FieldDescription::setRecord(bool	yes_no)	{
				is_record	=	yes_no	;
}

bool	FieldDescription::isRecord()	{
				return	is_record	;
}

void	FieldDescription::setSTL(bool	yes_no)	{
				is_stl	=	yes_no	;
}

bool	FieldDescription::isSTL()	{
				return	is_stl	;
}

void	FieldDescription::setSTLClear(bool	yes_no)	{
				has_stl_clear	=	yes_no	;
}

bool	FieldDescription::hasSTLClear()	{
				return	has_stl_clear	;
}

void	FieldDescription::setStatic(bool	yes_no)	{
				is_static	=	yes_no	;
}

bool	FieldDescription::isStatic()	{
				return	is_static	;
}

void	FieldDescription::setReference(bool	yes_no)	{
				is_reference	=	yes_no	;
}

bool	FieldDescription::isReference()	{
				return	is_reference	;
}

unsigned	int	FieldDescription::getNumDims()	{
				return	num_dims	;
}

int	FieldDescription::getArrayDim(unsigned	int	dim_num)	{
				if	(	dim_num	<	8	)	{
								return	array_sizes[dim_num]	;
				}
				return	0	;
}

void	FieldDescription::addArrayDim(	int	in_dim	)	{
				array_sizes[num_dims++]	=	in_dim	;
}

std::ostream	&	operator	<<	(std::ostream	&	ostream	,	FieldDescription	&	fieldDescription	)	{
				ostream	<<	"				name	=	"	<<	fieldDescription.name	<<	std::endl	;
				ostream	<<	"				file_name	=	"	<<	fieldDescription.file_name	<<	std::endl	;
				ostream	<<	"				namespaces	="	;
				fieldDescription.printNamespaces(ostream)	;
				ostream	<<	std::endl	;
				ostream	<<	"				parent	classes	="	;
				fieldDescription.printContainerClasses(ostream)	;
				ostream	<<	std::endl	;
				ostream	<<	"				line_no	=	"	<<	fieldDescription.line_no	<<	std::endl	;
				ostream	<<	"				container_class	=	"	<<	fieldDescription.container_class	<<	std::endl	;
				ostream	<<	"				type_name	=	"	<<	fieldDescription.type_name	<<	std::endl	;
				ostream	<<	"				mangled_type_name	=	"	<<	fieldDescription.mangled_type_name	<<	std::endl	;
				ostream	<<	"				type_enum_string	=	"	<<	fieldDescription.type_enum_string	<<	std::endl	;
				ostream	<<	"				units	=	"	<<	fieldDescription.units	<<	std::endl	;
				ostream	<<	"				io	=	"	<<	fieldDescription.io	<<	std::endl	;
				ostream	<<	"				description	=	"	<<	fieldDescription.description	<<	std::endl	;
				ostream	<<	"				access	=	"	<<	fieldDescription.access	<<	std::endl	;
				ostream	<<	"				is_bitfield	=	"	<<	fieldDescription.is_bitfield	<<	std::endl	;
				ostream	<<	"				bitfield_width	=	"	<<	fieldDescription.bitfield_width	<<	std::endl	;
				ostream	<<	"				bitfield_start_bit	=	"	<<	fieldDescription.bitfield_start_bit	<<	std::endl	;
				ostream	<<	"				bitfield_word_offset	=	"	<<	fieldDescription.bitfield_word_offset	<<	std::endl	;
				ostream	<<	"				num_dims	=	"	<<	fieldDescription.num_dims	<<	std::endl	;
				ostream	<<	"				array_sizes	="	;
				for(	unsigned	int	ii	=	0	;	ii	<	8	;	ii++	)	{
								ostream	<<	"	"	<<	fieldDescription.array_sizes[ii]	;
				}
				ostream	<<	std::endl	;
				ostream	<<	"				is_enum	=	"	<<	fieldDescription.is_enum	<<	std::endl	;
				ostream	<<	"				is_record	=	"	<<	fieldDescription.is_record	<<	std::endl	;
				ostream	<<	"				is_static	=	"	<<	fieldDescription.is_static	<<	std::endl	;

				return	ostream	;
}
