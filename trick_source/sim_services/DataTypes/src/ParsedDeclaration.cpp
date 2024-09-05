#include	"ParsedDeclaration.hh"
#include	"LexicalAnalyzer.hh"
#include	<cstdlib>

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

//	CONSTRUCTOR
ParsedDeclaration::ParsedDeclaration(	std::string	s	)	{
				typeSpec.clear();
				varName.clear();
				ndims	=	0;
				for	(int	ii	=	0;	ii	<	MAX_DIMS;	ii++){
								dims[ii]	=	0;
				}
				lexer.load(s);
				parseDeclaration();
}

//	===============================================
//	type-name:	type-specifier
//										|	type-specifier	declarator
//										;
//	===============================================
bool	ParsedDeclaration::parseDeclaration()	{

				std::cout	<<	__FUNCTION__	<<	std::endl;
				bool	errorCondition	=	parseTypeSpecifier();
				if	(	!errorCondition	)	{
								Token::e	token	=	lexer.getToken();
								if	((token	==	Token::Asterisk)	||
												(token	==	Token::Identifier	)	||
												(token	==	Token::LeftParen	)	||
												(token	==	Token::LeftBracket))	{
												errorCondition	=	parseDeclarator();
								}
								if	(lexer.matchToken(Token::EndOfFile)	==	Token::Error)	{
												errorCondition	=	true;
								}
				}
				return	errorCondition;
}

//	==============================================================
//	declarator	:	pointer
//												|	pointer	direct-declarator
//												|	direct-declarator
//												;
//
//	NOTE:	a	pointer	is	one	or	more	asterisks.
//	==============================================================
bool	ParsedDeclaration::parseDeclarator()	{

			std::cout	<<	__FUNCTION__	<<	std::endl;
			bool	errorCondition	=	false;
			int	asteriskCount	=	0;

			Token::e	token	=	lexer.getToken();
			while	(token	==	Token::Asterisk	)	{
							token	=	lexer.matchToken(	Token::Asterisk	);
							asteriskCount	++;
			}
			if	((token	==	Token::LeftParen)	||	(token	==	Token::LeftBracket)	||	(token	==	Token::Identifier))	{
							errorCondition	|=	parseDirectDeclarator();
			}

			if	(asteriskCount	!=	0)	{
							for	(int	ii=0	;	ii	<	asteriskCount	;	ii++)	{
											dims[ndims]	=	-1;
											ndims	++;
							}
			}
			return	errorCondition;
}

//	==============================================================
//	direct-declarator:	identifier
//																		|	(	declarator	)
//																		|	[	integer	]
//																		|	direct-declarator	[	integer	]
//																		;
//	==============================================================
bool	ParsedDeclaration::parseDirectDeclarator()	{

			std::cout	<<	__FUNCTION__	<<	std::endl;
			bool	errorCondition	=	false;

			Token::e	token	=	lexer.getToken();
			if	(token	==	Token::Identifier)	{
							varName	=	lexer.getText();
							token	=	lexer.matchToken(	Token::Identifier	);
			}	else	if	(token	==	Token::LeftParen)	{
							token	=	lexer.matchToken(	Token::LeftParen	);
							errorCondition	|=	parseDeclarator();
							if	(	!errorCondition	)	{
											if	((token	=	lexer.matchToken(	Token::RightParen	))	==	Token::Error)	{
											errorCondition	=	true;
											}
							}
			}	else	if	(token	==	Token::LeftBracket)	{
							token	=	lexer.matchToken(	Token::LeftBracket	);
							if	(token	==	Token::Integer)	{
											dims[ndims]	=	std::atoi(	(lexer.getText()).c_str()	);
											ndims	++;
											token	=	lexer.matchToken(Token::Integer);
							}
							if	((token	=	lexer.matchToken(	Token::RightBracket	))	==	Token::Error)	{
											errorCondition	=	true;
							}
			}	else	{
							//	Note:	this	is	the	case	of	an	anonymous	declaration.
							varName.clear();
			}

			while	(token	==	Token::LeftBracket)	{
							token	=	lexer.matchToken(	Token::LeftBracket	);
							if	(token	==	Token::Integer)	{
											dims[ndims]	=	std::atoi(	(lexer.getText()).c_str()	);
											ndims	++;
											token	=	lexer.matchToken(Token::Integer);
							}
							if	((token	=	lexer.matchToken(	Token::RightBracket	))	==	Token::Error)	{
											errorCondition	=	true;
							}
			}
			return	errorCondition;
}

bool	ParsedDeclaration::parseTypeSpecifier()	{

				std::cout	<<	__FUNCTION__	<<	std::endl;
				bool	errorCondition	=	false;
				Token::e	token	=	lexer.getToken();

				switch	(	token	)	{
								case	Token::Void	:	{
												token	=	lexer.matchToken(Token::Void);
												typeSpec	=	"void";
								}	break;
								case	Token::Boolean	:	{
												token	=	lexer.matchToken(Token::Boolean);
												typeSpec	=	"bool";
								}	break;
								case	Token::Char	:	{
												token	=	lexer.matchToken(Token::Char);
												typeSpec	=	"char";
								}	break;
								case	Token::Short	:	{
												token	=	lexer.matchToken(Token::Short);
												typeSpec	=	"short";
								}	break;
								case	Token::Int	:	{
												token	=	lexer.matchToken(Token::Int);
												typeSpec	=	"int";
								}	break;
								case	Token::Long	:	{
												token	=	lexer.matchToken(Token::Long);
												if	(token	==	Token::Long)	{
																token	=	lexer.matchToken(Token::Long);
																typeSpec	=	"long	long";
												}	else	{
																typeSpec	=	"long";
												}
								}	break;
								case	Token::Unsigned	:	{
												token	=	lexer.matchToken(Token::Unsigned);
												switch	(	token	)	{
																case	Token::Char	:	{
																				token	=	lexer.matchToken(Token::Char);
																				typeSpec	=	"unsigned	char";
																}	break;
																case	Token::Short	:	{
																				token	=	lexer.matchToken(Token::Short);
																				typeSpec	=	"unsigned	short";
																}	break;
																case	Token::Int	:	{
																				token	=	lexer.matchToken(Token::Int);
																				typeSpec	=	"unsigned	int";
																}	break;
																case	Token::Long	:	{
																				token	=	lexer.matchToken(Token::Long);
																				if	(token	==	Token::Long)	{
																								token	=	lexer.matchToken(Token::Long);
																								typeSpec	=	"unsigned	long	long";
																				}	else	{
																								typeSpec	=	"unsigned	long";
																				}
																}	break;
																default:	{
																				typeSpec	=	"unsigned	int";
																}
												}
								}	break;
								case	Token::Signed	:	{
												token	=	lexer.matchToken(Token::Signed);
												switch	(	token	)	{
																case	Token::Char	:	{
																				token	=	lexer.matchToken(Token::Char);
																				typeSpec	=	"char";
																}	break;
																case	Token::Short	:	{
																				token	=	lexer.matchToken(Token::Short);
																				typeSpec	=	"short";
																}	break;
																case	Token::Int	:	{
																				token	=	lexer.matchToken(Token::Int);
																				typeSpec	=	"int";
																}	break;
																case	Token::Long	:	{
																				token	=	lexer.matchToken(Token::Long);
																				if	(token	==	Token::Long)	{
																								token	=	lexer.matchToken(Token::Long);
																								typeSpec	=	"long	long";
																				}	else	{
																								typeSpec	=	"long";
																				}
																}	break;
																default:	{
																				typeSpec	=	"int";
																}
												}
								}	break;
								case	Token::WideChar	:	{
												token	=	lexer.matchToken(Token::WideChar);
												typeSpec	=	"wchar_t";
								}	break;
								case	Token::Float	:	{
												token	=	lexer.matchToken(Token::Float);
												typeSpec	=	"float";
								}	break;
								case	Token::Double	:	{
												token	=	lexer.matchToken(Token::Double);
												typeSpec	=	"double";
								}	break;
								case	Token::Identifier	:	{
												token	=	lexer.matchToken(Token::Identifier);
												typeSpec	=	lexer.getText();
								}	break;
								default	:	{
												errorCondition	=	true;
								}
				}
				return	errorCondition;
}

std::string	ParsedDeclaration::getTypeSpecifier()	{
				return	typeSpec;
}

std::string	ParsedDeclaration::getVariableName()	{
				return	varName;
}

unsigned	int	ParsedDeclaration::getNumberOfDimensions()	{
				return	ndims;
}

int	ParsedDeclaration::getDimensionSize(	unsigned	int	index)	{
				//	FIXME:	needs	bounds	checking.
				return	dims[index];
}
