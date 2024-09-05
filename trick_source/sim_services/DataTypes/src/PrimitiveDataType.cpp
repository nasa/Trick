#include	"PrimitiveDataType.hh"
#include	<ctype.h>
#include	<iostream>

/*
	Template	specializations	for	PrimitiveDataType<T>::getTypeSpecName()
	*/
template	<>	std::string	PrimitiveDataType<void>::getTypeSpecName()	const	{
				std::string	s("void");	return	s;
}
template	<>	std::string	PrimitiveDataType<char>::getTypeSpecName()	const	{
				std::string	s("char");	return	s;
}
template	<>	std::string	PrimitiveDataType<short>::getTypeSpecName()	const	{
				std::string	s("short");	return	s;
}
template	<>	std::string	PrimitiveDataType<int>::getTypeSpecName()	const	{
				std::string	s("int");	return	s;
}
template	<>	std::string	PrimitiveDataType<long>::getTypeSpecName()	const	{
				std::string	s("long");	return	s;
}
template	<>	std::string	PrimitiveDataType<wchar_t>::getTypeSpecName()	const	{
				std::string	s("wchar_t");	return	s;
}
template	<>	std::string	PrimitiveDataType<long	long>::getTypeSpecName()	const	{
				std::string	s("long	long");	return	s;
}
template	<>	std::string	PrimitiveDataType<unsigned	char>::getTypeSpecName()	const	{
				std::string	s("unsigned	char");	return	s;
}
template	<>	std::string	PrimitiveDataType<unsigned	short>::getTypeSpecName()	const	{
				std::string	s("unsigned	short");	return	s;
}
template	<>	std::string	PrimitiveDataType<unsigned	int>::getTypeSpecName()	const	{
				std::string	s("unsigned	int");	return	s;
}
template	<>	std::string	PrimitiveDataType<unsigned	long>::getTypeSpecName()	const	{
				std::string	s("unsigned	long");	return	s;
}
template	<>	std::string	PrimitiveDataType<unsigned	long	long>::getTypeSpecName()	const	{
				std::string	s("unsigned	long	long");	return	s;
}
template	<>	std::string	PrimitiveDataType<float>::getTypeSpecName()	const	{
				std::string	s("float");	return	s;
}
template	<>	std::string	PrimitiveDataType<double>::getTypeSpecName()	const	{
				std::string	s("double");	return	s;
}

/*
	Template	specializations	for	PrimitiveDataType<T>::isFloatingPoint()
	*/
template	<>	bool	PrimitiveDataType<float>::isFloatingPoint()	const	{	return	true;	}
template	<>	bool	PrimitiveDataType<double>::isFloatingPoint()	const	{	return	true;	}

/*
	Template	specializations	for	PrimitiveDataType<T>::isSigned()
	*/
template	<>	bool	PrimitiveDataType<char>::isSigned()	const	{	return	true;	}
template	<>	bool	PrimitiveDataType<short>::isSigned()	const	{	return	true;	}
template	<>	bool	PrimitiveDataType<int>::isSigned()	const	{	return	true;	}
template	<>	bool	PrimitiveDataType<long>::isSigned()	const	{	return	true;	}
template	<>	bool	PrimitiveDataType<long	long>::isSigned()	const	{	return	true;	}

/*
	Template	specializations	for	PrimitiveDataType<T>::isVoid()
	*/
template	<>	bool	PrimitiveDataType<void>::isVoid()	const	{	return	true;	}

/*
	Template	specializations	for	PrimitiveDataType<char>::printValue()
*/
template	<>	void	PrimitiveDataType<char>::printValue(std::ostream	&s,	void	*address	)	const	{
				if	(isprint(	(int)*(char*)address)	)	{
								s	<<	"'"	<<	*(char*)address	<<	"'";
				}	else	{
								s	<<	(int)*(char*)address;
				}
}

template	<>	void	PrimitiveDataType<unsigned	char>::printValue(std::ostream	&s,	void	*address	)	const	{
				if	(isprint(	(int)*(unsigned	char*)address)	)	{
								s	<<	"'"	<<	*(unsigned	char*)address	<<	"'";
				}	else	{
								s	<<	(int)*(unsigned	char*)address;
				}
}

/*
	Template	specializations	for	PrimitiveDataType<void>
	*/
template	<>	size_t	PrimitiveDataType<void>::getSize()	const	{
				std::cerr	<<	"WARNING:	Calling	PrimitiveDataType<void>::getSize()"
														<<	",	i.e.,	attempting	to	get	the	size	of	void,	indicates	that"
														<<	"	you	may	be	up	to	no	good	and	about	to	run	out	of	luck."	<<	std::endl;
				return	0;
}
template	<>	void	PrimitiveDataType<void>::deleteInstance(void*	address)	const	{
				std::cerr	<<	"WARNING:	Cannot	delete	the	object,	of	type	<void>	at	address	("
														<<	address	<<	")."	<<	std::endl;
}
template	<>	void*	PrimitiveDataType<void>::createInstance(unsigned	int	num)	const	{
				std::cerr	<<	"WARNING:	Cannot	create	an	object,	of	type	<void>."	<<	std::endl;
				return	NULL;
}
template	<>	void	PrimitiveDataType<void>::	clearValue(void	*	address)	const	{
				std::cerr	<<	"WARNING:	Cannot	clear	the	value	of	the	object,	of	type	<void>	at	address	("
														<<	address	<<	")."	<<	std::endl;
}
template	<>	void	PrimitiveDataType<void>::	assignValue(void	*	address,	Value	*	value)	const	{
				std::cerr	<<	"WARNING:	Cannot	assign	a	value	to	the	object,	of	type	<void>	at	address	("
														<<	address	<<	")."	<<	std::endl;
}
template	<>	void	PrimitiveDataType<void>::printValue(std::ostream	&s,	void	*address	)	const	{
				std::cerr	<<	"WARNING:	Cannot	print	the	value	of	the	object,	of	type	<void>	at	address	("	<<	address
														<<	"	of	type	void."	<<	std::endl;
}
