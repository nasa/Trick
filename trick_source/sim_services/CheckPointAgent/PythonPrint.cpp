#include	<string>
#include	<cstring>
#include	<iostream>
#include	<iomanip>
#include	<sstream>
#include	<stdlib.h>
#include	<math.h>

#include	"trick/PythonPrint.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/parameter_types.h"
#include	"trick/bitfield_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

//	MEMBER	FUNCTION
bool	Trick::PythonPrint::input_perm_check(ATTRIBUTES	*	attr	__attribute__((unused)))	{
				return	true	;
}

//	MEMBER	FUNCTION
bool	Trick::PythonPrint::output_perm_check(ATTRIBUTES	*	attr	__attribute__((unused)))	{
				return	true	;
}


//	MEMBER	FUNCTION
void	Trick::PythonPrint::write_decl(std::ostream&	chkpnt_os,	ALLOC_INFO	*info)	{
				(void)chkpnt_os	;
				(void)info	;
				return	;
}

//	MEMBER	FUNCTION
void	Trick::PythonPrint::assign_rvalue(std::ostream&	chkpnt_os,	void*	address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

				chkpnt_os	<<	left_side_name()	<<	"	=	";
				write_rvalue(	chkpnt_os,	(void*)address,	attr,	curr_dim,	offset);
				chkpnt_os	<<	";"	<<	std::endl;

				return	;
}

//	MEMBER	FUNCTION
int	Trick::PythonPrint::restore(	std::istream*	checkpoint_stream)	{
				(void)checkpoint_stream	;
				return	0	;
}


//	STATIC	FUNCTION
static	void	write_quoted_str(	std::ostream&	os,	const	char*	s,	bool	in_list)	{
				int	ii;
				int	len	=	strlen(s);
				if	(	in_list	)	{	os	<<	"\""	;	}
				for	(ii=0	;	ii<len	;	ii++)	{
								switch	((s)[ii])	{
								case	'\n':	os	<<	"\\n";	break;
								case	'\t':	os	<<	"\\t";	break;
								case	'\b':	os	<<	"\\b";	break;
								case	'\"':	os	<<	"\\\"";	break;
								default		:	os	<<	s[ii]	;	break;
								}
				}
				if	(	in_list	)	{	os	<<	"\""	;	}
}

void	Trick::PythonPrint::write_singleton(	std::ostream&	chkpnt_os,	void*	address,
	ATTRIBUTES*	attr,	int	offset,	bool	write_units	,	bool	in_list	)	{

				void*	src_addr;

				switch(attr->type)	{
								case	TRICK_UNSIGNED_CHARACTER:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	char);
												chkpnt_os	<<	std::dec	<<	(int)*(unsigned	char*)src_addr	;
								break;
								case	TRICK_BOOLEAN:
												src_addr	=	(char*)address	+	offset	*	sizeof(bool);
												if	(*(bool*)src_addr)	{
																chkpnt_os	<<	"True"	;
												}	else	{
																chkpnt_os	<<	"False"	;
												}
								break;
								case	TRICK_CHARACTER:
												src_addr	=	(char*)address	+	offset	*	sizeof(char);
												if	(isprint(	*(char*)src_addr)	)	{
																chkpnt_os	<<	"'"	<<	*(char*)src_addr	<<	"'"	;
												}	else	{
																//unsigned	int	ch	=	*(unsigned	char*)src_addr;
																//chkpnt_os	<<	"'\\x"	<<	hex	<<	ch	<<	"'"	;
																int	ch	=	*(char*)src_addr;
																chkpnt_os	<<	ch	;
												}
								break;
								case	TRICK_WCHAR:
												src_addr	=	(char*)address	+	offset	*	sizeof(wchar_t);
												chkpnt_os	<<	std::dec	<<	*(wchar_t*)src_addr;
												break;
								case	TRICK_SHORT:
												src_addr	=	(char*)address	+	offset	*	sizeof(short);
												chkpnt_os	<<	std::dec	<<	*(short*)src_addr;
												break;
								case	TRICK_UNSIGNED_SHORT:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	short);
												chkpnt_os	<<	std::dec	<<	*(unsigned	short*)src_addr;
												break;
								case	TRICK_ENUMERATED:	{
																int	ii	=	0;
																int	found	=	0;
																int	value;
																ENUM_ATTR*	enum_attr;

																if	((size_t)attr->size	==	sizeof(int))	{
																				src_addr	=	(char*)address	+	offset	*	sizeof(int);
																				value	=		*(int*)src_addr;
																}	else	if	((size_t)attr->size	==	sizeof(short))	{
																				src_addr	=	(char*)address	+	offset	*	sizeof(short);
																				value	=		*(short*)src_addr;
																}	else	{
																				std::cerr	<<	__FUNCTION__	<<	"	enumeration	size	error."	<<	std::endl;
																				std::cerr.flush();
																				value	=	-1;
																}

																enum_attr	=	(ENUM_ATTR*)attr->attr;

																while	(	!found	&&	(enum_attr[ii].label[0]	!=	'\0'))	{
																			if	(value	==	enum_attr[ii].value)	{
																							chkpnt_os	<<	enum_attr[ii].label;
																							found	=	1;
																			}
																			ii++;
																}
																if	(!found)	{
																				chkpnt_os	<<	std::dec	<<	value;
																}

												}	break;
								case	TRICK_INTEGER:
												src_addr	=	(char*)address	+	offset	*	sizeof(int);
												chkpnt_os	<<	std::dec	<<	*(int*)src_addr;
												break;
								case	TRICK_UNSIGNED_INTEGER:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	int);
												chkpnt_os	<<	std::dec	<<	*(unsigned	int*)src_addr;
												break;
								case	TRICK_LONG:
												src_addr	=	(char*)address	+	offset	*	sizeof(long);
												chkpnt_os	<<	std::dec	<<	*(long*)src_addr;
												break;
								case	TRICK_UNSIGNED_LONG:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	long);
												chkpnt_os	<<	std::dec	<<	*(unsigned	long*)src_addr;
												break;
								case	TRICK_FLOAT:
												src_addr	=	(char*)address	+	offset	*	sizeof(float);
												if	(fpclassify(	*(float*)src_addr)	!=	FP_NAN)	{
																chkpnt_os	<<	std::setprecision(8)	<<	*(float*)src_addr;
												}	else	{
																chkpnt_os	<<	"nan";
												}
												break;
								case	TRICK_DOUBLE:
												src_addr	=	(char*)address	+	offset	*	sizeof(double);
												if	(fpclassify(	*(float*)src_addr)	!=	FP_NAN)	{
																chkpnt_os	<<	std::setprecision(16)	<<	*(double*)src_addr;
												}	else	{
																chkpnt_os	<<	"nan";
												}
												break;
								case	TRICK_BITFIELD:	{
																int	sbf	=	0;
																src_addr	=	(char*)address	+	offset	*	(size_t)attr->size;
																if	(attr->size	==	sizeof(int))	{
																					sbf	=	extract_bitfield_any(	*(int*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	if	(attr->size	==	sizeof(short))	{
																					sbf	=	extract_bitfield_any(	*(short*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	if	(attr->size	==	sizeof(char))	{
																					sbf	=	extract_bitfield_any(	*(char*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	{
																					message_publish(MSG_ERROR,	"Checkpoint	Agent	INTERNAL	ERROR:\n"
																																																"Unsupported	bitfield	size	(%d)	bytes.\n",	attr->size)	;
																}
																chkpnt_os	<<	std::dec	<<	sbf;
												}	break;
								case	TRICK_UNSIGNED_BITFIELD:	{
																int	bf	=	0;
																src_addr	=	(char*)address	+	offset	*	(size_t)attr->size;
																if	(attr->size	==	sizeof(int))	{
																					bf	=	extract_unsigned_bitfield_any(	*(unsigned	int*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	if	(attr->size	==	sizeof(short))	{
																					bf	=	extract_unsigned_bitfield_any(	*(unsigned	short*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	if	(attr->size	==	sizeof(char))	{
																					bf	=	extract_unsigned_bitfield_any(	*(unsigned	char*)src_addr,	attr->size,	attr->index[0].start,	attr->index[0].size);
																}	else	{
																					message_publish(MSG_ERROR,	"Checkpoint	Agent	INTERNAL	ERROR:\n"
																																																"Unsupported	bitfield	size	(%d)	bytes.\n",	attr->size)	;
																}
																chkpnt_os	<<	std::dec	<<	bf;
												}	break;
								case	TRICK_LONG_LONG:
												src_addr	=	(char*)address	+	offset	*	sizeof(long	long);
												chkpnt_os	<<	std::dec	<<	*(long	long*)src_addr;
												break;
								case	TRICK_UNSIGNED_LONG_LONG:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	long	long);
												chkpnt_os	<<	std::dec	<<	*(unsigned	long	long*)src_addr;
												break;
								case		TRICK_FILE_PTR:
												src_addr	=	(char*)address	+	offset	*	sizeof(void*);
												chkpnt_os	<<	*(void**)src_addr;
												break;
								case	TRICK_STRING:
												src_addr	=	(char*)address	+	offset	*	sizeof(std::string);
												write_quoted_str(chkpnt_os,	(*(std::string*)src_addr).c_str(),	in_list);
												break;
								case	TRICK_OPAQUE_TYPE:
												chkpnt_os	<<	"ERROR";
												break;
								default:
												chkpnt_os	<<	"0";
												message_publish(MSG_ERROR,	"Checkpoint	Agent	file	%s:	Unhandled	Type	(%d).\n",	__FILE__,	attr->type)	;
												break;
				}

				if	(	write_units	&&	attr->units	!=	NULL	&&	strcmp(attr->units,	"1")	)	{
												chkpnt_os	<<	"	"	<<	attr->units	;
				}
}

//	MEMBER	FUNCTION
std::string	Trick::PythonPrint::
				ref_string_from_ptr(	void*	pointer,	ATTRIBUTES*	attr,	int	curr_dim)	{

				/**	Notice	that	attr	and	curr_dim	together	specify	the	type.	*/

				std::string	reference_string;

				if	((curr_dim	>=	attr->num_index)	||	(attr->index[curr_dim].size	!=	0))	{
								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	ref_string_from_ptr	called	with	a	non-pointer	type.\n")	;
				}

				if	(pointer	==	NULL)	{
								reference_string	=	"NULL";
				}	else	{
								ALLOC_INFO	*alloc_info;

								/**	Find	the	allocation	that	contains	the	pointer-address.	*/
								alloc_info	=	get_alloc_info_of(	pointer);

								if	(alloc_info	!=	NULL)	{
												std::stringstream	workss;

												if	(	*(void	**)pointer	==	NULL	)	{
																workss	<<	"NULL"	;
												}	else	if	(((curr_dim	+	1)	==	attr->num_index)	&&	(attr->type	==	TRICK_CHARACTER))	{
																workss	<<	"\""	<<	*(char**)pointer	<<	"\""	;
												}	else	{
																workss	<<	*(void	**)pointer	;
												}

												reference_string	=	workss.str()	;

								}	else	if	(((curr_dim	+	1)	==	attr->num_index)	&&	(attr->type	==	TRICK_CHARACTER))	{
												reference_string	+=	(char*)pointer;
								}	else	if	(((curr_dim	+	1)	==	attr->num_index)	&&	(attr->type	==	TRICK_WCHAR))	{
												message_publish(MSG_ERROR,	"Checkpoint	Agent	INTERNAL	ERROR:	TRICK_WCHAR	not	fully	supported	yet.\n")	;
												reference_string	=	"NULL	/*	INTERNAL	ERROR:	TRICK_WCHAR	not	fully	supported	yet.*/";
								}	else	{
												message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Pointer	<%p>	is	not	in	Trick	managed	memory\n"
																																							"nor	is	it	a	character	pointer.\n",	pointer)	;
												reference_string	=	"NULL	/*ERROR*/";
								}
				}
				return(	reference_string);
}

//	MEMBER	FUNCTION
void	Trick::PythonPrint::write_rvalue(	std::ostream&	chkpnt_os,	void*	address,
	ATTRIBUTES*	attr,	int	curr_dim,	int	offset,	bool	write_units	,	bool	in_list	)	{

				//	If	the	variable	that	we	are	pointing	to	is	Un-arrayed
				if	(curr_dim	==	attr->num_index)	{

								write_singleton(	chkpnt_os,	address,	attr,	offset,	write_units,	in_list);

				//	If	the	variable	that	we	are	pointing	to	is	Arrayed
				}	else	if	(curr_dim	<	attr->num_index)	{

								//	If	the	variable	is	a	pointer
								if	(attr->index[curr_dim].size	==	0)	{
												std::string	ref_string;

												//void*	pointer	=	*(void**)((char*)address	+	offset	*	sizeof(void*));

												void*	pointer	=	(char*)address	+	offset	*	sizeof(void*);
												ref_string	=	ref_string_from_ptr(	pointer,	attr,	curr_dim);

												chkpnt_os	<<	ref_string.c_str()	;

								}	else	{	//	Fixed	dimension

												char*	src_addr;

												//	If	this	is	the	final,	fixed	dimension
												if	(((curr_dim	+	1)	==	attr->num_index)	||
																(attr->index[curr_dim	+	1].size	==	0))	{

																int	use_quoted_string;

																//	If	ALL	but	the	last	of	the	elements	of	a	character	array
																//	"isprintable"	AND	the	last	is	'\0'	then	print	out	the	array
																//	as	a	quoted	string.	Otherwise	print	out	each	of	the	characters.

																use_quoted_string	=	1;
																if	(attr->type	==	TRICK_CHARACTER)	{
																				int	ii	=	attr->index[curr_dim].size	-	1;
																				src_addr	=	(char*)address	+	offset	*	attr->index[curr_dim].size;

																				if	(src_addr[ii]	!=	'\0'	||	src_addr[0]	==	'\0'	)	{
																								use_quoted_string	=	0;
																				}
																				ii--;
																				while	(	use_quoted_string	&&	(ii	>=	0)	)	{
																								use_quoted_string	=	isprint(	src_addr[ii])	||	src_addr[ii]	==	'\0'	;
																								ii--;
																				}
																}

																if	((attr->type	==	TRICK_CHARACTER)	&&	use_quoted_string)		{
																				write_quoted_str(chkpnt_os,	src_addr,	in_list);
																}	else	{

																				int	ii;
																				int	array_len;

																				//	Determine	the	number	of	array	elements	we	need	to	print	out
																				//	to	get	all	of	the	non-zero	values.

																				array_len	=	attr->index[curr_dim].size	;
																				chkpnt_os	<<	"[";

																				for	(ii	=	0;	ii	<	array_len	;	ii++	)	{

																								if	(ii	>	0)	{
																												chkpnt_os	<<	",	";
																								}
																								write_rvalue(	chkpnt_os,	address,	attr,	curr_dim	+	1,
																									offset	*	attr->index[curr_dim].size	+	ii,	write_units,	true);
																				}
																				chkpnt_os	<<	"]";

																}

												}	else	{	//	Not	the	final	fixed	dimension.

																int	ii;

																chkpnt_os	<<	"[";

																for	(ii=0	;	ii<	attr->index[curr_dim].size	;	ii++)	{
																				if	(ii	>	0)	{
																								chkpnt_os	<<	",";
																				}
																				write_rvalue(	chkpnt_os,	address,	attr,	curr_dim	+	1,
																					offset	*	attr->index[curr_dim].size	+	ii,	write_units,	true);
																}

																chkpnt_os	<<	"]";
												}
								}

				}	else	{
								chkpnt_os	<<	"/*ERROR*/";

								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	The	specified	current	dimension	\"%d\"	is	greater\n"
																																			"than	the	number	of	dimensions	specified	in	the	type	ATTRIBUTES.\n",	curr_dim)	;

								return;
				}
}

std::string	Trick::PythonPrint::left_side_name()	{

				std::string	name	;
				int	ii;
				int	n_elements	=	leftside_stack.size();

				for	(ii	=	0;	ii	<	n_elements	;	ii++)	{

								VarNameElement	&	element	=	leftside_stack[ii];

								switch(	element.type)	{

												case	BASE_NAME:	{
																name	=	element.name;
												}	break;

												case	ELEM_NAME:	{
																name	+=	'.'	+	element.name;
												}	break;

												case	ARRAY_INDEX:	{
																std::stringstream	index_string;
																index_string	<<	element.index;
																name	+=	'[';
																name	+=	index_string.str();
																name	+=	']';
												}	break;

												default:	{
																message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	BADNESS!!\n")	;
												}
								}
				}
				return(	name);
}
