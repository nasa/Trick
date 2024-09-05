#include	<stdio.h>

#include	"trick/RtiEvent.hh"
#include	"trick/message_proto.h"
#include	"trick/memorymanager_c_intf.h"

void	Trick::RtiEventBase::print_rti	()	{

				message_publish	(MSG_NORMAL,	"----------------------------------------\n");
				message_publish	(MSG_NORMAL,	"											reference:	%s\n",	ref->reference);
				message_publish	(MSG_NORMAL,	"											num_index:	%d\n",	ref->num_index);
				message_publish	(MSG_NORMAL,	"															units:	%s\n",	ref->units);
				message_publish	(MSG_NORMAL,	"					pointer_present:	%d\n",	ref->pointer_present);
				message_publish	(MSG_NORMAL,	"													address:	%x\n",	ref->address);
				message_publish	(MSG_NORMAL,	"																type:	%s\n",	ref->attr->type_name);
				message_publish	(MSG_NORMAL,	"															value:	%s\n",	print_val().c_str());
				message_publish	(MSG_NORMAL,	"--====================================--\n");
}

void	Trick::RtiEventBase::check_address()	{
				if	(	ref->pointer_present	)	{
								ref->address	=	follow_address_path(ref)	;
				}
}
