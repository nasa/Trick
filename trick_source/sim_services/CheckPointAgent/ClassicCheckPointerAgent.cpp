#include	"trick/MemoryManager.hh"
#include	"trick/parameter_types.h"
#include	"trick/io_alloc.h"
#include	"trick/wcs_ext.h"
#include	"trick/bitfield_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

#include	"trick/ClassicCheckPointAgent.hh"
#include	"trick/ChkPtParseContext.hh"

#include	<string>
#include	<iostream>
#include	<iomanip>
#include	<sstream>
#include	<stdlib.h>
#include	<math.h>
#include	<string.h>

const	int	Trick::ClassicCheckPointAgent::array_elements_per_line[TRICK_NUMBER_OF_TYPES]	=	{
					5,	/**	TRICK_VOID	(for	pointers)	*/
				20,	/**	TRICK_CHARACTER	*/
				20,	/**	TRICK_UNSIGNED_CHARACTER	*/
					1,	/**	TRICK_STRING	*/
				10,	/**	TRICK_SHORT	*/
				10,	/**	TRICK_UNSIGNED_SHORT	*/
					5,	/**	TRICK_INTEGER	*/
					5,	/**	TRICK_UNSIGNED_INTEGER	*/
					5,	/**	TRICK_LONG	*/
					5,	/**	TRICK_UNSIGNED_LONG	*/
					5,	/**	TRICK_FLOAT	*/
					5,	/**	TRICK_DOUBLE	*/
					5,	/**	TRICK_BITFIELD	*/
					5,	/**	TRICK_UNSIGNED_BITFIELD	*/
					5,	/**	TRICK_LONG_LONG	*/
					5,	/**	TRICK_UNSIGNED_LONG_LONG	*/
					5,	/**	TRICK_FILE_PTR	*/
				10,	/**	TRICK_BOOLEAN	*/
				10,	/**	TRICK_WCHAR	*/
					1,	/**	TRICK_WSTRING	*/
					5,	/**	TRICK_VOID_PTR	*/
				10,	/**	TRICK_ENUMERATED	*/
					5,	/**	TRICK_STRUCTURED	(for	pointers)	*/
					5,	/**	TRICK_OPAQUE_TYPE	*/
					1		/**	TRICK_STL	*/
};

//	MEMBER	FUNCTION
Trick::ClassicCheckPointAgent::ClassicCheckPointAgent(	Trick::MemoryManager	*MM)	{

			mem_mgr	=	MM;
			reduced_checkpoint	=	1;
			hexfloat_checkpoint	=	0;
			debug_level	=	0;
}

//	MEMBER	FUNCTION
Trick::ClassicCheckPointAgent::~ClassicCheckPointAgent()	{	}

//	MEMBER	FUNCTION
bool	Trick::ClassicCheckPointAgent::input_perm_check(ATTRIBUTES	*	attr)	{
				return	(attr->io	&	TRICK_CHKPNT_INPUT)	;
}

bool	Trick::ClassicCheckPointAgent::output_perm_check(ATTRIBUTES	*	attr)	{
				return	(attr->io	&	TRICK_CHKPNT_OUTPUT)	;
}

//	MEMBER	FUNCTION
std::string	Trick::ClassicCheckPointAgent::left_side_name()	{

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
																message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Left	side	name-stack	appears	to	be	corrupted.\n")	;
												}
								}
				}
				return(	name);
}

//	MEMBER	FUNCTION
void	Trick::ClassicCheckPointAgent::write_decl(std::ostream&	chkpnt_os,	ALLOC_INFO	*info)	{

				const	char	*type_spec;

				type_spec	=	trickTypeCharString(info->type,	info->user_type_name);

				if	(info->stcl	==	TRICK_EXTERN)	{
								chkpnt_os	<<	"//	extern	";
				}

				if	(	info->num_index	==	0	)	{

								chkpnt_os	<<	type_spec	<<	"	"	<<	info->name	<<	";\n";

				}	else	if	((info->num_index	>	0)	&&	(info->num_index	<=	TRICK_MAX_INDEX))	{
								int	ii;

								chkpnt_os	<<	type_spec;

								ii	=	info->num_index-1;
								while	((ii	>=	0)	&&	(info->index[ii]	==	0))	{
												chkpnt_os	<<	"*";
												ii	--;
								}

								chkpnt_os	<<	"	"	<<	info->name	;

								ii	=	0;
								while	((ii	<	info->num_index)	&&	(info->index[ii]	!=	0))	{
												chkpnt_os	<<	"["	<<	info->index[ii]	<<	"]"	;
												ii	++;
								}
										chkpnt_os	<<	";"	<<	std::endl;

				}	else	{
								//	ERROR	-	num_index	cant	be	negative.
				}
}


//	STATIC	FUNCTION
/*	
			Given	an	offset,	that	is	within	the	bounds	of	a	composite
			object	(i.e.,	a	struct	or	class	instance),	return	the	corresponding	
			ATTRIBUTES	pointer	(singular).

			Helper	function	for	getCompositeSubReference.
.
*/

static	ATTRIBUTES*	findMember(ATTRIBUTES*	A,	long	referenceOffset)	{
				int	i	=	0;
				int	j;
				int	temp_size;
				/*	Find	the	member	which	contains	the	address	pointed	to	by	rAddr	*/
				while	((A[i].name[0]	!=	'\0'))	{

								//	if	mod	bit	0	is	set,	A[i]	is	a	reference.	Width	of	reference	is	stored
								//	in	mod	bits	3-8.	We	could	use	sizeof(void*),	but	that	is	implementation
								//	specific	and	not	required	by	C++	standard.
								if	((A[i].mods	&	1)	==	1)	{
										temp_size	=	((A[i].mods	>>	3)	&	0x3F);
										//	Calculate	the	size	(temp_size)	of	the	referenced	member	variable.
								}	else	if	(A[i].num_index	!=	0)	{
												//	if	size	of	last	valid	index	is	0,	then	we	are	looking	at	a	pointer
												if	(A[i].index[A[i].num_index	-	1].size	==	0)	{
																temp_size	=	sizeof(void*);
												}	else	{
																temp_size	=	A[i].size;
												}
												for	(j	=	0;	j	<	A[i].num_index;	j++)	{
																if	(A[i].index[j].size	!=	0)	{
																				temp_size	*=	A[i].index[j].size;
																}	else	{
																				break;
																}
												}
								}	else	{
												temp_size	=	A[i].size;
								}

								//	if	the	reference	is	not	to	this	member	variable,	move	on	to	the	next	member	variable.
								if	(	(	referenceOffset	<		(long)	(	A[i].offset)			)	||
													(	referenceOffset	>=	(long)	(	A[i].offset	+	temp_size))
											)	{
												i++;
								}	else	{
												break;
								}
				}

				return	&(A[i]);
}

//	STATIC	FUNCTION
/*
			Given	an	address,	that	is	within	the	bounds	of	a	composite
			object	(i.e.,	a	struct	or	class	instance),	store	the	corresponding	sub-name
			in	reference_name	buffer	and	return.

			A	return	value	of	1	indicates	an	error	occured.	A	return	value	of	0	indicates
			success.

			The	following	BNF	production	describes	a	valid	sub-reference:

			<sub_reference>	-->	""
																					|	"+"	+	<offset>
																					|	"["	+	<index>+	"]"	+	<sub_reference>
																					|	"."	+	<member_name>	+	<sub_reference>

			<offset>	is	an	integer.	It	is	the	the	reference	offset	minus	the	offset	of
			the	last	data	member.
			<index>	is	an	integer.	It	is	an	array	index.
			<member_name>	is	a	name	of	a	member	of	the	given	composite	type.
*/

static	int	getCompositeSubReference(
				void*								reference_address,	/*	Address	we	are	looking	for	*/
				ATTRIBUTES**	left_type,									/*	Attributes	of	type	we	are	looking	for	*/
				void*								structure_address,	/*	Address	of	struct	we	are	in	*/
				ATTRIBUTES*		A,																	/*	Attributes	of	current	struct	we	are	in	*/
				char*	reference_name												/*	destination	buffer	of	composite	subreference	*/
				)	{

				int			j,	m;
				long		offset;
				int			my_index[9];
				int			ret;
				int			size,	last_size;

				char*	rAddr	=	(char*)reference_address;
				char*	sAddr	=	(char*)structure_address;

				long	referenceOffset	=	(long)rAddr	-	(long)sAddr;

				//	selected	ATTRIBUTES	stucture	from	A	(singular)
				ATTRIBUTES*	Ai;


				if	(	referenceOffset	<	0)	{
								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Address	to	find	is	less	than	struct	address.\n")	;
								return	1;
				}

				//	Find	the	structure	member	that	corresponds	to	the	reference	address.
				//	If	name	is	empty,	we	have	failed.	
				Ai	=	findMember(A,	referenceOffset);

/******If	failed	to	find	member,	set	reference_name	to	offset	only	and	return	****/
				if	(Ai->name[0]	==	'\0')	{
								/*	If	we	fail	to	find	a	member	corresponding	to	the	reference	address,
											it	must	mean	that	the	ATTRIBUTES	don't	contain	a	description	for
											that	particular	member,	i.e.,	it	was	**'ed	out.	In	this	case,	we	can	only
											represent	the	reference_address	as	a	byte	offset	into	the	structure.
									*/
								if	(referenceOffset	==	0)	{
												reference_name[0]	=	'\0'	;
								}	else	if	(referenceOffset	>	0)	{
												snprintf(reference_name,	(size_t)256,	"	+	%ld"	,	referenceOffset);
								}	else	{
												return	1;	//	ERROR
								}
								return	0;
				}

/******************************************************************************/
				
/*	We	found	a	member	corresponding	to	the	reference	address,	so	print	it's	name.	*/
				snprintf(reference_name,	(size_t)256,	".%s",	Ai->name);

/*	If	the	referenced	member	variable	is	an	intrinsic	type	*/
				if	(Ai->type	!=	TRICK_STRUCTURED)	{

/*	If	the	reference	address	is	non-array	or	a	pointer,	return	reference_name	as	is	*/
								if((Ai->num_index	==	0)	||	(Ai->index[0].size	==	0))	{
												return	0;
								}

/*	else,	rAddr	is	pointing	to	an	array,	determine	its	dimensions	and	determine	
			the	element	pointed	to	by	rAddr.	Then	print	the	index	and	return	*/

								offset	=	(long)	rAddr	-	((long)	sAddr	+	Ai->offset);
								size	=	last_size	=	Ai->size;

								/*	Calculate	the	number	of	fixed	dimensions.	*/
								int	num_fixed_dims	=	0;
								for	(j	=	0;	j	<	Ai->num_index;	j++)	{
												if	(Ai->index[j].size	>	0)	{
																num_fixed_dims++;
												}
								}

								//	Calculate	the	array	indices	for	the	right-side.
								for	(j	=	num_fixed_dims	-	1;	j	>=	0;	j--)	{
												size	*=	Ai->index[j].size;
												if(!size)	{
														std::cerr	<<	"Checkpoint	Agent	"	<<	__FUNCTION__	<<	"	ERROR:	divide	by	zero	during	array	indices	calculation"	<<	std::endl;
														return	1;
												}
												my_index[j]	=	(int)	((offset	%	size)	/	last_size);
												offset	-=	last_size	*	my_index[j];
												last_size	=	size;
								}

								for	(j	=	0;	j	<	num_fixed_dims;	j++)	{
												size_t	len	=	strlen(reference_name);
												size_t	rem	=	(size_t)256	-	len;
												snprintf(&reference_name[len],	rem,	"[%d]",	my_index[j]);
								}
				return	0;
				}
/********	TRICK_STRUCTURED	****************************************************/
				/*	if	it	is	a	reference,	do	nothing	and	return	*/
				if	((Ai->mods	&	1)	==	1)	{	//	Ai->type	==	TRICK_STRUCTURED
								return	0;
				}
/*if	member	is	an	unarrayed	struct,	continue	to	call	getCompositeSubReference.*/
				if	(Ai->num_index	==	0)	{
								char	buf[256];
								ret	=	getCompositeSubReference(	rAddr,	left_type,	sAddr	+	Ai->offset,	(ATTRIBUTES	*)	Ai->attr,	buf);

								if	(ret	==	0)	{
												size_t	len	=	strlen(reference_name);
												size_t	rem	=	(size_t)256	-	len;
												snprintf(&reference_name[len],	rem,	"%s",	buf);
								}	else	{
												return	1;	//	ERROR.
								}
				return	0;
				}

/*****	If	the	member	is	not	a	pointer	do	nothing	and	return	*******************/
				if	(Ai->index[0].size	==	0)	{
								return	0;
				}

/***********	Member	is	an	arrayed	struct	**************************************/	

				offset	=	(long)	rAddr	-	((long)	sAddr	+	Ai->offset);
				size	=	last_size	=	Ai->size;

				/*	Calculate	the	indices	into	the	array	*/
				for	(j	=	Ai->num_index	-	1;	j	>=	0;	j--)	{
								if	(Ai->index[j].size	!=	0)	{
												size	*=	Ai->index[j].size;
								}
								my_index[j]	=	(int)	((offset	%	size)	/	last_size);
								offset	-=	last_size	*	my_index[j];
								last_size	=	size;
				}


				for	(j	=	0;	j	<	Ai->num_index;	j++)	{
								size_t	len	=	strlen(reference_name);
								size_t	rem	=	(size_t)256	-	len;
								snprintf(&reference_name[len],	rem,	"[%d]",	my_index[j]);
				}

				/*	if	left_type	specifies	the	current	member,	stop	here	*/
				if	(	(left_type	!=	NULL)	&&	(*left_type	!=	NULL)	&&	(Ai->attr	==	(*left_type)->attr))	{
								return	0;
				}	

/****	Go	find	the	subreference	for	the	arrayed	struct	member	and	append	*********/

				/*	get	the	offset	into	the	array	that	rAddr	points	to	*/
				offset	=	0;
				for	(j	=	0;	j	<	Ai->num_index;	j++)	{
						m	=	my_index[j];
						for(int	k	=	j	+	1;	m	&&	(k	<	Ai->num_index);	k++)	{
								m	*=	Ai->index[k].size;
						}
						offset	+=	m*Ai->size;
				}

				{
								char	buf[256];
								ret	=	getCompositeSubReference(	rAddr,	left_type,	sAddr	+	Ai->offset	+	offset,	(ATTRIBUTES	*)	Ai->attr,	buf);

								if	(ret	==	0)	{
												size_t	len	=	strlen(reference_name);
												size_t	rem	=	(size_t)256	-	len;
												snprintf(&reference_name[len],	rem,	"%s",	buf);
								}	else	{
												return	1;	//	ERROR
								}
				}

				return	0;
}


//	MEMBER	FUNCTION
//	Get	the	fully	qualified	varible	name	of	the	Allocation,	given	the	address.

std::string	Trick::ClassicCheckPointAgent::get_var_name(	void*	addr,
																																																									ATTRIBUTES*	A,
																																																									void*	struct_addr,
																																																									std::string	name,
																																																									ATTRIBUTES**	left_type)	{
				char	reference_name[256];
				int	ret;
				std::string	var_name;

				var_name	=	name;
				ret	=	getCompositeSubReference(	addr,	left_type,	struct_addr,	A,	reference_name	);

				if	(ret	==	0)	{
								var_name	+=	reference_name;
				}	else	{
										std::stringstream	ss;
										ss	<<	"Checkpoint	Agent	ERROR:	Unable	to	create	a	subreference	of	variable	\""	<<	name	<<	"\"."
													<<	std::endl;
										message_publish(MSG_ERROR,	ss.str().c_str()	);
				}
				return	(var_name);
}

//	MEMBER	FUNCTION
int	Trick::ClassicCheckPointAgent::restore(	std::istream*	checkpoint_stream)	{

				ChkPtParseContext*	context	=	new	ChkPtParseContext(	mem_mgr,	checkpoint_stream);
				int	status	=	0;

				if	(	CCP_parse(	context))	{
								status	=	1;
				}	else	if	((context->bad_declaration_count	>	0)	||
															(context->bad_assignment_count	>	0))	{
								std::stringstream	ss;
								ss	<<	"Checkpoint	Agent	ERROR:	"	<<	context->bad_declaration_count	<<	"	invalid	declaration(s)	"
											<<	"and	"	<<	context->bad_assignment_count	<<	"	invalid	assignment(s)."
											<<	std::endl;
								message_publish(MSG_ERROR,	ss.str().c_str()	);
								status	=	1;
				}

				if	(status)	{
								std::stringstream	ss;
								ss	<<	"Checkpoint	Agent	ERROR:	Checkpoint	restore	failed."
											<<	std::endl;
								message_publish(MSG_INFO,	ss.str().c_str()	);
				}
				delete	context	;
				return	(status);
}

//	MEMBER	FUNCTION
int	Trick::ClassicCheckPointAgent::is_nil_valued(	void*	address,
																																																		ATTRIBUTES*	attr,
																																																		int	curr_dim,
																																																		int	offset
																																																)	{

				char*	test_addr;
				int	remaining_dimensions	=	attr->num_index	-	curr_dim;

				/**	@par	Detailed	Description:	*/

				/**	@par
								If	we're	referencing	a	singleton	then	calculate	the	test-address
								from	the	(base)	address,	the	offset	and	the	data-type.	Then
								test	that	calculated	address	(with	the	given	type)	for	nil.
								If	it's	nil	return	1,	otherwise	return	0.
					*/
				if	(remaining_dimensions	==0)	{

								switch	(attr->type)	{
											case	TRICK_CHARACTER	:
											case	TRICK_UNSIGNED_CHARACTER	:
															test_addr	=	(char*)address	+	offset	*	sizeof(char);
															if	(*(char*)test_addr	==	'\0')	return(1);
															break;
											case	TRICK_BOOLEAN:
															test_addr	=	(char*)address	+	offset	*	sizeof(bool);
															if	(*(bool*)test_addr	==	false)	return(1);
															break;
											case	TRICK_WCHAR	:
															test_addr	=	(char*)address	+	offset	*	sizeof(wchar_t);
															if	(*(wchar_t*)test_addr	==	0)	return(1);
															break;
											case	TRICK_SHORT	:
											case	TRICK_UNSIGNED_SHORT	:
															test_addr	=	(char*)address	+	offset	*	sizeof(short);
															if	(*(short*)test_addr	==	0)	return(1);
															break;
											case	TRICK_INTEGER	:
											case	TRICK_UNSIGNED_INTEGER	:
															test_addr	=	(char*)address	+	offset	*	sizeof(int);
															if	(*(int*)test_addr	==	0)	return(1);
															break;
											case	TRICK_ENUMERATED	:
															if	((size_t)attr->size	==	sizeof(int))	{
																			test_addr	=	(char*)address	+	offset	*	sizeof(int);
																			if	(*(int*)test_addr	==	0)	return(1);
															}	else	if	((size_t)attr->size	==	sizeof(short))	{
																			test_addr	=	(char*)address	+	offset	*	sizeof(short);
																			if	(*(short*)test_addr	==	0)	return(1);
															}	else	{
																			return(-1);
															}
															break;
											case	TRICK_LONG	:
											case	TRICK_UNSIGNED_LONG	:
															test_addr	=	(char*)address	+	offset	*	sizeof(long);
															if	(*(long*)test_addr	==	0)	return(1);
															break;
											case	TRICK_FLOAT	:
															test_addr	=	(char*)address	+	offset	*	sizeof(float);
															if	(fpclassify(	*(float*)test_addr)	==	FP_ZERO)	return(1);
															break;
											case	TRICK_DOUBLE	:
															test_addr	=	(char*)address	+	offset	*	sizeof(double);
															if	(fpclassify(	*(double*)test_addr)	==	FP_ZERO)	return(1);
															break;
											case	TRICK_LONG_LONG	:
											case	TRICK_UNSIGNED_LONG_LONG	:
															test_addr	=	(char*)address	+	offset	*	sizeof(long	long);
															if	(*(long	long*)test_addr	==	0)	return(1);
															break;
											case	TRICK_BITFIELD	:
											case	TRICK_UNSIGNED_BITFIELD	:
															test_addr	=	(char*)address	+	offset	*	(size_t)attr->size;
															if	(attr->size	==	sizeof(int))	{
																			if	(*(int*)test_addr	==	0)	return(1);
															}	else	if	(attr->size	==	sizeof(short))	{
																			if	(*(short*)test_addr	==	0)	return(1);
															}	else	if	(attr->size	==	sizeof(char))	{
																			if	(*(char*)test_addr	==	0)	return(1);
															}	else	{
																			message_publish(MSG_ERROR,	"Checkpoint	Agent	INTERNAL	ERROR:\n"
																																														"Unhandled	bitfield	struct	size	(%d)	in	bitfield	assignment.\n",	attr->size)	;
																			return(-1);
															}
															break;
											case	TRICK_FILE_PTR	:
															//	FIXME
															return(0);
															break;
											case	TRICK_STRING	:
															test_addr	=	(char*)address	+	offset	*	sizeof(void*);
															if	(*(std::string*)test_addr	==	"")	return(1);
															break;
											case	TRICK_STL	:
															//	Can't	test	properly,	always	return	0	to	indicate	the	STL	is	not	empty.
															return(0);
															break;
											default	:
															message_publish(MSG_ERROR,	"Checkpoint	Agent	file	%s:	Unhandled	Type	(%d).\n",	__FILE__,	attr->type)	;
															return(-1);
															break;
								}
								return(0);

				/**	@par
								If	on	the	otherhand	we	are	referencing	an	array,	then	we	must	consider	two	cases.
					*/
				}	else	if	(remaining_dimensions	>	0)	{
							int	curr_dim_size;
							curr_dim_size	=	attr->index[curr_dim].size	;

							/**		@par
												If	the	array	is	unconstrained	(i.e.,	it's	a	pointer)	then	we	just	need	to	check
												whether	the	pointer	is	NULL.
								*/
							if	(	curr_dim_size	==	0)	{
											test_addr	=	((char*)address)	+	(offset*sizeof(void	*));
											if	(*(char**)test_addr	==	NULL)	return(1);

							/**	@par
											If	the	array	(at	this	dimension)	is	constrained	(i.e.,	it's	a	fixed	array	)
											then	it	is	nil	if	and	only	if	each	of	it's	sub-elements	(at	the	next	dimension,
											which	can	themselves	be	arrays)	are	nil.	So,	for	each	of	the	elements	in	current
											dimension,	we	recursively	call	is_nil_valued()	on	each	of	the	sub-elements	to
											find	out	whether	this	array	is	nil	valued	and	return	the	result.
											*/
							}	else	{
											int	ii;
											int	zerotest;

											for	(ii=0;	ii	<	curr_dim_size;	ii++)	{
															zerotest	=	is_nil_valued(	address,	attr,	curr_dim+1,	offset*curr_dim_size+ii);
															if	(!(zerotest	==	1))	return(zerotest);
											}
											return(1);
							}

				}	else	{
								return(-1);
				}
				return(0);
}

//	STATIC	FUNCTION
static	void	write_quoted_str(	std::ostream&	os,	const	char*	s)	{
				int	ii;
				int	len	=	strlen(s);
				os	<<	"\""	;
				for	(ii=0	;	ii<len	;	ii++)	{
								switch	((s)[ii])	{
								case	'\n':	os	<<	"\\n";	break;
								case	'\t':	os	<<	"\\t";	break;
								case	'\b':	os	<<	"\\b";	break;
								case	'\"':	os	<<	"\\\"";	break;
								default		:	os	<<	s[ii]	;	break;
								}
				}
				os	<<	"\""	;
}

void	Trick::ClassicCheckPointAgent::write_singleton(	std::ostream&	chkpnt_os,	void*	address,	ATTRIBUTES*	attr,	int	offset	)	{

				void*	src_addr;

				switch(attr->type)	{
								case	TRICK_VOID:
												chkpnt_os	<<	std::endl	<<	"//	ERROR	-	VOID	data	type	cannot	be	checkpointed."	<<	std::endl;
												message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	VOID	data	type	cannot	be	checkpointed.\n")	;
								break;
								case	TRICK_UNSIGNED_CHARACTER:
												src_addr	=	(char*)address	+	offset	*	sizeof(unsigned	char);
												chkpnt_os	<<	std::dec	<<	(int)*(unsigned	char*)src_addr	;
								break;
								case	TRICK_BOOLEAN:
												src_addr	=	(char*)address	+	offset	*	sizeof(bool);
												if	(*(bool*)src_addr)	{
																chkpnt_os	<<	"true"	;
												}	else	{
																chkpnt_os	<<	"false"	;
												}
								break;
								case	TRICK_CHARACTER:
												src_addr	=	(char*)address	+	offset	*	sizeof(char);
												if	(isprint(	*(char*)src_addr)	)	{
																chkpnt_os	<<	"'"	<<	*(char*)src_addr	<<	"'"	;
												}	else	{
																unsigned	int	ch	=	*(unsigned	char*)src_addr;
																		chkpnt_os	<<	"'\\x"	<<	std::hex	<<	ch	<<	"'"	;
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
																				std::cerr	<<	__FUNCTION__	<<	":	enumeration	size	error."	<<	std::endl;
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
																if	(hexfloat_checkpoint)	{
																				double	temp_dbl	=	*(float*)src_addr;
																				unsigned	char*	byte_p	=	(unsigned	char*)&temp_dbl;
																				chkpnt_os	<<	"0g";
																				for	(int	ii=sizeof(double)-1	;	ii>=	0	;	ii--)	{
																								chkpnt_os	<<	std::hex	<<	std::setw(2)	<<	std::setfill('0')	<<	(unsigned	int)byte_p[ii];
																				}
																}	else	{
																				chkpnt_os	<<	std::setprecision(8)	<<	*(float*)src_addr;
																}
												}	else	{
																chkpnt_os	<<	"NAN";
												}
												break;
								case	TRICK_DOUBLE:
												src_addr	=	(char*)address	+	offset	*	sizeof(double);
												if	(fpclassify(	*(double*)src_addr)	!=	FP_NAN)	{
																if	(hexfloat_checkpoint)	{
																				double	temp_dbl	=	*(double*)src_addr;
																				unsigned	char*	byte_p	=	(unsigned	char*)&temp_dbl;
																				chkpnt_os	<<	"0g";
																				for	(int	ii=sizeof(double)-1	;	ii>=	0	;	ii--)	{
																								chkpnt_os	<<	std::hex	<<	std::setw(2)	<<	std::setfill('0')	<<	(unsigned	int)byte_p[ii];
																				}
																}	else	{
																				chkpnt_os	<<	std::setprecision(16)	<<	*(double*)src_addr;
																}
												}	else	{
																chkpnt_os	<<	"NAN";
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
												write_quoted_str(chkpnt_os,	(*(std::string*)src_addr).c_str());
												break;
								case	TRICK_OPAQUE_TYPE:
												chkpnt_os	<<	std::endl	<<	"//	ERROR	-	OPAQUE	data	type	("	<<	attr->type_name	<<	")	cannot	be	checkpointed."	<<	std::endl;
												message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	OPAQUE	data	type	(%s)	cannot	be	checkpointed.\n",	attr->type_name)	;
								break;
								default:
												chkpnt_os	<<	"0";
												message_publish(MSG_ERROR,	"Checkpoint	Agent	file	%s:	Unhandled	Type	(%d).\n",	__FILE__,	attr->type)	;
												break;
				}
}

//	MEMBER	FUNCTION
//	Get	the	variable-name	associated	with	the	given	variable-address	and	data-type.
//	If	no	data-type	is	specified,	(i.e.,	if	attr	==	NULL),	then	the	name	will	refer
//	to	the	(inner-most)	primitive	data-type	associated	with	the	address.

//	attr	and	curr_dim	together	specify	the	expected	data-type.
//	If	(attr	==	NULL)	then	type	checking	can't	and	won't	be	performed.

std::string	Trick::ClassicCheckPointAgent::
				ref_string_from_ptr(	void*	pointer,	ATTRIBUTES*	attr,	int	curr_dim)	{

				std::string	reference_string;

				if	((attr	!=	NULL)	&&	((curr_dim	>=	attr->num_index)	||	(attr->index[curr_dim].size	!=	0)))	{
								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	ref_string_from_ptr	called	with	a	non-pointer	type.\n")	;
				}

				if	(pointer	==	NULL)	{
								reference_string	=	"NULL";
				}	else	{
								ALLOC_INFO	*alloc_info;

								/**	Find	the	allocation	that	contains	the	pointer-address.	*/
								alloc_info	=	mem_mgr->get_alloc_info_of(	pointer);

								if	(alloc_info	!=	NULL)	{
												int	alloc_elem_size;
												int	alloc_elem_index;
												int	misalignment;

												alloc_elem_size	=	alloc_info->size;
												alloc_elem_index	=	(int)	(((long)	pointer	-	(long)	alloc_info->start)	/	alloc_elem_size);
												misalignment	=	(int)	(((long)	pointer	-	(long)	alloc_info->start)	%	alloc_elem_size);

												//	If	type-checking	AND	the	type	specifiers	match	AND		the	type	we're	looking	for
												//	is	either	not	structured	or	if	it	is,	the	attr-list	that	describes	the	contents
												//	of	the	structure	is	the	same.

												if	(	(attr	!=	NULL)	&&	(attr->type	==	alloc_info->type)	&&
																	(	(attr->type	!=	TRICK_STRUCTURED)	||	(attr->attr	==	alloc_info->attr)))	{

																int	ii;
																int	n_l_ptrs,	n_r_ptrs;

																//	Calculate	the	number	of	pointers	(asterisks)	on	the	left	side	of	the	assignment.
																n_l_ptrs	=	attr->num_index	-	curr_dim;

																//	Calculate	the	number	of	pointers	(asterisks)	on	the	right	side	of	the	assignment.
																n_r_ptrs	=	0;
																for	(ii=0	;	ii	<alloc_info->num_index	;	ii++)	{
																				if	(alloc_info->index[ii]	==	0)	n_r_ptrs++;
																}

																if	(n_l_ptrs	!=	(n_r_ptrs	+	1))	{
																				reference_string	=	"NULL	/*ERROR:	#	asterisks	disagree.*/";
																}	else	{
																				if	(misalignment	==	0)	{
																								std::stringstream	workss;
																								if	(alloc_info->name	!=	NULL)	{
																												workss	<<	"&"	<<	alloc_info->name;
																												if	(alloc_info->num_index	!=	0)	{
																																workss	<<	"["	<<	alloc_elem_index	<<	"]";
																												}
																												reference_string	=	workss.str();
																								}	else	{
																												std::stringstream	ss;
																												ss	<<	"Checkpoint	Agent	ERROR:	The	name	of	the	allocation	at	"	<<	alloc_info->start	<<	"	is	NULL."
																															<<	"Therefore,	Trick::ClassicCheckPointAgent::ref_string_from_ptr()	can't	generate	a	textual	reference	to	it."
																															<<	std::endl;
																												message_publish(MSG_ERROR,	ss.str().c_str()	);
																												reference_string	=	"ERROR	-	Allocation	name	is	NULL";
																								}

																				}	else	{
																								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Badly	aligned	pointer.\n"
																																																			"			It	is	not	aligned	with	the	data	object\n"
																																																			"			(of	the	same	type)	into	which	it	is	pointing.\n")	;
																								reference_string	=	"ERROR	-	Badly	aligned	pointer";
																				}
																}
												}	else	if	(alloc_info->type	==	TRICK_STRUCTURED)	{
																//	The	type	specifications	don't	match,	but	the	right-side	is	structured,
																//	so	we	apparently	the	matching	type	is	buried	in	the	right	hand	side	structure.

																if	(alloc_info->name	!=	NULL)	{
																				std::string	rightside;
																				std::stringstream	element_name;
																				element_name	<<	"&"	<<	alloc_info->name;
																				if	(alloc_info->num_index	!=	0)	{
																								element_name	<<	'['	<<	alloc_elem_index	<<	']';
																				}
																				rightside	=	get_var_name(	pointer,
																																														alloc_info->attr,
																																														(char	*)	alloc_info->start	+	(alloc_elem_index	*	alloc_info->size),
																																														element_name.str(),
																																														&attr
																																												);
																				reference_string	=	rightside;
																}	else	{
																				std::stringstream	ss;
																				ss	<<	"Checkpoint	Agent	ERROR:	The	name	of	the	allocation	at	"	<<	alloc_info->start	<<	"	is	NULL."
																							<<	"Therefore,	Trick::ClassicCheckPointAgent::ref_string_from_ptr()	can't	generate	a	textual	reference	to	it."
																							<<	std::endl;
																				message_publish(MSG_ERROR,	ss.str().c_str()	);
																				reference_string	=	"ERROR	-	Allocation	name	is	NULL";
																}
												}	else	{	//	The	type	specifications	don't	match,	and	the	right	hand	side	is	not	structured

																if	(attr	!=	NULL)	{
																				const	char*	left_type_spec		=	trickTypeCharString(attr->type,	"");
																				const	char*	right_type_spec	=	trickTypeCharString(alloc_info->type,	alloc_info->user_type_name);
																				message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Type	mismatch.	Type	specifications	disagree.\n"
																																															"			The	left	side	type	specifier	is	\"%s\"	but	the	right	side	is	\"%s\".\n",
																																				left_type_spec,	right_type_spec)	;
																				reference_string	=	"ERROR	-	Type	specifications	disagree";
																}	else	{

																				if	(misalignment	==	0)	{
																								std::stringstream	workss;

																								if	(alloc_info->name	!=	NULL)	{
																												workss	<<	"&"	<<	alloc_info->name;
																												if	(alloc_info->num_index	!=	0)	{
																																workss	<<	"["	<<	alloc_elem_index	<<	"]";
																												}
																												reference_string	=	workss.str();
																								}	else	{
																												std::stringstream	ss;
																												ss	<<	"Checkpoint	Agent	ERROR:	The	name	of	the	allocation	at	"	<<	alloc_info->start	<<	"	is	NULL."
																															<<	"Therefore,	Trick::ClassicCheckPointAgent::ref_string_from_ptr()	can't	generate	a	textual	reference	to	it."
																															<<	std::endl;
																												message_publish(MSG_ERROR,	ss.str().c_str()	);
																												reference_string	=	"ERROR	-	Allocation	name	is	NULL";
																								}

																				}	else	{
																								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Badly	aligned	pointer.\n"
																																																			"			It	is	not	aligned	with	the	data	object\n"
																																																			"			(of	the	same	type)	into	which	it	is	pointing.\n")	;
																								reference_string	=	"ERROR	-	Badly	Aligned	Pointer";
																				}
																}

												}
								}	else	if	((attr	!=	NULL)	&&	((curr_dim	+	1)	==	attr->num_index))	{

												if	(attr->type	==	TRICK_CHARACTER)	{
																std::stringstream	ss;
																write_quoted_str(	ss,	(const	char*)pointer);
																reference_string	=	ss.str();
												}	else	if	(attr->type	==	TRICK_WCHAR)	{
																message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	TRICK_WCHAR	not	fully	supported	yet.\n")	;
																reference_string	=	"ERROR:	TRICK_WCHAR	not	fully	supported	yet.";
												}	else	{
																std::string	lname	=	left_side_name();
																message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Pointer	<%p>	in	\"%s\"	is	not	in	Trick	managed	memory\n"
																																											"nor	is	it	a	character	pointer.\n",	pointer,	lname.c_str())	;
																reference_string	=	"ERROR	-	Pointer	not	in	Trick	managed	memory";
												}

								}	else	{
												std::string	lname	=	left_side_name();
												message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	Pointer	<%p>	in	\"%s\"	is	not	in	Trick	managed	memory\n"
																																							"nor	is	it	a	character	pointer.\n",	pointer,	lname.c_str())	;
												reference_string	=	"ERROR	-	Pointer	not	in	Trick	managed	memory";
								}
				}
				return(	reference_string);
}

//	MEMBER	FUNCTION
void	Trick::ClassicCheckPointAgent::write_rvalue(	std::ostream&	chkpnt_os,	void*	address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

				//	If	the	variable	that	we	are	pointing	to	is	Un-arrayed
				if	(curr_dim	==	attr->num_index)	{

								write_singleton(	chkpnt_os,	address,	attr,	offset);

				//	If	the	variable	that	we	are	pointing	to	is	Arrayed
				}	else	if	(curr_dim	<	attr->num_index)	{

								//	If	the	variable	is	a	pointer
								if	(attr->index[curr_dim].size	==	0)	{
												std::string	ref_string;

												void*	pointer	=	*(void**)((char*)address	+	offset	*	sizeof(void*));

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

																				int	array_len	=	attr->index[curr_dim].size;
																				int	ii	=	array_len	-	1;
																				src_addr	=	(char*)address	+	offset	*	array_len	*	sizeof(char);
																				if	(src_addr[ii]	!=	'\0')	{
																								use_quoted_string	=	0;
																				}
																				ii--;
																				while	(	use_quoted_string	&&	(ii	>=	0)	)	{
																								use_quoted_string	=	isprint(	src_addr[ii]);
																								ii--;
																				}
																}


																//	This	is	necessary	due	to	https://github.com/nasa/trick/issues/1399
																//	If	there	is	a	string	array	where	the	memory	is	not	initialized,	the	code	above
																//	can	incorrectly	decide	to	write	it	as	a	quoted	string	when	it	should	be	an
																//	empty	array,	causing	a	segfault	when	the	checkpoint	is	reloaded.
																if	(attr->index[curr_dim	+	1].size	==	0	&&	((curr_dim	+	1)	!=	attr->num_index))	{
																				use_quoted_string	=	0;
																}	


																if	((attr->type	==	TRICK_CHARACTER)	&&	use_quoted_string)		{

																				write_quoted_str(chkpnt_os,	src_addr);

																}	else	{

																				int	ii,	jj;
																				int	array_len;

																				//	Determine	the	number	of	array	elements	we	need	to	print	out
																				//	to	get	all	of	the	non-zero	values.

																				array_len	=	attr->index[curr_dim].size	;

																				chkpnt_os	<<	"\n";

																				for	(ii=0	;	ii	<	curr_dim+1	;	ii++)	{
																								chkpnt_os	<<	"				";
																				}
																				chkpnt_os	<<	"{";

																				for	(ii	=	0;	ii	<	array_len	;	ii++	)	{

																								if	(ii	>	0)	{
																												chkpnt_os	<<	",	";

																												//	Conditionally	line-break	and	indent.
																												if	((	(ii+1)	%	array_elements_per_line[attr->type])	==	0	)	{
																																//	Line-break.
																																chkpnt_os	<<	"\n";
																																//	Indent.
																																for	(jj=0	;	jj	<	curr_dim+1	;	jj++)	{
																																				chkpnt_os	<<	"				";
																																}
																																chkpnt_os	<<	"	";
																												}
																								}
																								write_rvalue(	chkpnt_os,	address,	attr,	curr_dim	+	1,	offset	*	attr->index[curr_dim].size	+	ii);
																				}
																				chkpnt_os	<<	"}";

																}

												}	else	{	//	Not	the	final	fixed	dimension.

																int	ii;

																chkpnt_os	<<	"\n";
																for	(ii=0	;	ii	<	curr_dim+1	;	ii++)	{
																				chkpnt_os	<<	"				";
																}
																chkpnt_os	<<	"{";

																for	(ii=0	;	ii<	attr->index[curr_dim].size	;	ii++)	{
																				if	(ii	>	0)	{
																								chkpnt_os	<<	",";
																				}
																				write_rvalue(	chkpnt_os,	address,	attr,	curr_dim	+	1,	offset	*	attr->index[curr_dim].size	+	ii);
																}

																chkpnt_os	<<	"\n";

																for	(ii=0	;	ii	<	curr_dim+1	;	ii++)	{
																				chkpnt_os	<<	"				"	;
																}
																chkpnt_os	<<	"}";
												}
								}

				}	else	{
								chkpnt_os	<<	"/*ERROR*/";

								message_publish(MSG_ERROR,	"Checkpoint	Agent	ERROR:	The	specified	current	dimension	\"%d\"	is	greater\n"
																																			"than	the	number	of	dimensions	specified	in	the	type	ATTRIBUTES.\n",	curr_dim)	;

								return;
				}
}

//	MEMBER	FUNCTION
//	Create	an	assignment	statement.
void	Trick::ClassicCheckPointAgent::assign_rvalue(std::ostream&	chkpnt_os,	void*	address,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset)	{

				std::string	lname	=	left_side_name();

				if	(!output_perm_check(attr))	{
								if	(debug_level)	{
												message_publish(MSG_DEBUG,	"Checkpoint	Agent	INFO:	No	assignment	generated	for	\"%s\"	"
																																							"because	its	io	specification	does	not	allow	it.\n",	lname.c_str())	;
								}
								return;
				}

				if	((reduced_checkpoint	&&	is_nil_valued(	(void*)address,	attr,	curr_dim,	offset	)	)	)	{
								if	(debug_level)	{
												message_publish(MSG_DEBUG,	"Checkpoint	Agent	INFO:	No	assignment	generated	for	\"%s\"	"
																																							"because	its	value	is	nil	and	the	reduced_checkpoint	flag	is	set.\n",	lname.c_str())	;
								}
								return;
				}

				if	(debug_level)	{
								message_publish(MSG_DEBUG,	"Checkpoint	Agent	INFO:	Generating	assignment	for	[%p]	%s.\n",(void*)address,	lname.c_str())	;
				}

				if	(!input_perm_check(attr))	{
								chkpnt_os	<<	"/*	OUTPUT-ONLY:	";
				}
				if	(	attr->type	==	TRICK_STL	)	{
								chkpnt_os	<<	"//	STL:	"	<<	lname	;
				}	else	{
								chkpnt_os	<<	lname	<<	"	=	";
								write_rvalue(	chkpnt_os,	(void*)address,	attr,	curr_dim,	offset);
								chkpnt_os	<<	";";
				}
				if	(!input_perm_check(attr))	{
								chkpnt_os	<<	"*/";
				}
				chkpnt_os	<<	std::endl;
				chkpnt_os.flush();

}
