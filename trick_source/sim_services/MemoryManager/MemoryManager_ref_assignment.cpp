#include	"trick/MemoryManager.hh"
#include	"trick/bitfield_proto.h"
#include	"trick/vval.h"
#include	"trick/wcs_ext.h"
#include	<limits.h>
#include	<sstream>
#include	<udunits2.h>
#include	<string.h>

int	Trick::MemoryManager::assign_recursive(void*	base_addr,	ATTRIBUTES*	attr,	int	curr_dim,	int	offset,	V_TREE*	v_tree,	cv_converter*	cf)	{

			char*	assign_addr;
			int	remaining_dimensions	=	attr->num_index	-	curr_dim;

			if	(	remaining_dimensions	==	0	)	{

						switch	(attr->type)	{

											case	TRICK_CHARACTER	:
											case	TRICK_UNSIGNED_CHARACTER	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(char);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(char*)assign_addr	=	vval_char(v_tree->v_data);
															}	else	{
																			*(char*)assign_addr	=	'\0';
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(char*)"	<<	(void*)assign_addr
																													<<	"	=	";
																			if	(isprint(*(char*)assign_addr))	{
																							std::cout	<<	*(char*)assign_addr;
																			}	else	{
																							std::cout	<<	(int)*(char*)assign_addr;
																			}
																			std::cout	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_WCHAR	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(wchar_t);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(wchar_t*)assign_addr	=	vval_char(v_tree->v_data);
															}	else	{
																			*(wchar_t*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(wchar_t*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(wchar_t*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_SHORT	:
											case	TRICK_UNSIGNED_SHORT	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(short);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(short	*)assign_addr	=	vval_short(v_tree->v_data);
															}	else	{
																			*(short	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(short*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(short*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_INTEGER	:
											case	TRICK_UNSIGNED_INTEGER	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(int);
															if	(v_tree	&&	v_tree->v_data)	{
																			int	input_value;
																			input_value	=	vval_int(v_tree->v_data);
																			if	(cf	==	NULL)	{
																							*(int	*)assign_addr	=	input_value;
																			}	else	{
																							*(int	*)assign_addr	=	(int)cv_convert_double(cf,	(double)input_value)	;
																			}
															}	else	{
																			*(int	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(int*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(int*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_BOOLEAN	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(bool);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(bool	*)assign_addr	=	(vval_short(v_tree->v_data)!=0);
															}	else	{
																			*(bool	*)assign_addr	=	false;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(bool*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(bool*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_ENUMERATED	:
															if	(v_tree	&&	v_tree->v_data)	{
																			if	((size_t)attr->size	==	sizeof(int))	{
																							assign_addr	=	(char*)base_addr	+	offset	*	sizeof(int);
																							*(int	*)assign_addr	=	vval_int(v_tree->v_data);
																							if	(debug_level)	{
																											std::cout	<<	std::endl	<<	"Assignment	(Enum):	*(int*)"	<<	(void*)assign_addr
																																					<<	"	=	"	<<	*(int*)assign_addr	<<	";"	<<	std::endl;
																											std::cout.flush();
																							}
																			}	else	if	((size_t)attr->size	==	sizeof(short))	{
																							assign_addr	=	(char*)base_addr	+	offset	*	sizeof(short);
																							*(short	*)assign_addr	=	vval_short(v_tree->v_data);
																							if	(debug_level)	{
																											std::cout	<<	std::endl	<<	"Assignment	(Enum):	*(short*)"	<<	(void*)assign_addr
																																					<<	"	=	"	<<	*(short*)assign_addr	<<	";"	<<	std::endl;
																											std::cout.flush();
																							}
																			}	else	if	((size_t)attr->size	==	sizeof(char))	{
																							assign_addr	=	(char*)base_addr	+	offset	*	sizeof(char);
																							*(char	*)assign_addr	=	vval_char(v_tree->v_data);
																							if	(debug_level)	{
																											std::cout	<<	std::endl	<<	"Assignment	(Enum):	*(char*)"	<<	(void*)assign_addr
																																					<<	"	=	"	<<	*(char*)assign_addr	<<	";"	<<	std::endl;
																											std::cout.flush();
																							}
																			}	else	{
																							std::stringstream	message;
																							message	<<	"Enumeration	of	size	"	<<	attr->size	<<	"	is	not	supported.";
																							emitError(message.str());
																			}
															}	else	{
																			emitError("v_tree	data	appears	to	be	corrupted.");
															}
															break;
											case	TRICK_LONG	:
											case	TRICK_UNSIGNED_LONG	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(long);
															if	(v_tree	&&	v_tree->v_data)	{
																			long	input_value;
																			input_value	=	vval_long(v_tree->v_data);
																			if	(cf	==	NULL)	{
																							*(long	*)assign_addr	=	input_value;
																			}	else	{
																							*(long	*)assign_addr	=	(long)cv_convert_double(cf,	(double)input_value)	;
																			}
															}	else	{
																			*(long	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(long*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(long*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_FLOAT	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(float);
															if	(v_tree	&&	v_tree->v_data)	{
																			float	input_value;
																			input_value	=	vval_float(v_tree->v_data);
																			if	(cf	==	NULL)	{		//	There	is	no	units	conversion.
																							*(float	*)assign_addr	=	input_value;
																			}	else	{	//	There	is	units	conversion.
																							*(float	*)assign_addr	=	(float)cv_convert_double(cf,	(double)input_value)	;
																			}
															}	else	{
																			*(float	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(float*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(float*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_DOUBLE	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(double);
															if	(v_tree	&&	v_tree->v_data)	{
																			double	input_value;
																			input_value	=	vval_double(v_tree->v_data);
																			if	(cf	==	NULL)	{
																							*(double	*)assign_addr	=	input_value;
																			}	else	{
																							*(double	*)assign_addr	=	cv_convert_double(cf,	input_value)	;
																			}
															}	else	{
																			*(double	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(double*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(double*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_LONG_LONG	:
											case	TRICK_UNSIGNED_LONG_LONG	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(long	long);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(long	long	*)assign_addr	=	vval_longlong(v_tree->v_data);
															}	else	{
																			*(long	long	*)assign_addr	=	0;
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(long	long*)"	<<	(void*)assign_addr
																													<<	"	=	"	<<	*(long	long*)assign_addr	<<	";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_BITFIELD	:
											case	TRICK_UNSIGNED_BITFIELD	:	{
																			int	input_value;
																			assign_addr	=	(char*)base_addr	+	offset	*	(size_t)attr->size;
																			if	(v_tree	&&	v_tree->v_data)	{
																							input_value	=	vval_int(v_tree->v_data);
																			}	else	{
																							input_value	=	0;
																			}
																			if	(attr->size	==	sizeof(int))	{
																							*(unsigned	int*)assign_addr	=	insert_bitfield_any(
																											*(unsigned	int*)assign_addr,	input_value,	attr->size,	attr->index[0].start,	attr->index[0].size);
																			}	else	if	(attr->size	==	sizeof(short))	{
																							*(unsigned	short*)assign_addr	=	insert_bitfield_any(
																											*(unsigned	short*)assign_addr,	input_value,	attr->size,	attr->index[0].start,	attr->index[0].size);
																			}	else	if	(attr->size	==	sizeof(char))	{
																							*(unsigned	char*)assign_addr	=	insert_bitfield_any(
																											*(unsigned	char*)assign_addr,	input_value,	attr->size,	attr->index[0].start,	attr->index[0].size);
																			}	else	{
																							std::stringstream	message;
																							message	<<	"Unhandled	bitfield	struct	size	("	<<	attr->size	<<	")	in	bitfield	assignment.";
																							emitError(message.str());
																			}
																			if	(debug_level)	{
																							std::cout	<<	std::endl	<<	"Assignment:	"
																																	<<	"Within	the	"	<<	attr->size	<<	"	byte	struct	at	"	<<	(void*)assign_addr
																																	<<	",	the	bitfield	["<<	attr->index[0].start	<<	".."
																																	<<	attr->index[0].start	+	attr->index[0].size	-	1
																																	<<	"]	=	"	<<	input_value	<<	";"
																																	<<	std::endl;
																							std::cout.flush();
																			}
															}	break;
											case	TRICK_FILE_PTR	:
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	TRICK_FILE_PTR	assignments	not	yet	implemented."
																													<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_VOID_PTR	:
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	TRICK_VOID_PTR	assignments	not	yet	implemented."
																													<<	std::endl;
																			std::cout.flush();
															}
															break;
											case	TRICK_STRING	:
															assign_addr	=	(char*)base_addr	+	offset	*	sizeof(std::string);
															if	(v_tree	&&	v_tree->v_data)	{
																			*(std::string*)assign_addr	=	vval_string(v_tree->v_data);
															}	else	{
																			*(std::string*)assign_addr	=	"";
															}
															if	(debug_level)	{
																			std::cout	<<	std::endl	<<	"Assignment:	*(std::string)"	<<	(void*)assign_addr
																													<<	"	=	\""	<<	*(std::string*)assign_addr	<<	"\";"	<<	std::endl;
																			std::cout.flush();
															}
															break;
											default:
															std::stringstream	message;
															message	<<	"Unhandled	Type	("	<<	attr->type	<<	")	in	assignment.";
															emitError(message.str());
															return	(1);
															break;
						}

			}	else	if	(	remaining_dimensions	>	0	)	{

							int	size_of_curr_dim	=	attr->index[curr_dim].size	;

							if	(	size_of_curr_dim	==	0)	{	//	the	remaining	dimensions	are	pointer	dimensions.

											assign_addr	=	(char*)base_addr	+	offset	*	sizeof(void*);

											if	(v_tree	&&	v_tree->v_data)	{

															if	((remaining_dimensions	==	1)	&&	(v_tree->v_data->type	==	TRICK_STRING))	{
																			*(char**)assign_addr	=	mm_strdup(	vval_string(v_tree->v_data));
															}	else	{
																			*(void**)assign_addr	=	vval_voidp(v_tree->v_data);
															}

											}	else	{
															*(void**)assign_addr	=	0;
											}

											if	(debug_level)	{
															std::cout	<<	std::endl	<<	"Assignment:	*(void**)"	<<	(void*)assign_addr
																									<<	"	=	"	<<	*(void**)assign_addr	<<	";"	<<	std::endl;
															std::cout.flush();
											}

							}	else	{	//	next	dimension	is	fixed.

											if	((attr->type	==	TRICK_CHARACTER)	&&
															(remaining_dimensions	==	1)	&&
															(v_tree)	&&
															(v_tree->v_data)
														)	{

															assign_addr	=	(char*)base_addr	+	offset	*	size_of_curr_dim	*	sizeof(char);

															if	((v_tree->v_data->type	==	TRICK_STRING)	&&
																			(v_tree->v_data->value.cp	!=	NULL))	{

																			int	rhs_len	=	(int)strlen(v_tree->v_data->value.cp)	+	1;
																			if	(rhs_len	<=	size_of_curr_dim	)	{
																							strcpy((char*)assign_addr,	v_tree->v_data->value.cp);
																			}	else	{
																							emitError("Memory	Manager:	char	array	is	too	small	for	the	attempted	string	assignment.");
																							return	(1);
																			}
															}	else	{
																			*(char*)assign_addr	=	'\0';
															}

											}	else	if	(	(attr->type	==	TRICK_WCHAR)	&&
																							(remaining_dimensions	==	1))	{

															assign_addr	=	(char*)base_addr	+	offset	*	size_of_curr_dim	*	sizeof(wchar_t);

															if	((v_tree)	&&
																			(v_tree->v_data->type	==	TRICK_WSTRING)	&&
																			(v_tree->v_data->value.wcp	!=	NULL))	{

																							int	rhs_len	=	(int)wcslen(v_tree->v_data->value.wcp)	+	1;
																							if	(rhs_len	<=	size_of_curr_dim	)	{
																											wcscpy((wchar_t*)assign_addr,	v_tree->v_data->value.wcp);
																							}	else	{
																											std::stringstream	message;
																											message	<<	"wchar_t	array	at	["	<<	(void*)assign_addr
																																			<<	"]	is	to	small	for	the	attempted	string	assignment."	;
																											emitError(message.str());
																											return	(1);
																							}
															}	else	if	((v_tree)	&&
																										(v_tree->v_data->type	==	TRICK_STRING)	&&
																										(v_tree->v_data->value.cp	!=	NULL))	{

																							int	rhs_len	=	(int)ncs_to_wcs_len(v_tree->v_data->value.cp)	+	1;
																							if	(rhs_len	<=	size_of_curr_dim	)	{
																											ncs_to_wcs(	v_tree->v_data->value.cp,	(wchar_t*)assign_addr,	rhs_len);
																							}	else	{
																											std::stringstream	message;
																											message	<<	"wchar_t	array	at	["	<<	(void*)assign_addr
																																			<<	"]	is	too	small	for	the	attempted	string	assignment."	;
																											emitError(message.str());
																											return	(1);
																							}
															}	else	{
																			*(wchar_t*)assign_addr	=	(wchar_t)	NULL;
															}

											}	else	{
															int	ii;
															V_TREE*	curr_vt_node;
															if	(v_tree)	{
																			curr_vt_node	=	v_tree->down;
															}	else	{
																			curr_vt_node	=		NULL;
															}

															for	(ii=0;	ii	<	size_of_curr_dim;	ii++)	{
																			int	ret;
																			ret	=	assign_recursive	(base_addr,
																																											attr,
																																											curr_dim+1,
																																											offset	*	size_of_curr_dim	+	ii,
																																											curr_vt_node,
																																											cf);

																			if	(ret	!=0)	{
																							return(1);
																			}

																			if	(curr_vt_node)	{
																					curr_vt_node	=	curr_vt_node->next;
																			}
															}
											}
							}
			}	else	{
								emitError("This	is	bad.	In	assign_recursive(),	remaining_dimensions	is	negative.");
								return	(1);
			}
			return	(0);
}

#ifdef	TRICK_VER
#include	"trick/UdUnits.hh"
#else
static	ut_system	*	u_system	;
#endif
ut_system	*	Trick::MemoryManager::get_unit_system()	{
#ifdef	TRICK_VER
				return	Trick::UdUnits::get_u_system()	;
#else
				/*	Initialize	the	udunits-2	library	*/
				ut_set_error_message_handler(ut_ignore)	;
				if(	(u_system	=	ut_read_xml(	NULL	))	==	NULL	)	{
								std::cerr	<<	"Error	initializing	udunits-2	unit	system"	<<	std::endl	;
								return	-1	;
				}
				ut_set_error_message_handler(ut_write_to_stderr)	;
				return	u_system	;
#endif
}

//	MEMBER	FUNCTION
int	Trick::MemoryManager::ref_assignment(	REF2*	R,	V_TREE*	V)	{

				int	ret	=	0;
				cv_converter	*	cf	=	NULL	;

				//	Create	a	units	conversion	function	if	necessary.
				if	(R->units)	{
								ut_unit	*	from	=	ut_parse(get_unit_system(),	R->units,	UT_ASCII)	;
								ut_unit	*	to	=	ut_parse(get_unit_system(),	R->attr->units,	UT_ASCII)	;
								if	(	!from	or	!to	)	{
												std::stringstream	message;
												message	<<	"Can't	convert	\""	<<	R->units	<<	"\"	to	\""	<<	R->attr->units	<<	"\".";
												emitError(message.str());
												return	TRICK_UNITS_CONVERSION_ERROR	;
								}
								cf	=	ut_get_converter(from,to)	;
								if	(	!cf	)	{
												std::stringstream	message;
												message	<<	"Can't	convert	\""	<<	R->units	<<	"\"	to	\""	<<	R->attr->units	<<	"\".";
												emitError(message.str());
												return	TRICK_UNITS_CONVERSION_ERROR	;
								}
				}

				//	R->num_index	is	badly	named.	It	is	really	the	current	dimension
				ret	=	assign_recursive(	R->address,	R->attr,	R->num_index,	0,	V,	cf);
				if	(	cf	)	cv_free(cf)	;

				return	(	ret);

}

