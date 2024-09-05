#include	<stdlib.h>
#include	<iostream>
#include	<udunits2.h>
#include	<math.h>	//	for	fpclassify
#include	<iomanip>	//	for	setprecision
#include	<string.h>
#include	<sstream>

#include	"trick/VariableReference.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/wcs_ext.h"
#include	"trick/map_trick_units_to_udunits.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/UdUnits.hh"
#include	"trick/bitfield_proto.h"
#include	"trick/trick_byteswap.h"


//	Static	variables	to	be	addresses	that	are	known	to	be	the	error	ref	address
int	Trick::VariableReference::_bad_ref_int	=	0	;
int	Trick::VariableReference::_do_not_resolve_bad_ref_int	=	0	;

REF2*	Trick::VariableReference::make_error_ref(std::string	in_name)	{
				REF2*	new_ref;
				new_ref	=	(REF2*)calloc(1,	sizeof(REF2));
				new_ref->reference	=	strdup(in_name.c_str())	;
				new_ref->units	=	NULL	;
				new_ref->address	=	(char	*)&_bad_ref_int	;
				new_ref->attr	=	(ATTRIBUTES*)calloc(1,	sizeof(ATTRIBUTES))	;
				new_ref->attr->type	=	TRICK_NUMBER_OF_TYPES	;
				new_ref->attr->units	=	(char	*)"--"	;
				new_ref->attr->size	=	sizeof(int)	;
				return	new_ref;
}

REF2*	Trick::VariableReference::make_do_not_resolve_ref(std::string	in_name)	{
				REF2*	new_ref;
				new_ref	=	(REF2*)calloc(1,	sizeof(REF2));
				new_ref->reference	=	strdup(in_name.c_str())	;
				new_ref->units	=	NULL	;
				new_ref->address	=	(char	*)&_do_not_resolve_bad_ref_int	;
				new_ref->attr	=	(ATTRIBUTES*)calloc(1,	sizeof(ATTRIBUTES))	;
				new_ref->attr->type	=	TRICK_NUMBER_OF_TYPES	;
				new_ref->attr->units	=	(char	*)"--"	;
				new_ref->attr->size	=	sizeof(int)	;
				return	new_ref;
}

//	Helper	function	to	deal	with	time	variable
REF2*	make_time_ref(double	*	time)	{
				REF2*	new_ref;
				new_ref	=	(REF2*)calloc(1,	sizeof(REF2));
				new_ref->reference	=	strdup("time")	;
				new_ref->units	=	strdup("s")	;
				new_ref->address	=	(char	*)time	;
				new_ref->attr	=	(ATTRIBUTES*)calloc(1,	sizeof(ATTRIBUTES))	;
				new_ref->attr->type	=	TRICK_DOUBLE	;
				new_ref->attr->units	=	strdup("s")	;
				new_ref->attr->size	=	sizeof(double)	;
				return	new_ref;
}

Trick::VariableReference::VariableReference(std::string	var_name,	double*	time)	:	_staged(false),	_write_ready(false)	{
				if	(var_name	!=	"time")	{
								ASSERT(0);
				}

				_var_info	=	make_time_ref(time);

				//	Set	up	member	variables
				_address	=	_var_info->address;
				_size	=	_var_info->attr->size	;
				_deref	=	false;

				//	Deal	with	weirdness	around	string	vs	wstring
				_trick_type	=	_var_info->attr->type	;

				//	Allocate	stage	and	write	buffers
				_stage_buffer	=	calloc(_size,	1)	;
				_write_buffer	=	calloc(_size,	1)	;

				_conversion_factor	=	cv_get_trivial();
				_base_units	=	_var_info->attr->units;
				_requested_units	=	"s";
				_name	=	_var_info->reference;
}

Trick::VariableReference::VariableReference(std::string	var_name)	:	_staged(false),	_write_ready(false)	{

				if	(var_name	==	"time")	{
								ASSERT(0);
				}	else	{
								//	get	variable	attributes	from	memory	manager
								_var_info	=	ref_attributes(var_name.c_str());
				}

				//	Handle	error	cases
				if	(	_var_info	==	NULL	)	{
								//	TODO:	ERROR	LOGGER	sendErrorMessage("Variable	Server	could	not	find	variable	%s.\n",	var_name);
								//	PRINTF	IS	NOT	AN	ERROR	LOGGER	@me
								message_publish(MSG_ERROR,	"Variable	Server	could	not	find	variable	%s.\n",	var_name.c_str());
								_var_info	=	make_error_ref(var_name);
				}	else	if	(	_var_info->attr	)	{
								if	(	_var_info->attr->type	==	TRICK_STRUCTURED	)	{
												//	sendErrorMessage("Variable	Server:	var_add	cant	add	\"%s\"	because	its	a	composite	variable.\n",	var_name);
												message_publish(MSG_ERROR,	"Variable	Server:	var_add	cant	add	\"%s\"	because	its	a	composite	variable.\n",	var_name.c_str());

												free(_var_info);
												_var_info	=	make_do_not_resolve_ref(var_name);

								}	else	if	(	_var_info->attr->type	==	TRICK_STL	)	{
												//	sendErrorMessage("Variable	Server:	var_add	cant	add	\"%s\"	because	its	an	STL	variable.\n",	var_name);
												message_publish(MSG_ERROR,"Variable	Server:	var_add	cant	add	\"%s\"	because	its	an	STL	variable.\n",	var_name.c_str());

												free(_var_info);
												_var_info	=	make_do_not_resolve_ref(var_name);
								}
				}	else	{
								//	sendErrorMessage("Variable	Server:	BAD	MOJO	-	Missing	ATTRIBUTES.");
								message_publish(MSG_ERROR,	"Variable	Server:	BAD	MOJO	-	Missing	ATTRIBUTES.");

								free(_var_info);
								_var_info	=	make_error_ref(var_name);
				}

				//	Set	up	member	variables
				_var_info->units	=	NULL;
				_address	=	_var_info->address;
				_size	=	_var_info->attr->size	;
				_deref	=	false;

				//	Deal	with	weirdness	around	string	vs	wstring
				_trick_type	=	_var_info->attr->type	;

				if	(	_var_info->num_index	==	_var_info->attr->num_index	)	{
								//	single	value	-	nothing	else	necessary
				}	else	if	(	_var_info->attr->index[_var_info->attr->num_index	-	1].size	!=	0	)	{
								//	Constrained	array
								for	(	int	i	=	_var_info->attr->num_index-1;		i	>	_var_info->num_index-1	;	i--	)	{
												_size	*=	_var_info->attr->index[i].size	;
								}
				}	else	{
								//	Unconstrained	array
								if	((_var_info->attr->num_index	-	_var_info->num_index)	>	1	)	{
												message_publish(MSG_ERROR,	"Variable	Server	Error:	var_add(%s)	requests	more	than	one	dimension	of	dynamic	array.\n",	_var_info->reference);
												message_publish(MSG_ERROR,	"Data	is	not	contiguous	so	returned	values	are	unpredictable.\n")	;
								}
								if	(	_var_info->attr->type	==	TRICK_CHARACTER	)	{
												_trick_type	=	TRICK_STRING	;
												_deref	=	true;
								}	else	if	(	_var_info->attr->type	==	TRICK_WCHAR	)	{
												_trick_type	=	TRICK_WSTRING	;
												_deref	=	true;
								}	else	{
												_deref	=	true	;
												_size	*=	get_size((char*)_address)	;
								}
				}
				//	handle	strings:	set	a	max	buffer	size,	the	copy	size	may	vary	so	will	be	set	in	copy_sim_data
				if	((	_trick_type	==	TRICK_STRING	)	||	(	_trick_type	==	TRICK_WSTRING	))	{
								_size	=	MAX_ARRAY_LENGTH	;
				}

				//	Allocate	stage	and	write	buffers
				_stage_buffer	=	calloc(_size,	1)	;
				_write_buffer	=	calloc(_size,	1)	;

				_conversion_factor	=	cv_get_trivial();
				_base_units	=	_var_info->attr->units;
				_requested_units	=	"";
				_name	=	_var_info->reference;

				//	Done!
}

Trick::VariableReference::~VariableReference()	{
				if	(_var_info	!=	NULL)	{
								free(	_var_info	);
								_var_info	=	NULL;
				}
				if	(_stage_buffer	!=	NULL)	{
								free	(_stage_buffer);
								_stage_buffer	=	NULL;
				}
				if	(_write_buffer	!=	NULL)	{
								free	(_write_buffer);
								_write_buffer	=	NULL;
				}
				if	(_conversion_factor	!=	NULL)	{
								cv_free(_conversion_factor);
				}
}

std::string	Trick::VariableReference::getName()	const	{
				return	_name;
}

int	Trick::VariableReference::getSizeBinary()	const	{
				return	_size;
}

TRICK_TYPE	Trick::VariableReference::getType()	const	{
				return	_trick_type;
}

std::string	Trick::VariableReference::getBaseUnits()	const	{
				return	_base_units;
}

int	Trick::VariableReference::setRequestedUnits(std::string	units_name)	{
				//	Some	error	logging	lambdas	-	these	should	probably	go	somewhere	else
				//	But	I	do	kinda	like	them
				auto	publish	=	[](MESSAGE_TYPE	type,	const	std::string&	message)	{
								std::ostringstream	oss;
								oss	<<	"Variable	Server:	"	<<	message	<<	std::endl;
								message_publish(type,	oss.str().c_str());
				};

				auto	publishError	=	[&](const	std::string&	units)	{
								std::ostringstream	oss;
								oss	<<	"units	error	for	["	<<	getName()	<<	"]	["	<<	units	<<	"]";
								publish(MSG_ERROR,	oss.str());
				};

				//	If	the	units_name	parameter	is	"xx",	set	it	to	the	current	units.
				if	(!units_name.compare("xx"))	{
								units_name	=	getBaseUnits();
				}

				//	if	unitless	('--')	then	do	not	convert	to	udunits	
				if(units_name.compare("--"))	{
								//	Check	to	see	if	this	is	an	old	style	Trick	unit	that	needs	to	be	converted	to	new	udunits
								std::string	new_units	=	map_trick_units_to_udunits(units_name)	;
								//	Warn	if	a	conversion	has	taken	place
								if	(	units_name.compare(new_units)	)	{
												//	TODO:	MAKE	BETTER	SYSTEM	FOR	ERROR	LOGGING
												std::ostringstream	oss;
												oss	<<	"["	<<	getName()	<<	"]	old-style	units	converted	from	["
																<<	units_name	<<	"]	to	["	<<	new_units	<<	"]";
												publish(MSG_WARNING,	oss.str());
								}

								//	Interpret	base	unit
								ut_unit	*	from	=	ut_parse(Trick::UdUnits::get_u_system(),	getBaseUnits().c_str(),	UT_ASCII)	;
								if	(	!from	)	{
												std::cout	<<	"Error	in	interpreting	base	units"	<<	std::endl;
												publishError(getBaseUnits());
												ut_free(from)	;
												return	-1	;
								}

								//	Interpret	requested	unit
								ut_unit	*	to	=	ut_parse(Trick::UdUnits::get_u_system(),	new_units.c_str(),	UT_ASCII)	;
								if	(	!to	)	{
												std::cout	<<	"Error	in	interpreting	requested	units"	<<	std::endl;
												publishError(new_units);
												ut_free(from)	;
												ut_free(to)	;
												return	-1	;
								}

								//	Create	a	converter	from	the	base	to	the	requested
								auto	new_conversion_factor	=	ut_get_converter(from,	to)	;
								ut_free(from)	;
								ut_free(to)	;
								if	(	!new_conversion_factor	)	{
												std::ostringstream	oss;
												oss	<<	"["	<<	getName()	<<	"]	cannot	convert	units	from	["	<<	getBaseUnits()
																<<	"]	to	["	<<	new_units	<<	"]";
												publish(MSG_ERROR,	oss.str());
												return	-1	;
								}	else	{
												_conversion_factor	=	new_conversion_factor;
								}
				
								//	Set	the	requested	units.	This	will	cause	the	unit	string	to	be	printed	in	write_value_ascii
								_requested_units	=	new_units;
				}
				return	0;
}

int	Trick::VariableReference::stageValue(bool	validate_address)	{
				_write_ready	=	false;

				//	Copy	<size>	bytes	from	<address>	to	staging_point.

				//	Try	to	recreate	connection	if	it	has	been	broken
				if	(_var_info->address	==	&_bad_ref_int)	{
								REF2	*new_ref	=	ref_attributes(_var_info->reference);
								if	(new_ref	!=	NULL)	{
												_var_info	=	new_ref;
												_address	=	_var_info->address;
												//	_requested_units	=	"";
								}
				}

				//	if	there's	a	pointer	somewhere	in	the	address	path,	follow	it	in	case	pointer	changed
				if	(	_var_info->pointer_present	==	1	)	{
								_address	=	follow_address_path(_var_info)	;
								if	(_address	==	NULL)	{
												tagAsInvalid();
								}	else	if	(	validate_address	)	{
												validate();
								}	else	{
												_var_info->address	=	_address	;
								}
				}

				//	if	this	variable	is	a	string	we	need	to	get	the	raw	character	string	out	of	it.
				if	((	_trick_type	==	TRICK_STRING	)	&&	!_deref)	{
								std::string	*	str_ptr	=	(std::string	*)_var_info->address	;
								//	Get	a	pointer	to	the	internal	character	array
								_address	=	(void	*)(str_ptr->c_str())	;
				}

				//	if	this	variable	itself	is	a	pointer,	dereference	it
				if	(	_deref	)	{
								_address	=	*(void**)_var_info->address	;
				}

				//	handle	c++	string	and	char*
				if	(	_trick_type	==	TRICK_STRING	)	{
								if	(_address	==	NULL)	{
												_size	=	0	;
								}	else	{
												_size	=	strlen((char*)_address)	+	1	;
								}
				}
				//	handle	c++	wstring	and	wchar_t*
				if	(	_trick_type	==	TRICK_WSTRING	)	{
								if	(_address	==	NULL)	{
												_size	=	0	;
								}	else	{
												_size	=	wcslen((wchar_t	*)_address)	*	sizeof(wchar_t);
								}
				}
				if(_address	!=	NULL)	{
								memcpy(	_stage_buffer	,	_address	,	_size	)	;
				}

				_staged	=	true;
				return	0;
}

bool	Trick::VariableReference::validate()	{
				//	The	address	is	not	NULL.
				//	Should	be	called	by	VariableServer	Session	if	validateAddress	is	on.
				//	check	the	memory	manager	if	the	address	falls	into
				//	any	of	the	memory	blocks	it	knows	of.		Don't	do	this	if	we	have	a	std::string	or
				//	wstring	type,	or	we	already	are	pointing	to	a	bad	ref.
				if	(	(_trick_type	!=	TRICK_STRING)	and
												(_trick_type	!=	TRICK_WSTRING)	and
												(_var_info->address	!=	&_bad_ref_int)	and
												(get_alloc_info_of(_address)	==	NULL)	)	{
								
								//	This	variable	is	broken,	make	it	into	an	error	ref
								tagAsInvalid();
								return	false;
				}

				//	Everything	is	fine
				return	true;
}

static	void	write_escaped_string(	std::ostream&	os,	const	char*	s)	{
				for	(int	ii=0	;	ii<strlen(s)	;	ii++)	{
								if	(isprint(s[ii]))	{
												os	<<	s[ii];
								}	else	{
												switch	((s)[ii])	{
																case	'\n':	os	<<	"\\n";	break;
																case	'\t':	os	<<	"\\t";	break;
																case	'\b':	os	<<	"\\b";	break;
																case	'\a':	os	<<	"\\a";	break;
																case	'\f':	os	<<	"\\f";	break;
																case	'\r':	os	<<	"\\n";	break;
																case	'\v':	os	<<	"\\v";	break;
																case	'\"':	os	<<	"\\\"";	break;
																default		:	{
																				//	Replicating	behavior	from	original	vs_format_ascii
																				char	temp_s[6];
																				sprintf(temp_s,	"\\x%02x",	s[ii]);
																				os	<<	temp_s	;	
																				break;
																}
												}
								}
				}
}

int	Trick::VariableReference::getSizeAscii()	const	{
				std::stringstream	ss;
				writeValueAscii(ss);
				return	ss.str().length();
}


int	Trick::VariableReference::writeValueAscii(	std::ostream&	out	)	const	{
				//	This	is	copied	and	modified	from	vs_format_ascii

				if	(!isWriteReady())	{
								return	-1;
				}

				int	bytes_written	=	0;
				void	*	buf_ptr	=	_write_buffer	;
				while	(bytes_written	<	_size)	{
								bytes_written	+=	_var_info->attr->size	;

								switch	(_trick_type)	{

								case	TRICK_CHARACTER:
												if	(_var_info->attr->num_index	==	_var_info->num_index)	{
																//	Single	char
																out	<<	(int)cv_convert_double(_conversion_factor,	*(char	*)buf_ptr);
												}	else	{
																//	All	but	last	dim	specified,	leaves	a	char	array	
																write_escaped_string(out,	(const	char	*)	buf_ptr);
																bytes_written	=	_size	;
												}
												break;
								case	TRICK_UNSIGNED_CHARACTER:
												if	(_var_info->attr->num_index	==	_var_info->num_index)	{
																//	Single	char
																out	<<	(unsigned	int)cv_convert_double(_conversion_factor,*(unsigned	char	*)buf_ptr);
												}	else	{
																//	All	but	last	dim	specified,	leaves	a	char	array	
																write_escaped_string(out,	(const	char	*)	buf_ptr);
																bytes_written	=	_size	;
												}
												break;

								case	TRICK_WCHAR:{
																if	(_var_info->attr->num_index	==	_var_info->num_index)	{
																				out	<<	*(wchar_t	*)	buf_ptr;
																}	else	{
																				//	convert	wide	char	string	char	string
																				size_t	len	=	wcs_to_ncs_len((wchar_t	*)buf_ptr)	+	1	;

																				char	temp_buf[len];
																				wcs_to_ncs((wchar_t	*)	buf_ptr,	temp_buf,	len);
																				out	<<	temp_buf;
																				bytes_written	=	_size	;
																}
												}
												break;

								case	TRICK_STRING:
												if	((char	*)	buf_ptr	!=	NULL)	{
																write_escaped_string(out,	(const	char	*)	buf_ptr);
																bytes_written	=	_size	;
												}	else	{
																out	<<	'\0';
												}
												break;

								case	TRICK_WSTRING:
												if	((wchar_t	*)	buf_ptr	!=	NULL)	{
																//	convert	wide	char	string	char	string
																size_t	len	=	wcs_to_ncs_len(	(wchar_t	*)buf_ptr)	+	1	;

																char	temp_buf[len];
																wcs_to_ncs(		(wchar_t	*)	buf_ptr,	temp_buf,	len);
																out	<<	temp_buf;
																bytes_written	=	_size	;
												}	else	{
																out	<<	'\0';
												}
												break;
								case	TRICK_SHORT:
												out	<<	(short)cv_convert_double(_conversion_factor,*(short	*)buf_ptr);
												break;

								case	TRICK_UNSIGNED_SHORT:
												out	<<	(unsigned	short)cv_convert_double(_conversion_factor,*(unsigned	short	*)buf_ptr);
												break;

								case	TRICK_INTEGER:
								case	TRICK_ENUMERATED:
												out	<<	(int)cv_convert_double(_conversion_factor,*(int	*)buf_ptr);
												break;

								case	TRICK_BOOLEAN:
												out	<<	(int)cv_convert_double(_conversion_factor,*(bool	*)buf_ptr);
												break;

								case	TRICK_BITFIELD:
												out	<<	(GET_BITFIELD(buf_ptr,	_var_info->attr->size,	_var_info->attr->index[0].start,	_var_info->attr->index[0].size));
												break;

								case	TRICK_UNSIGNED_BITFIELD:
												out	<<	(GET_UNSIGNED_BITFIELD(buf_ptr,	_var_info->attr->size,	_var_info->attr->index[0].start,	_var_info->attr->index[0].size));
												break;
												
								case	TRICK_UNSIGNED_INTEGER:
												out	<<	(unsigned	int)cv_convert_double(_conversion_factor,*(unsigned	int	*)buf_ptr);
												break;

								case	TRICK_LONG:	{
												long	l	=	*(long	*)buf_ptr;
												if	(_conversion_factor	!=	cv_get_trivial())	{
																l	=	(long)cv_convert_double(_conversion_factor,	l);
												}
												out	<<	l;
												break;
								}

								case	TRICK_UNSIGNED_LONG:	{
												unsigned	long	ul	=	*(unsigned	long	*)buf_ptr;
												if	(_conversion_factor	!=	cv_get_trivial())	{
																ul	=	(unsigned	long)cv_convert_double(_conversion_factor,	ul);
												}
												out	<<	ul;
												break;
								}

								case	TRICK_FLOAT:
												out	<<	std::setprecision(8)	<<	cv_convert_float(_conversion_factor,*(float	*)buf_ptr);
												break;

								case	TRICK_DOUBLE:
												out	<<	std::setprecision(16)	<<	cv_convert_double(_conversion_factor,*(double	*)buf_ptr);
												break;

								case	TRICK_LONG_LONG:	{
												long	long	ll	=	*(long	long	*)buf_ptr;
												if	(_conversion_factor	!=	cv_get_trivial())	{
																ll	=	(long	long)cv_convert_double(_conversion_factor,	ll);
												}
												out	<<	ll;
												break;
								}

								case	TRICK_UNSIGNED_LONG_LONG:	{
												unsigned	long	long	ull	=	*(unsigned	long	long	*)buf_ptr;
												if	(_conversion_factor	!=	cv_get_trivial())	{
																ull	=	(unsigned	long	long)cv_convert_double(_conversion_factor,	ull);
												}
												out	<<	ull;
												break;
								}

								case	TRICK_NUMBER_OF_TYPES:
												out	<<	"BAD_REF";
												break;

								default:{

												break;
								}
								}	//	end	switch

								if	(bytes_written	<	_size)	{
								//	if	returning	an	array,	continue	array	as	comma	separated	values
												out	<<	",";
												buf_ptr	=	(void*)	((long)buf_ptr	+	_var_info->attr->size)	;
								}
				}	//end	while

				if	(_requested_units	!=	"")	{
								if	(	_var_info->attr->mods	&	TRICK_MODS_UNITSDASHDASH	)	{
												out	<<	"	{--}";
								}	else	{
												out	<<	"	{"	<<	_requested_units	<<	"}";
								}
				}

				return	0;
}

void	Trick::VariableReference::tagAsInvalid	()	{
				std::string	save_name(getName())	;
				free(_var_info)	;
				_var_info	=	make_error_ref(save_name)	;
				_address	=	_var_info->address	;
}


int	Trick::VariableReference::prepareForWrite()	{
				if	(!_staged)	{
								return	1;
				}

				void	*	temp_p	=	_stage_buffer;
				_stage_buffer	=	_write_buffer;
				_write_buffer	=	temp_p;

				_staged	=	false;
				_write_ready	=	true;
				return	0;
}

bool	Trick::VariableReference::isStaged()	const	{
				return	_staged;
}

bool	Trick::VariableReference::isWriteReady()	const	{
				return	_write_ready;
}

int	Trick::VariableReference::writeTypeBinary(	std::ostream&	out,	bool	byteswap	)	const	{
				int	local_type	=	_trick_type;
				if	(byteswap)	{
								local_type	=	trick_byteswap_int(local_type);
				}
				out.write(const_cast<const	char	*>(reinterpret_cast<char	*>(&local_type)),	sizeof(int));

				return	0;
}

int	Trick::VariableReference::writeSizeBinary(	std::ostream&	out,	bool	byteswap	)	const	{
				int	local_size	=	_size;
				if	(byteswap)	{
								local_size	=	trick_byteswap_int(local_size);
				}
				out.write(const_cast<const	char	*>(reinterpret_cast<char	*>(&local_size)),	sizeof(int));

				return	0;
}

int	Trick::VariableReference::writeNameBinary(	std::ostream&	out,	bool	byteswap	)	const	{
				std::string	name	=	getName();
				out.write(name.c_str(),	name.size());

				return	0;
}

int	Trick::VariableReference::writeNameLengthBinary(	std::ostream&	out,	bool	byteswap	)	const	{
				int	name_size	=	getName().size();
				if	(byteswap)	{
								name_size	=	trick_byteswap_int(name_size);
				}

				out.write(const_cast<const	char	*>(reinterpret_cast<char	*>(&name_size)),	sizeof(int));

				return	0;
}


void	Trick::VariableReference::byteswap_var	(char	*	out,	char	*	in)	const	{
				byteswap_var(out,	in,	*this);
}


void	Trick::VariableReference::byteswap_var	(char	*	out,	char	*	in,	const	VariableReference&	ref)	{
				ATTRIBUTES	*	attr	=	ref._var_info->attr;
				int	array_size	=	1;

				//	Determine	how	many	elements	are	in	this	array	if	it	is	an	array
				for	(int	j	=	0;	j	<	ref._var_info->attr->num_index;	j++)	{
								array_size	*=	attr->index[j].size;
				}

				switch	(attr->size)	{
								case	1:
												//	If	these	are	just	characters,	no	need	to	byteswap
												for	(int	j	=	0;	j	<	array_size;	j++)	{
																out[j]	=	in[j];
												}
												break;

								case	2:	{
												short	*	short_in	=	reinterpret_cast<short	*>	(in);
												short	*	short_out	=	reinterpret_cast<short	*>	(out);

												for	(int	j	=	0;	j	<	array_size;	j++)	{
																short_out[j]	=	trick_byteswap_short(short_in[j]);
												}
												break;
								}

								case	4:	{
												int	*	int_in	=	reinterpret_cast<int	*>	(in);
												int	*	int_out	=	reinterpret_cast<int	*>	(out);

												for	(int	j	=	0;	j	<	array_size;	j++)	{
																int_out[j]	=	trick_byteswap_int(int_in[j]);
												}
												break;
								}
								case	8:	{
												//	We	don't	actually	care	if	this	is	double	or	long,	just	that	it's	the	right	size
												double	*	double_in	=	reinterpret_cast<double	*>	(in);
												double	*	double_out	=	reinterpret_cast<double	*>	(out);

												for	(int	j	=	0;	j	<	array_size;	j++)	{
																double_out[j]	=	trick_byteswap_double(double_in[j]);
												}
												break;
								}
				}
}



int	Trick::VariableReference::writeValueBinary(	std::ostream&	out,	bool	byteswap	)	const	{

				if	(	_trick_type	==	TRICK_BITFIELD	)	{
								int	temp_i	=	GET_BITFIELD(_write_buffer	,	_var_info->attr->size	,
												_var_info->attr->index[0].start,	_var_info->attr->index[0].size)	;
								out.write((char	*)(&temp_i),	_size);
								return	_size;
				}

				if	(	_trick_type	==	TRICK_UNSIGNED_BITFIELD	)	{
								int	temp_unsigned	=	GET_UNSIGNED_BITFIELD(_write_buffer	,	_var_info->attr->size	,
																_var_info->attr->index[0].start,	_var_info->attr->index[0].size)	;
								out.write((char	*)(&temp_unsigned),	_size);
								return	_size;
				}

				if	(_trick_type	==		TRICK_NUMBER_OF_TYPES)	{
								//	TRICK_NUMBER_OF_TYPES	is	an	error	case
								int	temp_zero	=	0	;
								out.write((char	*)(&temp_zero),	_size);
								return	_size;
				}

				if	(byteswap)	{
								char	*	byteswap_buf	=	(char	*)	calloc	(_size,	1);
								byteswap_var(byteswap_buf,	(char	*)	_write_buffer);
								out.write(byteswap_buf,	_size);
								free	(byteswap_buf);
				}
				else	{
								out.write((char	*)	_write_buffer,	_size);
				}

				return	_size;
				
}		

std::ostream&	Trick::operator<<	(std::ostream&	s,	const	Trick::VariableReference&	ref)	{
				s	<<	"						\""	<<	ref.getName()	<<	"\"";
				return	s;
}