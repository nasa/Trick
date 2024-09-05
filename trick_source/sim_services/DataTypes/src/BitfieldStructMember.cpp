#include	"BitfieldStructMember.hh"

/*
	Template	specializations	for	BitfieldStructMember<T>::getTypeSpecName()
	*/
template	<>	std::string	BitfieldStructMember<char>::getTypeSpecName()	const	{
				std::string	s("char");	return	s;
}
template	<>	std::string	BitfieldStructMember<short>::getTypeSpecName()	const	{
				std::string	s("short");	return	s;
}
template	<>	std::string	BitfieldStructMember<int>::getTypeSpecName()	const	{
				std::string	s("int");	return	s;
}
template	<>	std::string	BitfieldStructMember<long>::getTypeSpecName()	const	{
				std::string	s("long");	return	s;
}
template	<>	std::string	BitfieldStructMember<long	long>::getTypeSpecName()	const	{
				std::string	s("long	long");	return	s;
}
template	<>	std::string	BitfieldStructMember<unsigned	char>::getTypeSpecName()	const	{
				std::string	s("unsigned	char");	return	s;
}
template	<>	std::string	BitfieldStructMember<unsigned	short>::getTypeSpecName()	const	{
				std::string	s("unsigned	short");	return	s;
}
template	<>	std::string	BitfieldStructMember<unsigned	int>::getTypeSpecName()	const	{
				std::string	s("unsigned	int");	return	s;
}
template	<>	std::string	BitfieldStructMember<unsigned	long>::getTypeSpecName()	const	{
				std::string	s("unsigned	long");	return	s;
}
template	<>	std::string	BitfieldStructMember<unsigned	long	long>::getTypeSpecName()	const	{
				std::string	s("unsigned	long	long");	return	s;
}

