#include	<gtest/gtest.h>
#include	<iostream>
#include	"trick/dllist.h"

/*	==========================================================================
DLL_Delete	deletes	all	of	the	nodes,	and	then	the	list.	Returns	Nothing.
PROTOTYPE:
				void	DLL_Delete(DLLIST*);

NOTE:	The	following	test,	"DLL_Delete_null_list"	exercises	error	handling
				behavior.	DLL_Delete	is	called	in	numerous	other	test	cases	below.
*/

TEST(	dllist_test,	DLL_Delete_null_list)	{
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLL_Init(	nullptr);
				//	ALAS,	there	is	nothing	to	check.
}

/*	==========================================================================
DLL_Create	allocates	and	initializes	an	empty	DLLIST	object.	Returns
the	initialized	list	object.
PROTOTYPE:
				DLLIST*	DLL_Create();
*/

TEST(	dllist_test,	DLL_Create)	{

				DLLIST*	result	=	DLL_Create();

				EXPECT_NE(result,	nullptr);
				EXPECT_EQ(result->count,	0);
				EXPECT_EQ(result->head,	nullptr);
				EXPECT_EQ(result->tail,	nullptr);
				EXPECT_EQ(result->compare,	nullptr);

				DLL_Delete(result);
}

/*	==========================================================================
DLL_Init
Initializes	an	empty	DLLIST	object.
Returns	nothing.

PROTOTYPE:
				void	DLL_Init(DLLIST	*	list)
*/

TEST(	dllist_test,	DLL_Init)	{

				DLLIST	list;
				DLL_Init(	&list);

				EXPECT_EQ(list.count,	0);
				EXPECT_EQ(list.head,	nullptr);
				EXPECT_EQ(list.tail,	nullptr);
				EXPECT_EQ(list.compare,	nullptr);

}

TEST(	dllist_test,	DLL_Init_null_list)	{
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLL_Init(	nullptr);

				//	ALAS,	there	is	nothing	to	check.
}

/*	==========================================================================
DLL_GetCount	returns	the	number	of	elements	in	the	given	list,	unless	list	is
null,	in	which	case	it	returns	-1.
PROTOTYPE:
				int	DLL_GetCount(DLLIST*	list);

NOTE:	The	following	test,	"DLL_GetCount_null_list"	tests	error	handling
behavior.	Normal	behavior	of	DLL_GetCount	is	tested	in:	"DLL_AddHead",
"DLL_AddTail",	"DLL_InsertBefore",	and	"DLL_InsertAfter".
*/

TEST(	dllist_test,	DLL_GetCount_null_list)	{
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				int	result	=	DLL_GetCount(nullptr);
				EXPECT_EQ(result,	-1);
}

/*	==========================================================================
DLL_GetHeadPosition	returns	a	the	head	node	position	of	the	given	list,
unless	the	list	is	null,	in	which	case	it	returns	NULL.
PROTOTYPE:
				DLLPOS	DLL_GetHeadPosition(DLLIST*	list);

NOTE:	The	following	test,	"DLL_GetHeadPosition_null_list"	tests	error	handling
behavior.	Normal	behavior	of	DLL_GetHeadPosition	is	tested	in:	"DLL_AddHead",
"DLL_AddTail",	"DLL_InsertBefore",	and	"DLL_InsertAfter".
*/

TEST(	dllist_test,	DLL_GetHeadPosition_null_list)	{
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLLPOS	result	=	DLL_GetHeadPosition(nullptr);
				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_GetTailPosition	returns	a	the	tail	node	position	of	the	given	list,
unless	the	list	is	null,	in	which	case	it	returns	NULL.
PROTOTYPE:
				DLLPOS	DLL_GetTailPosition(DLLIST*	list);

NOTE:	The	error	case	is	tested	in	"DLL_GetTailPosition_null_list".
Normal	behavior	is	tested	in:	"DLL_AddHead",	"DLL_AddTail",
"DLL_InsertBefore",	and	"DLL_InsertAfter".
*/

TEST(	dllist_test,	DLL_GetTailPosition_null_list)	{
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLLPOS	result	=	DLL_GetTailPosition(nullptr);
				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_AddHead	inserts	an	element	at	the	head	of	the	given	list,	and	returns
a	pointer	to	a	new	node.	If	the	given	list	is	NULL,	an	error	message	is	printed
and	the	function	returns	NULL.

PROTOTYPE:
				DLLPOS	DLL_AddHead(void*	element,	DLLIST*	list);
*/

TEST(	dllist_test,	DLL_AddHead)	{

				int	A[]	=	{1,2,3,4,5};

				/*	Create	a	list.	*/
				DLLIST*	list	=	DLL_Create();
				EXPECT_NE(list,	nullptr);

				/*	Add	a	node	to	the	list.	*/
				DLLPOS	pos_first	=	DLL_AddHead(	&A[0],	list);

				/*	List	State	after	call.
								H	[first]	T
				*/

				EXPECT_NE(pos_first,	nullptr);
				EXPECT_EQ(pos_first->info,	&A[0]);
				EXPECT_EQ(pos_first->prev,	nullptr);
				EXPECT_EQ(pos_first->next,	nullptr);
				EXPECT_EQ(pos_first,	list->head);
				EXPECT_EQ(pos_first,	list->tail);

				/*	Add	a	second	node	to	the	list.	*/
				DLLPOS	pos_second	=	DLL_AddHead(	&A[1],	list);

				/*	Expected	List	State	after	call.
								H	[second]=[first]	T
				*/

				EXPECT_NE(pos_second,	nullptr);
				EXPECT_EQ(pos_second->info,	&A[1]);

				EXPECT_EQ(pos_first->prev,	pos_second);
				EXPECT_EQ(pos_first->next,	nullptr);
				EXPECT_EQ(pos_second->prev,	nullptr);
				EXPECT_EQ(pos_second->next,	pos_first);

				EXPECT_EQ(pos_second,	list->head);
				EXPECT_EQ(pos_first,	list->tail);

				/*	Add	a	third	node	to	the	list.	*/
				DLLPOS	pos_third	=	DLL_AddHead(	&A[2],	list);

				/*	Expected	List	State	after	call.
								H	[third]=[second]=[first]	T
				*/

				EXPECT_NE(pos_third,	nullptr);
				EXPECT_EQ(pos_third->info,	&A[2]);
				EXPECT_EQ(pos_first->prev,	pos_second);
				EXPECT_EQ(pos_first->next,	nullptr);
				EXPECT_EQ(pos_second->prev,	pos_third);
				EXPECT_EQ(pos_second->next,	pos_first);
				EXPECT_EQ(pos_third->prev,	nullptr);
				EXPECT_EQ(pos_third->next,	pos_second);

				EXPECT_EQ(pos_third,	list->head);
				EXPECT_EQ(pos_third,	DLL_GetHeadPosition(list));
				EXPECT_EQ(pos_first,	list->tail);
				EXPECT_EQ(pos_first,	DLL_GetTailPosition(list));

				EXPECT_EQ(list->count,	3);
				EXPECT_EQ(	DLL_GetCount(list),	3);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_AddHead_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLLPOS	result	=	DLL_AddHead(	&A[0],	nullptr);
				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_AddTail	inserts	an	element	at	the	tail	of	the	given	list,	and	returns
a	pointer	to	a	new	node.	If	the	given	list	is	NULL,	an	error	message	is	printed
and	the	functions	returns	NULL.

PROTOTYPE:
				DLLPOS	DLL_AddTail(void*	element,	DLLIST*	list);
*/

TEST(	dllist_test,	DLL_AddTail)	{

				int	A[]	=	{1,2,3,4,5};																		//	These	elements	will	be	added	to	a	list.
				DLLIST*	list	=	DLL_Create();												//	Create	an	empty	list	to	add	things	to.
				EXPECT_NE(list,	nullptr);															//	It	better	not	be	null.

				/*	Add	a	node	to	the	list.	*/
				DLLPOS	pos_first	=	DLL_AddTail(	&A[0],	list);

				/*	List	State	after	call.
								H	[first]	T
				*/

				EXPECT_NE(pos_first,	nullptr);
				EXPECT_EQ(pos_first->info,	&A[0]);

				EXPECT_EQ(pos_first->prev,	nullptr);
				EXPECT_EQ(pos_first->next,	nullptr);

				/*	=====	Add	a	second	node	to	the	list.	=====	*/
				DLLPOS	pos_second	=	DLL_AddTail(	&A[1],	list);

				/*	Expected	List	State	after	call.
								H	[first]=[second]	T
				*/

				EXPECT_NE(pos_second,	nullptr);
				EXPECT_EQ(pos_second->info,	&A[1]);

				EXPECT_EQ(pos_first->prev,	nullptr);
				EXPECT_EQ(pos_first->next,	pos_second);
				EXPECT_EQ(pos_second->prev,	pos_first);
				EXPECT_EQ(pos_second->next,	nullptr);

				/*	=====	CALL:	Add	a	third	node	to	the	list.	=====	*/
				DLLPOS	pos_third	=	DLL_AddTail(	&A[2],	list);

				/*	Expected	List	State	after	call.
								H	[first]=[second]==[third]	T
				*/

				EXPECT_NE(pos_third,	nullptr);
				EXPECT_EQ(pos_third->info,	&A[2]);

				EXPECT_EQ(pos_first->prev,	nullptr);
				EXPECT_EQ(pos_first->next,	pos_second);
				EXPECT_EQ(pos_second->prev,	pos_first);
				EXPECT_EQ(pos_second->next,	pos_third);
				EXPECT_EQ(pos_third->prev,	pos_second);
				EXPECT_EQ(pos_third->next,	nullptr);

				EXPECT_EQ(pos_first,	list->head);
				EXPECT_EQ(pos_first,	DLL_GetHeadPosition(list));
				EXPECT_EQ(pos_third,	list->tail);
				EXPECT_EQ(pos_third,	DLL_GetTailPosition(list));

				EXPECT_EQ(list->count,	3);
				EXPECT_EQ(	DLL_GetCount(list),	3);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_AddTail_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLLPOS	result	=	DLL_AddTail(	&A[0],	nullptr);
				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_InsertBefore	inserts	the	given	data	element	before	the	given	position,	of
the	given	list.	Returns	the	position	of	the	inserted	element	node.

PROTOTYPE:
				DLLPOS	DLL_InsertBefore(DLLPOS	pos,	void*	element,	DLLIST*	list);
*/

TEST(	dllist_test,	DLL_InsertBefore)	{

				int	A[]	=	{1,2,3,4,5};
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=	DLL_AddHead(	&A[0],	list);
				DLLPOS	pos_second	=	DLL_AddHead(	&A[1],	list);

				/*	Initial	List	State
								H	[second]=[first]	T
				*/

				/*	=====	CALL:	Add	a	third	node	to	the	list.	=====	*/
				DLLPOS	pos_third	=	DLL_InsertBefore(pos_first,	&A[2],	list);

				/*	Expected	List	State	after	the	call:
								H	[second]=[third]=[first]	T
				*/

				EXPECT_NE(pos_third,	nullptr);
				EXPECT_EQ(pos_third->info,	&A[2]	);

				EXPECT_EQ(pos_first->prev,	pos_third	);
				EXPECT_EQ(pos_first->next,	nullptr	);
				EXPECT_EQ(pos_second->prev,	nullptr	);
				EXPECT_EQ(pos_second->next,	pos_third	);
				EXPECT_EQ(pos_third->prev,	pos_second	);
				EXPECT_EQ(pos_third->next,	pos_first	);

				/*	=====	CALL:	Add	a	fourth	node	to	the	beginning	of	the	list.	=====	*/
				DLLPOS	pos_fourth	=	DLL_InsertBefore(pos_second,	&A[3],	list);

				/*	Expected	List	State	after	the	call:
								H	[fourth]=[second]=[third]=[first]	T
				*/

				EXPECT_NE(pos_fourth,	nullptr);
				EXPECT_EQ(pos_fourth->info,	&A[3]	);

				EXPECT_EQ(pos_first->prev,	pos_third	);
				EXPECT_EQ(pos_first->next,	nullptr	);
				EXPECT_EQ(pos_second->prev,	pos_fourth	);
				EXPECT_EQ(pos_second->next,	pos_third	);
				EXPECT_EQ(pos_third->prev,	pos_second	);
				EXPECT_EQ(pos_third->next,	pos_first	);
				EXPECT_EQ(pos_fourth->next,	pos_second	);
				EXPECT_EQ(pos_fourth->prev,	nullptr	);

				EXPECT_EQ(pos_fourth,	list->head);
				EXPECT_EQ(pos_fourth,	DLL_GetHeadPosition(list));
				EXPECT_EQ(pos_first,	list->tail);
				EXPECT_EQ(pos_first,	DLL_GetTailPosition(list));

				EXPECT_EQ(list->count,	4);
				EXPECT_EQ(	DLL_GetCount(list),	4);

				DLL_Delete(list);
}

/*	==========================================================================
DLL_InsertAfter	inserts	the	given	data	element	after	the	given	position,	of
the	given	list.	Returns	the	position	of	the	inserted	element	node.

PROTOTYPE:
				DLLPOS	DLL_InsertBefore(DLLPOS	pos,	void*	element,	DLLIST*	list);
=============================================================================
*/
TEST(	dllist_test,	DLL_InsertAfter)	{

				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=	DLL_AddHead(	&A[0],	list);
				DLLPOS	pos_second	=	DLL_AddHead(	&A[1],	list);

				/*	Initial	List	State
								H	[second]=[first]	T
				*/

				/*	Add	a	third	node.	*/
				DLLPOS	pos_third	=	DLL_InsertAfter(pos_first,	&A[2],	list);

				/*	Expected	State	of	List	after	the	call:
								H	[second]=[first]=[third]	T
				*/

				EXPECT_NE(pos_third,	nullptr);
				EXPECT_EQ(pos_third->info,	&A[2]	);

				EXPECT_EQ(pos_first->prev,		pos_second	);
				EXPECT_EQ(pos_first->next,	pos_third	);
				EXPECT_EQ(pos_second->prev,	nullptr	);
				EXPECT_EQ(pos_second->next,	pos_first	);
				EXPECT_EQ(pos_third->prev,	pos_first	);
				EXPECT_EQ(pos_third->next,	nullptr	);

				/*	=====	CALL:	Add	a	fourth	node	=====	*/
				DLLPOS	pos_fourth	=	DLL_InsertAfter(pos_second,	&A[3],	list);

				/*	Expected	State	of	List	after	the	call:
								H	[second]=[fourth]=[first]=[third]	T
				*/

				EXPECT_NE(pos_fourth,	nullptr);
				EXPECT_EQ(pos_fourth->info,	&A[3]	);

				EXPECT_EQ(pos_first->prev,		pos_fourth	);
				EXPECT_EQ(pos_first->next,	pos_third	);
				EXPECT_EQ(pos_second->prev,	nullptr	);
				EXPECT_EQ(pos_second->next,	pos_fourth	);
				EXPECT_EQ(pos_third->prev,	pos_first	);
				EXPECT_EQ(pos_third->next,	nullptr	);
				EXPECT_EQ(pos_fourth->next,	pos_first	);
				EXPECT_EQ(pos_fourth->prev,	pos_second	);

				EXPECT_EQ(pos_second,	list->head);
				EXPECT_EQ(pos_second,	DLL_GetHeadPosition(list));
				EXPECT_EQ(pos_third,	list->tail);
				EXPECT_EQ(pos_third,	DLL_GetTailPosition(list));

				EXPECT_EQ(list->count,	4);
				EXPECT_EQ(	DLL_GetCount(list),	4);

				DLL_Delete(list);
}

/*	==========================================================================
DLL_GetNext	returns	a	pointer	to	the	data	of	the	DLLPOS	pointed
to	by	pos_p,	and	updates	the	DLLPOS	referenced	by	pos_p	to	the
next	DLLPOS	in	the	linked	list.

PROTOTYPE:
				void*	DLL_GetNext(DLLPOS*	pos_p	,DLLIST*);
=============================================================================*/

TEST(	dllist_test,	DLL_GetNext)	{

				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=		DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);
				(void)	DLL_AddTail(	&(A[2]),	list);
				(void)	DLL_AddTail(	&(A[3]),	list);
				(void)	DLL_AddTail(	&(A[4]),	list);

				/*	Expected	List	State	after	call.
								H	[first]=[second]=[third]=[fourth]=[fifth]	T
													|								|							|							|								|
												[1]						[2]					[3]					[4]						[5]
				*/

				DLLPOS	current_pos	=	pos_first;

				int	*result;
				result	=	(int*)DLL_GetNext(&current_pos,	list);
				EXPECT_NE(current_pos,	nullptr);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_GetNext(&current_pos,	list);
				EXPECT_EQ(*result,	2);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetNext(&current_pos,	list);
				EXPECT_EQ(*result,	3);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetNext(&current_pos,	list);
				EXPECT_EQ(*result,	4);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetNext(&current_pos,	list);
				EXPECT_EQ(*result,	5);
				EXPECT_EQ(current_pos,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetNext_null_pos)	{
				int	A[]	=	{1,2,3,4,5};
				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				void*	result	=	DLL_GetNext(nullptr,	list);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetNext_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=		DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);
				DLLPOS	current_pos	=	pos_first;

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				void*	result	=	DLL_GetNext(&current_pos,	nullptr);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}


/*	==========================================================================
DLL_GetPrev	returns	a	pointer	to	the	node	data	of	the	DLLPOS	pointed
to	by	pos_p,	and	updates	the	DLLPOS	referenced	by	pos_p	to	the
previous	DLLPOS	in	the	linked	list.
PROTOTYPE:
				void*	DLL_GetPrev(DLLPOS*	pos_p	,DLLIST*);
=============================================================================*/

TEST(	dllist_test,	DLL_GetPrev)	{

				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=		DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);
				(void)	DLL_AddHead(	&(A[2]),	list);
				(void)	DLL_AddHead(	&(A[3]),	list);
				(void)	DLL_AddHead(	&(A[4]),	list);

				/*	Expected	List	State.
								H	[fifth]=[fourth]=[third]=[second]=[first]	T
													|								|							|							|								|
												[5]						[4]					[3]					[2]						[1]
				*/

				DLLPOS	current_pos	=	pos_first;

				int	*result;
				result	=	(int*)DLL_GetPrev(&current_pos,	list);
				EXPECT_NE(current_pos,	nullptr);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_GetPrev(&current_pos,	list);
				EXPECT_EQ(*result,	2);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetPrev(&current_pos,	list);
				EXPECT_EQ(*result,	3);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetPrev(&current_pos,	list);
				EXPECT_EQ(*result,	4);
				EXPECT_NE(current_pos,	nullptr);
				result	=	(int*)DLL_GetPrev(&current_pos,	list);
				EXPECT_EQ(*result,	5);
				EXPECT_EQ(current_pos,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetPrev_null_pos)	{
				int	A[]	=	{1,2,3,4,5};
				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				/*	Call	function	under	test	with	pos	==	NULL.	*/
				void*	result	=	DLL_GetPrev(nullptr,	list);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetPrev_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=		DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);
				DLLPOS	current_pos	=	pos_first;

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				/*	Call	function	under	test	with	list	==	NULL.	*/
				void*	result	=	DLL_GetPrev(&current_pos,	nullptr);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

/*	==========================================================================
DLL_Find	returns	a	pointer	to	data	stored	in	the	list	that	matches	the
specified	data.	Return	NULL	if	the	list	is	NULL,	or	if	the	specified	data	is
not	found	in	the	list.

DLL_Find	requires	that	a	pointer	to	a	compare	function	must	be	set	in	the
DLLIST	structure.	The	prototype	of	a	suitable	compare	function	is:
				int	(*compare)(void*	info1,void*	info2);
The	function	should	return	1	if	the	data	match	or	0	if	they	don't.

The	test	below	can	serve	as	an	example.

PROTOTYPE:
				void*	DLL_Find(void*	data,	DLLIST*	list);
=============================================================================*/

/*	The	following	is	a	comparison	function	used	in	the	DLL_Find	test.	*/
int	IntCompare(void*	a,	void*	b	)	{
				if	(	*(int*)a	==	*(int*)b	)	{
								return	1;
				}	else	{
								return	0;
				}
}

TEST(	dllist_test,	DLL_Find)	{
				int	A[]	=	{1,2,3,4,5};
				int	waldo	=	0;
				void*	result	=	nullptr;

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);
				(void)	DLL_AddHead(	&(A[2]),	list);
				(void)	DLL_AddHead(	&(A[3]),	list);
				(void)	DLL_AddHead(	&(A[4]),	list);

				//	DLL_Find	requires	the	list	to	have	a	compare	function.
				list->compare	=		&IntCompare;

				waldo	=	1;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	&(A[0]));
				waldo	=	2;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	&(A[1]));
				waldo	=	3;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	&(A[2]));
				waldo	=	4;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	&(A[3]));
				waldo	=	5;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	&(A[4]));
				waldo	=	7;
				result	=	DLL_Find(	&waldo,	list	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_Find_null_list)	{
				int	waldo	=	1;
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				/*	Call	function	under	test	with	list	==	NULL.	*/
				void*	result	=	DLL_Find(&waldo,	nullptr);
				EXPECT_EQ(result,	nullptr);
}

TEST(	dllist_test,	DLL_Find_null_compare_function)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				int	waldo	=	1;
				void*	result	=	DLL_Find(	&waldo,	list	);
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

/*	==========================================================================
DLL_GetAt	returns	a	pointer	to	the	data	of	the	node	specified	by	pos.
Return	NULL	if	list	is	NULL,	pos	is	NULL,	or	if	the	node	specified	by	pos	is
not	contained	in	the	list.
PROTOTYPE:
				void*	DLL_GetAt(DLLPOS	pos,	DLLIST*	list);
*/

TEST(	dllist_test,	DLL_GetAt)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=	DLL_AddHead(	&(A[0]),	list);
				DLLPOS	pos_second	=	DLL_AddHead(	&(A[1]),	list);
				DLLPOS	pos_third	=	DLL_AddHead(	&(A[2]),	list);
				DLLPOS	pos_fourth	=	DLL_AddHead(	&(A[3]),	list);
				DLLPOS	pos_fifth	=	DLL_AddHead(	&(A[4]),	list);

				result	=	(int*)DLL_GetAt(	pos_first,	list	);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_GetAt(	pos_second,	list	);
				EXPECT_EQ(*result,	2);
				result	=	(int*)DLL_GetAt(	pos_third,	list	);
				EXPECT_EQ(*result,	3);
				result	=	(int*)DLL_GetAt(	pos_fourth,	list	);
				EXPECT_EQ(*result,	4);
				result	=	(int*)DLL_GetAt(	pos_fifth,	list	);
				EXPECT_EQ(*result,	5);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetAt_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;

				DLLIST*	null_list	=	nullptr;
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first		=	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_GetAt(	pos_first,	null_list	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetAt_null_pos)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;
				DLLPOS	null_pos	=	nullptr;
				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_GetAt(	null_pos,	list	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_GetAt_list_doesnt_contain_node)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;

				DLLIST*	list1	=	DLL_Create();
				DLLPOS	pos_in_list1	=	DLL_AddHead(	&(A[0]),	list1);
				DLLIST*	list2	=	DLL_Create();
				DLLPOS	pos_in_list2	=	DLL_AddHead(	&(A[0]),	list2);

				//	These	work.
				result	=	(int*)DLL_GetAt(	pos_in_list1,	list1	);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_GetAt(	pos_in_list2,	list2	);
				EXPECT_EQ(*result,	1);

				//	These	don't.
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_GetAt(	pos_in_list1,	list2	);
				EXPECT_EQ(result,	nullptr);
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_GetAt(	pos_in_list2,	list1	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list1);
				DLL_Delete(list2);
}

/*	==========================================================================
DLL_SetAt	sets	the	data	of	the	node	specified	by	pos	to	info,	and	return	the
previous	info.	Return	NULL	if	list	is	NULL,	pos	is	NULL,	or	if	the	node
specified	by	pos	is	not	contained	in	the	list.
PROTOTYPE:
				void	*DLL_SetAt(DLLPOS	pos,	void	*info,	DLLIST	*	list)
*/

TEST(	dllist_test,	DLL_SetAt)	{
				int	A[]	=	{1,2,3,4,5};
				int	B[]	=	{6,7,8,9,10};
				int*	result;
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first	=	DLL_AddHead(	&(A[0]),	list);
				DLLPOS	pos_second	=	DLL_AddHead(	&(A[1]),	list);
				DLLPOS	pos_third	=	DLL_AddHead(	&(A[2]),	list);
				DLLPOS	pos_fourth	=	DLL_AddHead(	&(A[3]),	list);
				DLLPOS	pos_fifth	=	DLL_AddHead(	&(A[4]),	list);

				result	=	(int*)DLL_SetAt(	pos_first,		&(B[0]),	list	);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_SetAt(	pos_second,	&(B[1]),	list	);
				EXPECT_EQ(*result,	2);
				result	=	(int*)DLL_SetAt(	pos_third,		&(B[2]),	list	);
				EXPECT_EQ(*result,	3);
				result	=	(int*)DLL_SetAt(	pos_fourth,	&(B[3]),	list	);
				EXPECT_EQ(*result,	4);
				result	=	(int*)DLL_SetAt(	pos_fifth,		&(B[4]),	list	);
				EXPECT_EQ(*result,	5);
				result	=	(int*)DLL_GetAt(	pos_first,	list	);
				EXPECT_EQ(*result,	6);
				result	=	(int*)DLL_GetAt(	pos_second,	list	);
				EXPECT_EQ(*result,	7);
				result	=	(int*)DLL_GetAt(	pos_third,	list	);
				EXPECT_EQ(*result,	8);
				result	=	(int*)DLL_GetAt(	pos_fourth,	list	);
				EXPECT_EQ(*result,	9);
				result	=	(int*)DLL_GetAt(	pos_fifth,	list	);
				EXPECT_EQ(*result,	10);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_SetAt_null_list)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;

				DLLIST*	null_list	=	nullptr;
				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first		=	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_SetAt(	pos_first,	&(A[0]),	null_list	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_SetAt_null_pos)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;

				DLLPOS	null_pos	=	nullptr;
				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddHead(	&(A[0]),	list);
				(void)	DLL_AddHead(	&(A[1]),	list);

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_SetAt(	null_pos,	&(A[0]),	list	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_SetAt_list_doesnt_contain_node)	{
				int	A[]	=	{1,2,3,4,5};
				int*	result;

				DLLIST*	list1	=	DLL_Create();
				DLLPOS	pos_in_list1	=	DLL_AddHead(	&(A[0]),	list1);
				DLLIST*	list2	=	DLL_Create();
				DLLPOS	pos_in_list2	=	DLL_AddHead(	&(A[0]),	list2);

				//	These	should	work.
				result	=	(int*)DLL_SetAt(	pos_in_list1,	&(A[1]),	list1	);
				EXPECT_EQ(*result,	1);
				result	=	(int*)DLL_SetAt(	pos_in_list2,	&(A[1]),	list2	);
				EXPECT_EQ(*result,	1);

				//	These	shouldn't,	because	the	specified	nodes	are	not	in	the	specified	lists.
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_SetAt(	pos_in_list1,	&(A[1]),	list2	);
				EXPECT_EQ(result,	nullptr);
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_SetAt(	pos_in_list2,	&(A[1]),	list1	);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list1);
				DLL_Delete(list2);
}

/*	==========================================================================
DLL_FindPos
Return	the	position,	in	the	specified	list,	that	holds	the	specified	data
address.	If	the	list	is	NULL,	or	the	data	address	isn't	found,	return	NULL.
PROTOTYPE:
				DLLPOS	DLL_FindPos(void*	data,	DLLIST*);
*/

TEST(	dllist_test,	DLL_FindPos)	{
				int	A[]	=	{1,2,3,4,5};
				int	B	=	7;

				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first		=	DLL_AddHead(	&(A[0]),	list);
				DLLPOS	pos_second	=	DLL_AddHead(	&(A[1]),	list);
				DLLPOS	pos_third		=	DLL_AddHead(	&(A[2]),	list);
				DLLPOS	pos_fourth	=	DLL_AddHead(	&(A[3]),	list);
				DLLPOS	pos_fifth		=	DLL_AddHead(	&(A[4]),	list);

				DLLPOS	result	=	nullptr;
				result	=	DLL_FindPos(	&(A[0]),	list);
				EXPECT_EQ(result,	pos_first);
				result	=	DLL_FindPos(	&(A[1]),	list);
				EXPECT_EQ(result,	pos_second);
				result	=	DLL_FindPos(	&(A[2]),	list);
				EXPECT_EQ(result,	pos_third);
				result	=	DLL_FindPos(	&(A[3]),	list);
				EXPECT_EQ(result,	pos_fourth);
				result	=	DLL_FindPos(	&(A[4]),	list);
				EXPECT_EQ(result,	pos_fifth);

				/*	B	isn't	in	the	list.	*/
				result	=	DLL_FindPos(	&B,	list);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_FindPos_null_list)	{
				int	B	=	7;
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				/*	Call	function	under	test	with	list	==	NULL.	*/
				DLLPOS	result	=	DLL_FindPos(&B,	nullptr);
				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_FindIndex
Return	the	node	at	the	given	index	(n),	where	n	ranges	from	0	{the	head	node}
to	(list->count	-	1)	{The	tail	node}.
PROTOTYPE:
				DLLPOS	DLL_FindIndex(int	n,	DLLIST	*	list)
*/

TEST(	dllist_test,	DLL_FindIndex)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				DLLPOS	pos_first		=	DLL_AddTail(	&(A[0]),	list);
				DLLPOS	pos_second	=	DLL_AddTail(	&(A[1]),	list);
				DLLPOS	pos_third		=	DLL_AddTail(	&(A[2]),	list);
				DLLPOS	pos_fourth	=	DLL_AddTail(	&(A[3]),	list);
				DLLPOS	pos_fifth		=	DLL_AddTail(	&(A[4]),	list);

				DLLPOS	result	=	nullptr;
				result	=	DLL_FindIndex(	0,	list);
				EXPECT_EQ(result,	pos_first);
				result	=	DLL_FindIndex(	1,	list);
				EXPECT_EQ(result,	pos_second);
				result	=	DLL_FindIndex(	2,	list);
				EXPECT_EQ(result,	pos_third);
				result	=	DLL_FindIndex(	3,	list);
				EXPECT_EQ(result,	pos_fourth);
				result	=	DLL_FindIndex(	4,	list);
				EXPECT_EQ(result,	pos_fifth);

				/*	B	isn't	in	the	list.	*/
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	DLL_FindIndex(	6,	list);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_FindIndex_null_list)	{

				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				DLLPOS	result	=	DLL_FindIndex(0,	nullptr);

				EXPECT_EQ(result,	nullptr);
}

/*	==========================================================================
DLL_RemoveAt
Remove	the	spcified	node	(pos)	Node	from	the	specified	list.
Reurn	the	info	at	the	position	being	removed,	or	NULL	if	it's	not	found	in	the
list.
PROTOTYPE:
				void	*DLL_RemoveAt(DLLPOS	pos,	DLLIST	*	list)
*/

TEST(	dllist_test,	DLL_RemoveAt_head)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);
				(void)	DLL_AddTail(	&(A[2]),	list);

				DLLPOS	nodeToRemove	=	DLL_GetHeadPosition(list);
				int*	result	=	nullptr;

				result	=	(int*)DLL_RemoveAt(nodeToRemove,	list);
				EXPECT_EQ(*result,	1);
				EXPECT_EQ(	DLL_GetCount(list),	2);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_RemoveAt_tail)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);
				(void)	DLL_AddTail(	&(A[2]),	list);

				DLLPOS	nodeToRemove	=	DLL_GetTailPosition(list);
				int*	result	=	nullptr;

				result	=	(int*)DLL_RemoveAt(nodeToRemove,	list);
				EXPECT_EQ(*result,	3);
				EXPECT_EQ(	DLL_GetCount(list),	2);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_RemoveAt_from_middle)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);
				DLLPOS	nodeToRemove	=	DLL_AddTail(	&(A[1]),	list);
				(void)	DLL_AddTail(	&(A[2]),	list);
				EXPECT_EQ(	DLL_GetCount(list),	3);

				int*	result	=	nullptr;
				result	=	(int*)DLL_RemoveAt(nodeToRemove,	list);
				EXPECT_EQ(*result,	2);
				EXPECT_EQ(	DLL_GetCount(list),	2);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_RemoveAt_from_single_node_list)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);

				DLLPOS	nodeToRemove	=	DLL_GetTailPosition(list);
				int*	result	=	nullptr;

				result	=	(int*)DLL_RemoveAt(nodeToRemove,	list);
				EXPECT_EQ(*result,	1);
				EXPECT_EQ(	DLL_GetCount(list),	0);

				DLL_Delete(list);
}

TEST(	dllist_test,	DLL_RemoveAt_null_list)	{

				DLLIST*	list	=	nullptr;
				DLLPOS	nodeToRemove	=	nullptr;
				int*	result	=	nullptr;
				std::cout	<<	"NOTE:	An	error	message	is	expected	to	follow."	<<	std::endl;
				result	=	(int*)DLL_RemoveAt(nodeToRemove,	list);
				EXPECT_EQ(result,	nullptr);

				DLL_Delete(list);
}

/*	==========================================================================
DLL_RemoveAll
Removes	all	elements	in	the	list.	The	list	will	then	be	empty.
Returns	nothing.
PROTOTYPE:
				void	DLL_RemoveAll(DLLIST	*	list)
*/

TEST(	dllist_test,	DLL_RemoveAll)	{
				int	A[]	=	{1,2,3,4,5};

				DLLIST*	list	=	DLL_Create();
				(void)	DLL_AddTail(	&(A[0]),	list);
				(void)	DLL_AddTail(	&(A[1]),	list);
				(void)	DLL_AddTail(	&(A[2]),	list);
				(void)	DLL_AddTail(	&(A[3]),	list);
				(void)	DLL_AddTail(	&(A[4]),	list);
				EXPECT_EQ(	DLL_GetCount(list),	5);

				DLL_RemoveAll(list);

				EXPECT_EQ(	DLL_GetCount(list),	0);

				DLL_Delete(list);

}

//	END	OF	TEST	SUITE
