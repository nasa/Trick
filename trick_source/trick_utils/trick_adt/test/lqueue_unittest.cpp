#include	<gtest/gtest.h>
#include	<iostream>
#include	"trick/lqueue.h"

/*

LQUEUE*	LQ_Create(void);
int	LQ_Delete(LQUEUE*	lqueue);														free	an	lqueue	object	created	by	LQ_Create.
void	LQ_Init(LQUEUE*);
void	LQ_EnQ(void*	pData,LQUEUE*	pQueue);				insert	data	into	queue
void*	LQ_DeQ(LQUEUE*	pQueue);															extract	data	from	queue
void*	LQ_Peek(LQUEUE*	pQueue);														get	data	off	queue	without	extracting
int	LQ_GetCount(LQUEUE*	pQueue);												returns	number	of	items	in	the	queue

BUG:	There's	no	way	to	free	an	LQUEUE	without	a	memory	leak.
*/



TEST(lqueue_test,	LQUEUE_Create)	{
				LQUEUE*	result	=	LQ_Create();
				EXPECT_NE(result,	nullptr);
				EXPECT_EQ(	LQ_GetCount(result),	0);
}

TEST(lqueue_test,	LQUEUE_Delete)	{
				LQUEUE*	lqueue	=	LQ_Create();
				EXPECT_EQ(	LQ_Delete(lqueue),	0);
}

TEST(lqueue_test,	LQUEUE_Delete_null)	{

				LQUEUE*	lqueue	=	nullptr;
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				EXPECT_EQ(	LQ_Delete(lqueue),	-1);
}

TEST(lqueue_test,	LQUEUE_Init)	{

				LQUEUE*	lqueue	=	LQ_Create();
				lqueue->list.count	=	1;
				lqueue->list.head	=	(DLLNODE*)(0xDEADBEEF);
				lqueue->list.tail	=	(DLLNODE*)(0xDEADBEEF);
				LQ_Init(lqueue);
				EXPECT_EQ(lqueue->list.count,	0);
				EXPECT_EQ(lqueue->list.head,	nullptr);
				EXPECT_EQ(lqueue->list.tail,	nullptr);
				EXPECT_EQ(lqueue->list.compare,	nullptr);
				EXPECT_EQ(	LQ_GetCount(lqueue),	0);
}

TEST(lqueue_test,	LQUEUE_Init_null)	{

				LQUEUE*	lqueue	=	nullptr;
				std::cout	<<	"NOTE:	Two	error	messages	are	expected	to	follow."	<<	std::endl;
				LQ_Init(lqueue);

}

TEST(lqueue_test,	LQUEUE_EnQ)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				LQUEUE*	lqueue	=	LQ_Create();
				LQ_EnQ(	&A,	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	1);
				LQ_EnQ(	&B,	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	2);
				LQ_EnQ(	&C,	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	3);
}

TEST(lqueue_test,	LQUEUE_EnQ_null_queue)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				LQUEUE*	lqueue	=	LQ_Create();
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				LQ_EnQ(	&A,	nullptr);

}

TEST(lqueue_test,	LQUEUE_DeQ)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				LQUEUE*	lqueue	=	LQ_Create();
				LQ_EnQ(	&A,	lqueue);
				LQ_EnQ(	&B,	lqueue);
				LQ_EnQ(	&C,	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	3);
				LQ_DeQ(	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	2);
				LQ_DeQ(	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	1);
				LQ_DeQ(	lqueue);
				EXPECT_EQ(	LQ_GetCount(lqueue),	0);
}

TEST(lqueue_test,	LQUEUE_DeQ_null_queue)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				LQUEUE*	lqueue	=	nullptr;
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				LQ_EnQ(	&A,	lqueue);
}

TEST(lqueue_test,	LQUEUE_Peek)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				int*	result;
				LQUEUE*	lqueue	=	LQ_Create();
				LQ_EnQ(	&A,	lqueue);
				LQ_EnQ(	&B,	lqueue);
				LQ_EnQ(	&C,	lqueue);

				EXPECT_EQ(	LQ_GetCount(lqueue),	3);
				result	=	(int*)LQ_Peek(lqueue);
				EXPECT_EQ(*result,	1);
				LQ_DeQ(	lqueue);

				EXPECT_EQ(	LQ_GetCount(lqueue),	2);
				result	=	(int*)LQ_Peek(lqueue);
				EXPECT_EQ(*result,	2);
				LQ_DeQ(	lqueue);

				EXPECT_EQ(	LQ_GetCount(lqueue),	1);
				result	=	(int*)LQ_Peek(lqueue);
				EXPECT_EQ(*result,	3);
				LQ_DeQ(	lqueue);

				EXPECT_EQ(	LQ_GetCount(lqueue),	0);
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)LQ_Peek(lqueue);
				EXPECT_EQ(result,	nullptr);
}

TEST(lqueue_test,	LQUEUE_Peek_null_queue)	{
				int	A,B,C;
				A=1;	B=2;	C=3;
				int*	result	=	(int*)(0xDEADBEEF);;
				LQUEUE*	lqueue	=	nullptr;
				result	=	(int*)LQ_Peek(lqueue);
				EXPECT_EQ(result,	nullptr);
}
