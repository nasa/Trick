
#include	<stdio.h>

#ifdef	HAVE_ZEROCONF
#if	__linux
#include	<avahi-client/client.h>
#include	<avahi-client/publish.h>
#include	<avahi-common/simple-watch.h>
#include	<avahi-common/alternative.h>
#include	<avahi-common/error.h>
#include	<avahi-common/malloc.h>
#endif
#if	__APPLE__
#include	<dns_sd.h>
#endif
#endif

#include	"trick/Zeroconf.hh"
#include	"trick/variable_server_proto.h"

Trick::Zeroconf::Zeroconf()	{
				name	=	std::string("My	Sim!!!")	;
				type	=	std::string("_trick-vs._tcp")	;

#ifdef	HAVE_ZEROCONF
#if	__linux
				simple_poll	=	NULL	;
				client	=	NULL	;
				group	=	NULL	;
#endif
#endif

}

#ifdef	HAVE_ZEROCONF
#if	__linux
extern	"C"	void	entry_group_callback(AvahiEntryGroup	*g,	AvahiEntryGroupState	state,	void	*userdata)	;

extern	"C"	{
void	entry_group_callback(AvahiEntryGroup	*g,	AvahiEntryGroupState	state,	void	*userdata)	{

				/*	Called	whenever	the	entry	group	state	changes	*/
				char	*	name	=	(char	*)userdata;

				switch	(state)	{
								case	AVAHI_ENTRY_GROUP_ESTABLISHED	:
												/*	The	entry	group	has	been	established	successfully	*/
												fprintf(stderr,	"Service	'%s'	successfully	established.\n",	name);
												break;

								case	AVAHI_ENTRY_GROUP_COLLISION	:
												break;

								case	AVAHI_ENTRY_GROUP_FAILURE	:
												fprintf(stderr,	"Entry	group	failure:	%s\n",	avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));
												break;

								case	AVAHI_ENTRY_GROUP_UNCOMMITED:
								case	AVAHI_ENTRY_GROUP_REGISTERING:
												break;
				}
}
}
#endif
#endif

int	Trick::Zeroconf::init()	{

#ifdef	HAVE_ZEROCONF
#if	__linux
				int	error;
				int	ret	;
				char	r[128]	;

				if	(!(simple_poll	=	avahi_simple_poll_new()))	{
								fprintf(stderr,	"Failed	to	create	simple	poll	object.\n");
								return	-1	;
				}

				client	=	avahi_client_new(avahi_simple_poll_get(simple_poll),	(AvahiClientFlags)0,	NULL,	NULL,	&error);

				if	(	client	!=	NULL	)	{
								group	=	avahi_entry_group_new(client,	entry_group_callback,	(void	*)name.c_str())	;
								if	(avahi_entry_group_is_empty(group))	{
												ret	=	avahi_entry_group_add_service(group,	AVAHI_IF_UNSPEC,	AVAHI_PROTO_UNSPEC,	(AvahiPublishFlags)0,
																name.c_str(),	type.c_str(),	NULL,	NULL,	var_server_get_port(),	NULL,	r,	NULL)	;
												ret	=	avahi_entry_group_commit(group)	;
								}
				}
#endif
#if	__APPLE__
				DNSServiceErrorType	error	;

				error	=	DNSServiceRegister(&dns_service_ref,
																									0,	0,
																									name.c_str(),	type.c_str(),
																									NULL,	NULL,
																									htons(var_server_get_port()),
																									0,	NULL,	//	txt	record	stuff
																									NULL,	NULL);	//	callback	stuff
#endif
#endif

				return	0	;

}

