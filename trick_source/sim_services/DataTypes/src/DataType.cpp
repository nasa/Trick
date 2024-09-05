#include	"DataType.hh"
#include	<sstream>

/*
				type-name:	type-specifier
													|	type-specifier	declarator
													;

				declarator	:	pointer
															|	pointer	direct-declarator
															|	direct-declarator
															;

				direct-declarator:	identifier
																					|	(	declarator	)
																					|	[	constant-exp	]
																					|	[	]
																					|	direct-declarator	[	constant-exp	]
																					|	direct-declarator	[	]
																					;
*/

//	VIRTUAL	MEMBER	FUNCTION
//	NOTE:	PointerDataType	and	ArrayDataType	overrides	this	member-function.
std::string	DataType::makeDeclaration(std::string	declarator,	bool	isDirectDeclarator)	const	{
				std::stringstream	ss;
				if	(declarator.empty())	{
								ss	<<	getTypeSpecName();
				}	else	{
								ss	<<	getTypeSpecName()	<<	"	"	<<	declarator	;
				}
				return	ss.str();
}

//	VIRTUAL	MEMBER	FUNCTION
std::string	DataType::toString()	const	{
				return	makeDeclaration("",	true);
}
