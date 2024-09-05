#include	<gtest/gtest.h>
#include	<stddef.h>
#include	"LexicalAnalyzer.hh"
#include	<iostream>

TEST(LexicalAnalyzer,	test1)	{

				Token::e	token;
				LexicalAnalyzer	lexer;

				lexer.load("float");
				token	=	lexer.getToken();
				EXPECT_EQ(Token::Float,	token	);

				token	=	lexer.nextToken();
				EXPECT_EQ(Token::EndOfFile,	token	);
}

TEST(LexicalAnalyzer,	test2)	{

				Token::e	token;
				LexicalAnalyzer	lexer;

				lexer.load("double(*)[123]");

				token	=	lexer.getToken();

				EXPECT_EQ(Token::Double,	token	);

				token	=	lexer.matchToken(Token::Double);

				EXPECT_EQ(Token::LeftParen,	token	);

				token	=	lexer.matchToken(Token::LeftParen);

				EXPECT_EQ(Token::Asterisk,	token	);

				token	=	lexer.matchToken(Token::Asterisk);

				EXPECT_EQ(Token::RightParen,	token	);

				token	=	lexer.matchToken(Token::RightParen);

				EXPECT_EQ(Token::LeftBracket,	token	);

				token	=	lexer.matchToken(Token::LeftBracket);

				EXPECT_EQ(Token::Integer,	token	);

				token	=	lexer.matchToken(Token::Integer);

				EXPECT_EQ(Token::RightBracket,	token	);

				token	=	lexer.matchToken(Token::RightBracket);

				EXPECT_EQ(Token::EndOfFile,	token	);
}

