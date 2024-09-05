
#include	"trick/RtiExec.hh"
#include	"trick/message_proto.h"
#include	"trick/exec_proto.h"

Trick::RtiExec::RtiExec()	:
	frame_multiple(1)	,
	frame_offset(0)	,
	debug(false)	{
				pthread_mutex_init(&list_mutex,	NULL)	;
}

Trick::RtiExec::~RtiExec()	{}

/**
@details
-#	lock	the	file_list	mutex
-#	add	the	new	list	to	the	fire_list
-#	unlock	the	file_list	mutex
*/
void	Trick::RtiExec::AddToFireList(	RtiList	*	rti_list	)	{
				pthread_mutex_lock(&list_mutex)	;
				fire_list.push_back(rti_list)	;
				pthread_mutex_unlock(&list_mutex)	;
}

/**
@details
-#	If	the	current	frame	is	the	correct	frame	multiple	and	offset
-#	If	there	is	a	list	of	injections	in	the	fire_list
-#	Try	to	unlock	the	file_list	mutex
-#	If	successful,	execute	the	fire_list	and	clear	the	list
-#	unlock	the	mutex
*/
int	Trick::RtiExec::Exec	()	{
				long	long	curr_frame	=	exec_get_frame_count()	;
				curr_frame	%=	frame_multiple	;
				if	(	curr_frame	==	frame_offset	)	{
								if	(	!	fire_list.empty()	)	{
												//	use	trylock	so	we	don't	block	if	the	mutex	is	in	use.
												if	(	pthread_mutex_trylock(&list_mutex)	==	0	)	{
																std::vector	<	RtiList	*	>::iterator	rlit	;
																for	(	rlit	=	fire_list.begin()	;	rlit	!=	fire_list.end()	;	++rlit	)	{
																				(*rlit)->execute(debug)	;
																				delete	(*rlit)	;
																}
																fire_list.clear()	;
																pthread_mutex_unlock(&list_mutex)	;
												}
								}
				}
				return	(0);
}

void	Trick::RtiExec::SetDebug(	bool	on_off	)	{
				debug	=	on_off	;
}

int	Trick::RtiExec::SetFrameMultiple(	unsigned	int	mult	)	{
				frame_multiple	=	mult	;
				return	0	;
}

int	Trick::RtiExec::SetFrameOffset(	unsigned	int	offset	)	{
				if	(	offset	>=	frame_multiple	)	{
								message_publish(MSG_ERROR,	"RTI	frame	offset	must	be	less	than	frame_multiple\n")	;
								return	-1	;
				}
				frame_offset	=	offset	;
				return	0	;
}


