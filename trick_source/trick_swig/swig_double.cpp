
#include	<iostream>
#include	"trick/swig/swig_macros.hh"
#include	"trick/swig/swig_double.hh"
#include	"trick/swig/swig_int.hh"
#include	"trick/swig/swig_convert_units.hh"

char	swig_double::str_output[32]	;

swig_double::swig_double()	{
				value	=	0	;
				units	=	"1"	;
}

char	*	swig_double::__str__()	{
				if	(	!	units.empty()	&&	units.compare("1")	)	{
								snprintf(str_output	,	sizeof(str_output),	"%g	%s",	value	,	units.c_str())	;
				}	else	{
								snprintf(str_output	,	sizeof(str_output),	"%g",	value	)	;
				}
				return(str_output)	;
}

char	*	swig_double::__repr__()	{
				return(__str__())	;
}


//**********************************************************************************************

PyObject	*	swig_double::__add__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;
				double	new_value	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	value	+	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{

								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_value	=	(double)temp_m->value	;

								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_value	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	+	new_value	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_value	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_value	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	+	new_value	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	+	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__sub__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;
				double	new_value	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	value	-	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{

								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_value	=	(double)temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_value	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	-	new_value	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_value	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_value	)	;
								if	(	conv_ret	==	0	)	{
												result->value	=	value	-	new_value	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	-	PyInt_AsLong(obj1)	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__mul__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	value	*	PyFloat_AsDouble(obj1)	;
								result->units	=	units	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								result->value	=	value	*	temp_m->value	;
								result->units	=	units	+	"*("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								result->value	=	value	*	temp_m->value	;
								result->units	=	units	+	"*("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	value	*	PyInt_AsLong(obj1)	;
								result->units	=	units	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__div__(	PyObject	*	obj1	)	{
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

PyObject	*	swig_double::__truediv__(	PyObject	*	obj1	)	{
				return	__div__(obj1)	;
}

PyObject	*	swig_double::__floordiv__(	PyObject	*	obj1	)	{
				swig_int	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_int()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	(long	long)(value	/	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								result->value	=	(long	long)(value	/	temp_m->value)	;
								result->units	=	units	+	"/("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								result->value	=	(long	long)(value	/	temp_m->value)	;
								result->units	=	units	+	"/("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	(long	long)(value	/	PyInt_AsLong(obj1))	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_int	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__mod__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	fmod(value	,	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	fmod(	value	,	temp_m->value	)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Modulus	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	fmod(	value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Modulus	must	be	unitless.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	fmod(	value	,	PyInt_AsLong(obj1))	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__pow__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	pow(value	,	PyFloat_AsDouble(obj1))	;
								result->units	=	units	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	pow(value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												result->value	=	pow(value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								int	power	=	PyInt_AsLong(obj1)	;
								result->value	=	pow(value	,	power)	;
								result->units	=	units	;
								for	(	int	ii	=	1	;	ii	<	power	;	ii++	)	{
												result->units	=	result->units	+	"*("	+	units	+	")";
								}
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

//**********************************************************************************************

PyObject	*	swig_double::__radd__(	PyObject	*	obj1	)	{
				return	__add__(obj1)	;
}

PyObject	*	swig_double::__rsub__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	PyFloat_AsDouble(obj1)	-	value	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	PyInt_AsLong(obj1)	-	value	;
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__rmul__(	PyObject	*	obj1	)	{
				return	__mul__(obj1)	;
}

PyObject	*	swig_double::__rdiv__(	PyObject	*	obj1	)	{
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

PyObject	*	swig_double::__rtruediv__(	PyObject	*	obj1	)	{
				return	__rdiv__(obj1)	;
}

PyObject	*	swig_double::__rfloordiv__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								result->value	=	floor(PyFloat_AsDouble(obj1)	/	value)	;
				}	else	if	(	PyInt_Check(obj1)	)	{
								result->value	=	floor(PyInt_AsLong(obj1)	/	value)	;
				}
				result->units	=	"1/("	+	units	+	")"	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__rmod__(	PyObject	*	obj1	)	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				if	(	PyFloat_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												result->value	=	fmod(PyFloat_AsDouble(obj1)	,	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Modulus	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								if	(	!units.compare("1"))	{
												result->value	=	fmod(PyInt_AsLong(obj1)	,	value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Modulus	must	be	unitless.		Cannot	create	new	unit-ed	type.");
								}
				}
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__rpow__(	PyObject	*	obj1	)	{
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

//**********************************************************************************************


PyObject	*	swig_double::__iadd__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;
				double	new_val	;

				if	(	PyFloat_Check(obj1)	)	{
								value	+=	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	(double)temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	+=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	+=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	+=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__isub__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;
				int	conv_ret	;
				double	new_val	;

				if	(	PyFloat_Check(obj1)	)	{
								value	-=	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	-=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								new_val	=	temp_m->value	;
								conv_ret	=	scale_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												value	-=	new_val	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	-=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__imul__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	*=	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								value	*=	temp_m->value	;
								units	+=	"*("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								value	*=	temp_m->value	;
								units	+=	"*("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	*=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__idiv__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	/=	PyFloat_AsDouble(obj1)	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								value	/=	temp_m->value	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								value	/=	temp_m->value	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	/=	PyInt_AsLong(obj1)	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__itruediv__(	PyObject	*	obj1	)	{
				return	__idiv__(obj1)	;
}

PyObject	*	swig_double::__ifloordiv__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	floor(value	/	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								value	=	floor(value	/	temp_m->value)	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								value	=	floor(value	/	temp_m->value)	;
								units	+=	"/("	+	temp_m->units	+	")";
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	=	floor(	value	/	PyInt_AsLong(obj1))	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__imod__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	PyFloat_Check(obj1)	)	{
								value	=	fmod(	value	,	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	fmod(value	,	temp_m->value	);
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Operation	must	contain	at	least	one	unit-less	value.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	fmod(	value	,	temp_m->value	);
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Operation	must	contain	at	least	one	unit-less	value.		Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	=	fmod(	value	,	PyInt_AsLong(obj1))	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__ipow__(	PyObject	*	obj1	)	{
				PyObject	*	ret	=	NULL	;
				void	*	argp2	;

				if	(	units.compare("1"))	{
								PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
								return	NULL	;
				}

				if	(	PyFloat_Check(obj1)	)	{
								value	=	pow(value	,	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	pow(value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								if	(	!temp_m->units.compare("1"))	{
												value	=	pow(value	,	temp_m->value)	;
								}	else	{
												PyErr_SetString(PyExc_TypeError,"Both	arguments	must	be	unitless.	Cannot	create	new	unit-ed	type.");
												return	NULL	;
								}
				}	else	if	(	PyInt_Check(obj1)	)	{
								value	=	pow(value	,	PyInt_AsLong(obj1))	;
				}

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(new	swig_double(*this)),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

//**********************************************************************************************

PyObject	*	swig_double::__lt__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	<	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<	new_val)	;
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

PyObject	*	swig_double::__le__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	<=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<=	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	<=	new_val)	;
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

PyObject	*	swig_double::__eq__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	==	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	==	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	==	new_val)	;
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

PyObject	*	swig_double::__ne__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	!=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	!=	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	!=	new_val)	;
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

PyObject	*	swig_double::__gt__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	>	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>	new_val)	;
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

PyObject	*	swig_double::__ge__(	PyObject	*	obj1	)	{
				void	*	argp2	;
				bool	eq	=	0	;
				int	conv_ret	;

				if	(	PyFloat_Check(obj1)	)	{
								eq	=	(value	>=	PyFloat_AsDouble(obj1))	;
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_int	*"),	0	)))	{
								swig_int	*	temp_m	=	reinterpret_cast<	swig_int	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>=	new_val)	;
								}	else	{
												return	NULL	;
								}
				}	else	if	(SWIG_IsOK(SWIG_ConvertPtr(obj1,	&argp2,SWIG_TypeQuery("swig_double	*"),	0	)))	{
								swig_double	*	temp_m	=	reinterpret_cast<	swig_double	*	>(argp2)	;
								double	new_val	=	temp_m->value	;
								conv_ret	=	convert_united_value(	units	,	temp_m->units	,	&new_val	)	;
								if	(	conv_ret	==	0	)	{
												eq	=	(value	>=	new_val)	;
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

PyObject	*	swig_double::__neg__()	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				result->value	=	-value	;
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__pos__()	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				result->value	=	value	;
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

PyObject	*	swig_double::__abs__()	{
				swig_double	*	result	;
				PyObject	*	ret	=	NULL	;

				result	=	new	swig_double()	;

				result->value	=	fabs(value)	;
				result->units	=	units	;

				ret	=	SWIG_NewPointerObj(SWIG_as_voidptr(result),	SWIG_TypeQuery("swig_double	*"),	SWIG_POINTER_OWN);

				return	ret	;
}

//**********************************************************************************************

PyObject	*	swig_double::__int__()	{
				return	PyInt_FromLong((long	long)value)	;
}

PyObject	*	swig_double::__long__()	{
				return	PyLong_FromLongLong((long	long)value)	;
}

PyObject	*	swig_double::__float__()	{
				return	PyFloat_FromDouble(value)	;
}

//**********************************************************************************************

PyObject	*	swig_double::__len__()	{
				return	PyInt_FromLong(1)	;
}

