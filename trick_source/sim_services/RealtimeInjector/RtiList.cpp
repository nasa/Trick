/*
********************************
**	PURPOSE:	(communications)
********************************/
#include	"trick/RtiList.hh"
#include	"trick/message_proto.h"

void	Trick::RtiList::execute(	bool	debug	)	{
				std::vector<RtiEventBase	*	>::iterator	rebit	;
				for	(	rebit	=	event_list.begin()	;	rebit	!=	event_list.end()	;	++rebit	)	{
								if	(	debug	)	{
												message_publish(MSG_DEBUG,	"Executing	RTI:	%s	=	%s\n",
													(*rebit)->ref->reference	,	(*rebit)->print_val().c_str()	);
								}
								(*rebit)->do_assignment()	;
								delete	(*rebit)	;
				}
}


void	Trick::RtiList::print_list()	{
				std::vector<RtiEventBase	*	>::iterator	rebit	;
				for	(	rebit	=	event_list.begin()	;	rebit	!=	event_list.end()	;	++rebit	)	{
								(*rebit)->print_rti()	;
				}
}
