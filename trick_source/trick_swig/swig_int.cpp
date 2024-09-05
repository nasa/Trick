
#include	<iostream>
#include	"trick/swig/swig_macros.hh"
#include	"trick/swig/swig_int.hh"
#include	"trick/swig/swig_double.hh"
#include	"trick/swig/swig_convert_units.hh"

char	swig_int::str_output[32]	;

swig_int::swig_int()	{
				value	=	0	;
				units	=	"1"	;
}

char	*	swig_int::__str__()	{
				if	(	!	units.empty()	&&	units.compare("1")	)	{
								snprintf(str_output	,	sizeof(str_output),	"%lld	%s",	value	,	units.c_str())	;
				}	else	{
								snprintf(str_output	,	sizeof(str_output),	"%lld",	value	)	;
				}
				return(str_output)	;
}

char	*	swig_int::__repr__()	{
				return(__str__())	;
}

//**********************************************************************************************

PyObject	*	swig_int::__add__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	(long	long)(round(value	+	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	+	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	(long	long)(round(value	+	new_val))	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	+	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__sub__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	(long	long)(round(value	-	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	-	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	(long	long)(round(value	-	new_val))	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	-	(long	long)PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__mul__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;


				if	(	PyFloat_Check(obj1)	)	{
								swig_double	*	result	=	new	swig_double()	;
								result->value	=	(double)value	*	PyFloat_AsDouble(obj1)	;
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	result	=	new	swig_int()	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								result->value	=	value	*	temp_m->value	;
								result->units	=	units	+	"*("	+	temp_m->units	+	")";
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	result	=	new	swig_double()	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								result->value	=	value	*	temp_m->value	;
								result->units	=	units	+	"*("	+	temp_m->units	+	")";
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(	PyInt_Check(obj1)	)	{
								swig_int	*	result	=	new	swig_int()	;
								result->value	=	value	*	PyInt_AsLong(obj1)	;
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}

				return	ret	;
}

PyObject	*	swig_int::__div__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	value	/	PyFloat_AsDouble(obj1)	;
								result->units	=	units	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								result->value	=	value	/	temp_m->value	;
								result->units	=	units	+	"/("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								result->value	=	value	/	temp_m->value	;
								result->units	=	units	+	"/("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	/	PyInt_AsLong(obj1)	;
								result->units	=	units	;
				}
				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				return	ret	;
}

PyObject	*	swig_int::__truediv__(	PyObject	*	obj1	)	{
				return	__div__(obj1)	;
}

PyObject	*	swig_int::__floordiv__(	PyObject	*	obj1	)	{
				return	__div__(obj1)	;
}

PyObject	*	swig_int::__mod__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								swig_double	*	result	=	new	swig_double()	;
								result->value	=	fmod(value	,	PyFloat_AsDouble(obj1))	;
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	result	=	new	swig_int()	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	%	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Divisor	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	result	=	new	swig_double()	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	fmod(	value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Divisor	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(	PyInt_Check(obj1)	)	{
								swig_int	*	result	=	new	swig_int()	;
								result->value	=	value	%	PyInt_AsLong(obj1)	;
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}

				return	ret	;
}

PyObject	*	swig_int::__pow__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	units.compare("1"))	{
								PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
								return	NULL	;
				}

				if	(	PyFloat_Check(obj1)	)	{
								swig_double	*	result	=	new	swig_double()	;
								result->value	=	pow(value	,	PyFloat_AsDouble(obj1))	;
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								swig_int	*	result	=	new	swig_int()	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	(long	long)(pow(value	,	temp_m->value))	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	result	=	new	swig_double()	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	pow(value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(	PyInt_Check(obj1)	)	{
								swig_int	*	result	=	new	swig_int()	;
								int	power	=	PyInt_AsLong(obj1)	;
								result->value	=	(long	long)(pow(value	,	power))	;
								result->units	=	units	;
								for	(	int	ii	=	1	;	ii	<	power	;	ii++	)	{
												result->units	=	result->units	+	"*("	+	units	+	")";
								}
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}

				return	ret	;
}

PyObject	*	swig_int::__lshift__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	<<	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	<<	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__rshift__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	>>	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	>>	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__and__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	&	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	&	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__xor__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	^	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	^	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__or__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	value	|	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	|	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

//**********************************************************************************************

PyObject	*	swig_int::__radd__(	PyObject	*	obj1	)	{
				return	__add__(obj1)	;
}

PyObject	*	swig_int::__rsub__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	(long	long)(round(PyFloat_AsDouble(obj1)	-	value))	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	(long	long)(PyInt_AsLong(obj1)	-	value)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__rmul__(	PyObject	*	obj1	)	{
				return	__mul__(obj1)	;
}

PyObject	*	swig_int::__rdiv__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	PyFloat_AsDouble(obj1)	/	value	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	PyInt_AsLong(obj1)	/	value	;
				}
				result->units	=	"1/("	+	units	+	")"	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__rtruediv__(	PyObject	*	obj1	)	{
				return	__rdiv__(obj1)	;
}

PyObject	*	swig_int::__rfloordiv__(	PyObject	*	obj1	)	{
				return	__rdiv__(obj1)	;
}

PyObject	*	swig_int::__rmod__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;


				if	(	PyFloat_Check(obj1)	)	{
								swig_double	*	result	=	new	swig_double()	;
								if	(	!units.compare("1"))	{
												result->value	=	fmod(PyFloat_AsDouble(obj1)	,	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Divisor	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);
				}	else	if	(	PyInt_Check(obj1)	)	{
								swig_int	*	result	=	new	swig_int()	;
								if	(	!units.compare("1"))	{
												result->value	=	(long	long)(PyInt_AsLong(obj1)	%	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Divisor	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
								result->units	=	units	;
								ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				}

				return	ret	;
}

PyObject	*	swig_int::__rpow__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;

				if	(	PyFloat_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												return(PyFloat_FromDouble(pow(PyFloat_AsDouble(obj1)	,	value)))	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Power	argument	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												return(PyFloat_FromDouble(pow(PyInt_AsLong(obj1)	,	value)))	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Power	argument	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
				}

				return	ret	;
}

PyObject	*	swig_int::__rlshift__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shifted	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												result->value	=	(long	long)(PyInt_AsLong(obj1)	<<	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__rrshift__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shifted	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												result->value	=	(long	long)(PyInt_AsLong(obj1)	>>	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__rand__(	PyObject	*	obj1	)	{
				return	__and__(obj1)	;
}

PyObject	*	swig_int::__rxor__(	PyObject	*	obj1	)	{
				return	__xor__(obj1)	;
}

PyObject	*	swig_int::__ror__(	PyObject	*	obj1	)	{
				return	__or__(obj1)	;
}

//**********************************************************************************************

PyObject	*	swig_int::__iadd__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)round(value	+	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	+=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	=	(long	long)round(value	+	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	+=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}


PyObject	*	swig_int::__isub__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)round(value	-	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	-=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	=	(long	long)round(value	-	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	-=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__imul__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)(round(value	*	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								value	*=	temp_m->value	;
								units	+=	"*("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								value	=	(long	long)round(value	*	temp_m->value)	;
								units	+=	"*("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	*=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__idiv__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)(round(value	/	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								value	/=	temp_m->value	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								value	=	(long	long)round(value	/	temp_m->value)	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	/=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__itruediv__(	PyObject	*	obj1	)	{
				return	__idiv__(obj1)	;
}

PyObject	*	swig_int::__ifloordiv__(	PyObject	*	obj1	)	{
				return	__idiv__(obj1)	;
}

PyObject	*	swig_int::__imod__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)round(fmod(	value	,	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	%=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Operation	must	contain	at	least	one	unit-less	value.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	(long	long)round(fmod(	value	,	temp_m->value	));
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Operation	must	contain	at	least	one	unit-less	value.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	%=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__ipow__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	units.compare("1"))	{
								PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
								return	NULL	;
				}

				if	(	PyFloat_Check(obj1)	)	{
								value	=	(long	long)round(pow(value	,	PyFloat_AsDouble(obj1)))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	(long	long)(pow(value	,	temp_m->value))	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	(long	long)round(pow(value	,	temp_m->value))	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	=	(long	long)(pow(value	,	PyInt_AsLong(obj1)))	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__ilshift__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	<<=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	<<=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__irshift__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	>>=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Shift	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	>>=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;

}

PyObject	*	swig_int::__iand__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	&=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	&=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__ixor__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	^=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	^=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_int::__ior__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	|=	temp_m->value	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	unitless.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								PyErr_SetString(PyExc_TypeError,"Bitmask	value	must	be	integer");
								return	NULL	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	|=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_int(*this)),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

//**********************************************************************************************

PyObject	*	swig_int::__lt__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	<	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	<	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

PyObject	*	swig_int::__le__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	<=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<=	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<=	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	<=	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

PyObject	*	swig_int::__eq__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	==	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	==	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	==	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	==	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

PyObject	*	swig_int::__ne__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	!=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	!=	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	!=	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	!=	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

PyObject	*	swig_int::__gt__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	>	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	>	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

PyObject	*	swig_int::__ge__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	>=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								long	long	new_val	;
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>=	temp_m->value)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								double	new_val	;
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>=	new_val)	;	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								eq	=	(value	>=	PyInt_AsLong(obj1))	;
				}

				if	(	eq	)	{
								Py_RETURN_TRUE	;
				}

				Py_RETURN_FALSE	;
}

//**********************************************************************************************

PyObject	*	swig_int::__neg__()	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;
				result->value	=	-value	;
				result->units	=	units	;
				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				return	ret	;
}

PyObject	*	swig_int::__pos__()	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;
				result->value	=	value	;
				result->units	=	units	;
				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				return	ret	;
}

PyObject	*	swig_int::__abs__()	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;
				result->value	=	llabs(value)	;
				result->units	=	units	;
				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				return	ret	;
}

PyObject	*	swig_int::__invert__()	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_int()	;
				result->value	=	~value	;
				result->units	=	units	;
				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);
				return	ret	;
}

PyObject	*	swig_int::__nonzero__()	{
				if	(	value	!=	0	)	{
								Py_RETURN_TRUE	;
				}
				Py_RETURN_FALSE	;
}

//**********************************************************************************************

PyObject	*	swig_int::__int__()	{
				return	PyInt_FromLong((long)value)	;
}

PyObject	*	swig_int::__long__()	{
				return	PyLong_FromLongLong(value)	;
}

PyObject	*	swig_int::__float__()	{
				return	PyFloat_FromDouble((double)value)	;
}

//**********************************************************************************************

PyObject	*	swig_int::__oct__()	{
				char	temp[32]	;
				snprintf(temp,	sizeof(temp),	"0%o"	,	(unsigned	int)value)	;
				return	PyString_FromString(temp)	;
}

PyObject	*	swig_int::__hex__()	{
				char	temp[32]	;
				snprintf(temp,	sizeof(temp),	"0x%llx"	,	value)	;
				return	PyString_FromString(temp)	;
}

//**********************************************************************************************

PyObject	*	swig_int::__len__()	{
				return	PyInt_FromLong(1)	;
}

//**********************************************************************************************

//	New	for	python	3

int	swig_int::__index__()	{
				return	value	;
}

PyObject	*	swig_int::__bool__()	{
				return	__nonzero__()	;
}
