#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include	<iostream>
#include	"simpleJSON.hh"

class	LexicalAnalyzer	{
public:
				enum	Lexeme	{
							END_OF_INPUT,
							ERROR,
							LEFT_BRACE,
							RIGHT_BRACE,
							LEFT_SQUARE_BRACKET,
							RIGHT_SQUARE_BRACKET,
							COLON,
							COMMA,
							STRING,
							INTEGER
				};

				LexicalAnalyzer(const	char*);
				int	next_lexeme();
				char*	getTokenText();

private:
				const	char	*	s;
				const	char	*	p;
				const	char	*	vs;
				size_t	vlen;
				char	getch();
				void	ungetch()	;
				LexicalAnalyzer(){}
};

LexicalAnalyzer::LexicalAnalyzer(const	char*	str)	{
				s	=	str;
				p	=	str;
				vs	=	NULL;
				vlen	=	0;
}

char	LexicalAnalyzer::getch()	{
				char	ch;
				if	((ch	=	*p)	!=	0)	{	p++;	}
				return	ch;
}

void	LexicalAnalyzer::ungetch()	{
				if	(p	>	s)	{	p--;	}
}

char*	LexicalAnalyzer::getTokenText()	{
				if	(vlen	>	0)	{
								return	strndup(vs,	vlen);
				}	else	{
								return	NULL;
				}
}

int	LexicalAnalyzer::next_lexeme()	{
					int	state	=	0;
					vlen	=	0;
					char	ch;
					while	((ch	=	getch())	!=	0)	{
									switch	(state)	{
									case	0	:	{	//	Initial	state.
													if	(ch	==	'{')	{
																	return	LEFT_BRACE	;
													}	else	if	(ch	==	'}')	{
																	return	RIGHT_BRACE	;
													}	else	if	(ch	==	'[')	{
																	return	LEFT_SQUARE_BRACKET	;
													}	else	if	(ch	==	']')	{
																	return	RIGHT_SQUARE_BRACKET	;
													}	else	if	(ch	==	':')	{
																	return	COLON	;
													}	else	if	(ch	==	',')	{
																	return	COMMA	;
													}	else	if	(	ch	==	'"')	{
																	state	=	1;
																	vs	=	p;
													}	else	if	(	isdigit(ch)	)	{
																	ungetch();
																	state	=	2;
																	vs	=	p;
													}	else	if	(isspace(ch))	{
																state	=	0;
													}
									}	break;
									case	1	:	{	//	String	literal	accumulation	state.
													while	((ch	!=	0	)	&&	(ch	!=	'"'))
																ch	=	getch();
													if	(ch	==	'"')	{
																	vlen	=	p-vs-1	;
																	return	STRING	;
													}	else	{
																	return	ERROR	;
													}
									}	break;
									case	2	:	{	//	Integer	literal	accumulation	state.
													while	((ch	!=	0	)	&&	(isdigit(ch)))
																ch	=	getch();
													ungetch();
													vlen	=	p-vs;
													return	INTEGER	;
									}	break;
									default:
													return	ERROR	;
									}
					}
					return	END_OF_INPUT;
}

const	char	*token_description(int	token)	{
				const	char	*text;
				switch	(token)	{
								case	LexicalAnalyzer::END_OF_INPUT									:	text	=	"END_OF_INPUT";									break;
								case	LexicalAnalyzer::ERROR																:	text	=	"ERROR";																break;
								case	LexicalAnalyzer::LEFT_BRACE											:	text	=	"LEFT_BRACE";											break;
								case	LexicalAnalyzer::RIGHT_BRACE										:	text	=	"RIGHT_BRACE";										break;
								case	LexicalAnalyzer::LEFT_SQUARE_BRACKET		:	text	=	"LEFT_SQUARE_BRACKET";		break;
								case	LexicalAnalyzer::RIGHT_SQUARE_BRACKET	:	text	=	"RIGHT_SQUARE_BRACKET";	break;
								case	LexicalAnalyzer::COLON																:	text	=	"COLON";																break;
								case	LexicalAnalyzer::COMMA																:	text	=	"COMMA";																break;
								case	LexicalAnalyzer::STRING															:	text	=	"STRING";															break;
								case	LexicalAnalyzer::INTEGER														:	text	=	"INTEGER";														break;
								default																																				:	text	=	"**UNKNOWN**";										break;
				}
				return	text;
}

Member::Member(const	char	*k,	const	char	*v,	int	t)	{
					key=k;
					valText=v;
					type=t;
}

Member*	parseJSON_member(LexicalAnalyzer	&lexan)	{

				const	char*	key;
				const	char*	valText;
				int	type;

				int	token;
				token	=	lexan.next_lexeme();
				if	(	token	==	LexicalAnalyzer::STRING	)	{
								key	=	lexan.getTokenText();
				}	else	{
								std::cout	<<	"ERROR:	Expected	STRING.	Found	\""	<<	token_description(token)	<<	"\"."	<<	std::endl;
								return	NULL;
				}
				token	=	lexan.next_lexeme();
				if	(	token	!=	LexicalAnalyzer::COLON	)	{
								std::cout	<<	"ERROR:	Expected	COLON.	Found	\""	<<	token_description(token)	<<	"\"."	<<	std::endl;
								token_description(token);
								delete	key;
								return	NULL;
				}
				token	=	lexan.next_lexeme();
				if	((	token	==	LexicalAnalyzer::STRING)	||	(	token	==	LexicalAnalyzer::INTEGER	))	{
								valText	=	lexan.getTokenText();
								type	=	token;
				}	else	{
								std::cout	<<	"ERROR:	Expected	STRING	or	INTEGER.	Found	\""	<<	token_description(token)	<<	"."	<<	std::endl;
								token_description(token);
								return	NULL;
				}
				Member	*member	=	new	Member(key,	valText,	type);
				return	member;
}

std::vector<Member*>	parseJSON(	const	char	*json_s)	{

				std::vector<Member*>	members;
				Member*	member;
				int	token;
				bool	okiedokey	=	true;
				LexicalAnalyzer	lexan(json_s);
				token	=	lexan.next_lexeme();
				if	(	token	==	LexicalAnalyzer::LEFT_BRACE	)	{
								member	=	parseJSON_member(lexan);
								if	(member	!=	NULL)	{
												members.push_back(member);
												token	=	lexan.next_lexeme();
												while	(	okiedokey	&&	(token	==	LexicalAnalyzer::COMMA)	)	{
																member	=	parseJSON_member(lexan);
																if	(member	!=	NULL)	{
																				members.push_back(member);
																}	else	{
																				okiedokey	=	false;
																}
																token	=	lexan.next_lexeme();
												}
								}	else	{
												okiedokey	=	false;
								}
								if	(	token	!=	LexicalAnalyzer::RIGHT_BRACE	)	{
												std::cout	<<	"ERROR:	Expected	RIGHT_BRACE.	Found	\""	<<	token_description(token)	<<	"\"."	<<	std::endl;
												token_description(token);
												okiedokey	=	false;
								}
				}	else	{
								std::cout	<<	"ERROR:	Expected	LEFT_BRACE.	Found	\""	<<	token_description(token)	<<	"\"."	<<	std::endl;
								okiedokey	=	false;
				}
				if	(okiedokey	==	false)	{
								std::vector<Member*>::iterator	it;
								it	=	members.begin();
								while	(it	!=	members.end())	{
												delete	*it;
												it	=	members.erase(it);
								}
				}
				return	members;
}
