
#ifndef	SWIGUNUSED
#	if	defined(__GNUC__)
#			if	!(defined(__cplusplus))	||	(__GNUC__	>	3	||	(__GNUC__	==	3	&&	__GNUC_MINOR__	>=	4))
#					define	SWIGUNUSED	__attribute__	((__unused__))
#			else
#					define	SWIGUNUSED
#			endif
#	elif	defined(__ICC)
#			define	SWIGUNUSED	__attribute__	((__unused__))
#	else
#			define	SWIGUNUSED
#	endif
#endif

#ifndef	SWIGUNUSEDPARM
#	ifdef	__cplusplus
#			define	SWIGUNUSEDPARM(p)
#	else
#			define	SWIGUNUSEDPARM(p)	p	SWIGUNUSED
#	endif
#endif

#ifndef	SWIGINTERN
#	define	SWIGINTERN	static	SWIGUNUSED
#endif

#include	<Python.h>
#include	<iostream>


SWIGINTERN	char*
SWIG_Python_str_AsChar(PyObject	*str)
{
#if	PY_VERSION_HEX	>=	0x03000000
		char	*cstr;
		char	*newstr;
		Py_ssize_t	len;
		str	=	PyUnicode_AsUTF8String(str);
		PyBytes_AsStringAndSize(str,	&cstr,	&len);
		newstr	=	(char	*)	malloc(len+1);
		memcpy(newstr,	cstr,	len+1);
		Py_XDECREF(str);
		return	newstr;
#else
		return	PyString_AsString(str);
#endif
}

#if	PY_VERSION_HEX	>=	0x03000000
#		define	SWIG_Python_str_DelForPy3(x)	free(	(void*)	(x)	)
#else
#		define	SWIG_Python_str_DelForPy3(x)
#endif

extern	"C"	{

		/*	Python-specific	SWIG	API	*/
#define	SWIG_newvarlink()																													SWIG_Python_newvarlink()
#define	SWIG_addvarlink(p,	name,	get_attr,	set_attr)		SWIG_Python_addvarlink(p,	name,	get_attr,	set_attr)
#define	SWIG_InstallConstants(d,	constants)											SWIG_Python_InstallConstants(d,	constants)

		/*	-----------------------------------------------------------------------------
			*	global	variable	support	code.
			*	-----------------------------------------------------------------------------	*/

		typedef	struct	swig_globalvar	{
				char							*name;																		/*	Name	of	global	variable	*/
				PyObject	*(*get_attr)(void);							/*	Return	the	current	value	*/
				int							(*set_attr)(PyObject	*);	/*	Set	the	value	*/
				struct	swig_globalvar	*next;
		}	swig_globalvar;

		typedef	struct	swig_varlinkobject	{
				PyObject_HEAD
				swig_globalvar	*vars;
		}	swig_varlinkobject;

		SWIGINTERN	PyObject	*
		swig_varlink_repr(swig_varlinkobject	*SWIGUNUSEDPARM(v))	{
#if	PY_VERSION_HEX	>=	0x03000000
				return	PyUnicode_InternFromString("<Swig	global	variables>");
#else
				return	PyString_FromString("<Swig	global	variables>");
#endif
		}

		SWIGINTERN	PyObject	*
		swig_varlink_str(swig_varlinkobject	*v)	{
#if	PY_VERSION_HEX	>=	0x03000000
				PyObject	*str	=	PyUnicode_InternFromString("(");
				PyObject	*tail;
				PyObject	*joined;
				swig_globalvar	*var;
				for	(var	=	v->vars;	var;	var=var->next)	{
						tail	=	PyUnicode_FromString(var->name);
						joined	=	PyUnicode_Concat(str,	tail);
						Py_DecRef(str);
						Py_DecRef(tail);
						str	=	joined;
						if	(var->next)	{
								tail	=	PyUnicode_InternFromString(",	");
								joined	=	PyUnicode_Concat(str,	tail);
								Py_DecRef(str);
								Py_DecRef(tail);
								str	=	joined;
						}
				}
				tail	=	PyUnicode_InternFromString(")");
				joined	=	PyUnicode_Concat(str,	tail);
				Py_DecRef(str);
				Py_DecRef(tail);
				str	=	joined;
#else
				PyObject	*str	=	PyString_FromString("(");
				swig_globalvar	*var;
				for	(var	=	v->vars;	var;	var=var->next)	{
						PyString_ConcatAndDel(&str,PyString_FromString(var->name));
						if	(var->next)	PyString_ConcatAndDel(&str,PyString_FromString(",	"));
				}
				PyString_ConcatAndDel(&str,PyString_FromString(")"));
#endif
				return	str;
		}

		SWIGINTERN	int
		swig_varlink_print(swig_varlinkobject	*v,	FILE	*fp,	int	SWIGUNUSEDPARM(flags))	{
				char	*tmp;
				PyObject	*str	=	swig_varlink_str(v);
				fprintf(fp,"Swig	global	variables	");
				fprintf(fp,"%s\n",	tmp	=	SWIG_Python_str_AsChar(str));
				SWIG_Python_str_DelForPy3(tmp);
				Py_DECREF(str);
				return	0;
		}

		SWIGINTERN	void
		swig_varlink_dealloc(swig_varlinkobject	*v)	{
				swig_globalvar	*var	=	v->vars;
				while	(var)	{
						swig_globalvar	*n	=	var->next;
						free(var->name);
						free(var);
						var	=	n;
				}
		}

		SWIGINTERN	PyObject	*
		swig_varlink_getattr(swig_varlinkobject	*v,	char	*n)	{
				PyObject	*res	=	NULL;
				swig_globalvar	*var	=	v->vars;
				while	(var)	{
						if	(strcmp(var->name,n)	==	0)	{
								res	=	(*var->get_attr)();
								break;
						}
						var	=	var->next;
				}
				if	(res	==	NULL	&&	!PyErr_Occurred())	{
						PyErr_SetString(PyExc_NameError,"Unknown	C	global	variable");
				}
				return	res;
		}

		SWIGINTERN	int
		swig_varlink_setattr(swig_varlinkobject	*v,	char	*n,	PyObject	*p)	{
				int	res	=	1;
				swig_globalvar	*var	=	v->vars;
				while	(var)	{
						if	(strcmp(var->name,n)	==	0)	{
								res	=	(*var->set_attr)(p);
								break;
						}
						var	=	var->next;
				}
				if	(res	==	1	&&	!PyErr_Occurred())	{
						PyErr_SetString(PyExc_NameError,"Unknown	C	global	variable");
				}
				return	res;
		}

		SWIGINTERN	PyTypeObject*
		swig_varlink_type(void)	{
				static	char	varlink__doc__[]	=	"Swig	var	link	object";
				static	PyTypeObject	varlink_type;
				static	int	type_init	=	0;
				if	(!type_init)	{
						const	PyTypeObject	tmp
						=	{
								/*	PyObject	header	changed	in	Python	3	*/
#if	PY_VERSION_HEX	>=	0x03000000
								PyVarObject_HEAD_INIT(&PyType_Type,	0)
#else
								PyObject_HEAD_INIT(NULL)
								0,																																		/*	Number	of	items	in	variable	part	(ob_size)	*/
#endif
								(char	*)"swigvarlink",														/*	Type	name	(tp_name)	*/
								sizeof(swig_varlinkobject),									/*	Basic	size	(tp_basicsize)	*/
								0,																																		/*	Itemsize	(tp_itemsize)	*/
								(destructor)	swig_varlink_dealloc,		/*	Deallocator	(tp_dealloc)	*/
								(printfunc)	swig_varlink_print,					/*	Print	(tp_print)	*/
								(getattrfunc)	swig_varlink_getattr,	/*	get	attr	(tp_getattr)	*/
								(setattrfunc)	swig_varlink_setattr,	/*	Set	attr	(tp_setattr)	*/
								0,																																		/*	tp_compare	*/
								(reprfunc)	swig_varlink_repr,							/*	tp_repr	*/
								0,																																		/*	tp_as_number	*/
								0,																																		/*	tp_as_sequence	*/
								0,																																		/*	tp_as_mapping	*/
								0,																																		/*	tp_hash	*/
								0,																																		/*	tp_call	*/
								(reprfunc)	swig_varlink_str,								/*	tp_str	*/
								0,																																		/*	tp_getattro	*/
								0,																																		/*	tp_setattro	*/
								0,																																		/*	tp_as_buffer	*/
								0,																																		/*	tp_flags	*/
								varlink__doc__,																					/*	tp_doc	*/
								0,																																		/*	tp_traverse	*/
								0,																																		/*	tp_clear	*/
								0,																																		/*	tp_richcompare	*/
								0,																																		/*	tp_weaklistoffset	*/
#if	PY_VERSION_HEX	>=	0x02020000
								0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	/*	tp_iter	->	tp_weaklist	*/
#endif
#if	PY_VERSION_HEX	>=	0x02030000
								0,																																		/*	tp_del	*/
#endif
#ifdef	COUNT_ALLOCS
								0,0,0,0																													/*	tp_alloc	->	tp_next	*/
#endif
						};
						varlink_type	=	tmp;
						/*	for	Python	3	we	already	assigned	ob_type	in	PyVarObject_HEAD_INIT()	*/
#if	PY_VERSION_HEX	<	0x03000000
						varlink_type.ob_type	=	&PyType_Type;
#endif
						type_init	=	1;
				}
				return	&varlink_type;
		}

		/*	Create	a	variable	linking	object	for	use	later	*/
		SWIGINTERN	PyObject	*
		SWIG_Python_newvarlink(void)	{
				swig_varlinkobject	*result	=	PyObject_NEW(swig_varlinkobject,	swig_varlink_type());
				if	(result)	{
						result->vars	=	0;
				}
				return	((PyObject*)	result);
		}

		SWIGINTERN	void
		SWIG_Python_addvarlink(PyObject	*p,	char	*name,	PyObject	*(*get_attr)(void),	int	(*set_attr)(PyObject	*p))	{
				swig_varlinkobject	*v	=	(swig_varlinkobject	*)	p;
				swig_globalvar	*gv	=	(swig_globalvar	*)	malloc(sizeof(swig_globalvar));
				if	(gv)	{
						size_t	size	=	strlen(name)+1;
						gv->name	=	(char	*)malloc(size);
						if	(gv->name)	{
								strncpy(gv->name,name,size);
								gv->get_attr	=	get_attr;
								gv->set_attr	=	set_attr;
								gv->next	=	v->vars;
						}
				}
				v->vars	=	gv;
		}

		SWIGINTERN	PyObject	*
		SWIG_globals(void)	{
				static	PyObject	*_SWIG_globals	=	0;
				if	(!_SWIG_globals)	_SWIG_globals	=	SWIG_newvarlink();
				return	_SWIG_globals;
		}

		/*	new_cvar_list	returns	an	empty	cvar	list.
					It	is	not	a	static	function	so	it	is	accessible	in	trick/__init__.py	*/
		PyObject	*	new_cvar_list()	{
						return	SWIG_globals();
		}

		/*	combine_cvars	adds	cvars	from	arg2	list	to	list	in	arg1.
					No	error	checking	is	done	to	test	if	arg2	is	proper	swig_varlinkobject	type	*/
		void	combine_cvars(	PyObject	*arg1	,	PyObject	*arg2	)	{
						swig_globalvar	*	gvit	;
						if	(	arg2	!=	Py_None	)	{
										swig_varlinkobject	*v	=	(swig_varlinkobject	*)	arg2;
										gvit	=	v->vars	;
										while	(gvit	!=	NULL	)	{
														SWIG_addvarlink(	arg1,	gvit->name,	gvit->get_attr,	gvit->set_attr);
														gvit	=	gvit->next	;
										}
						}
		}

}
