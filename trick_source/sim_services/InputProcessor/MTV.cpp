
#include	<iostream>

#include	"trick/MTV.hh"
#include	"trick/VariableServer.hh"
#include	"trick/EventManager_c_intf.hh"

Trick::MTV	*	the_mtv	;

Trick::MTV::MTV()	:
mtv_list(NULL),	mtv_count(0)	{
				the_mtv	=	this	;
}

/**
@details
Adds	an	event	to	mtv	list.

-#	Check	to	see	if	the	event	is	already	in	the	mtv	list,	return	if	it	is.
-#	Increment	the	number	of	events	in	the	mtv	list.
-#	Reallocate	the	mtv_list	to	accomodate	the	new	event.
-#	Assign	the	last	element	of	mtv_list	to	the	incoming	event.
-#	Increment	the	update	ticker	so	mtv	knows	something	changed.
*/
int	Trick::MTV::add_event(	Trick::IPPythonEvent	*	in_event	)	{
				for	(	unsigned	int	ii	=	0	;	ii	<	mtv_count	;	ii++	)	{
								if	(in_event	==	mtv_list[ii])	{
												return	(0)	;
								}
				}
				mtv_count++;
				if	(mtv_count	==	1)	{
								mtv_list	=	(Trick::IPPythonEvent	**)TMM_declare_var_s("Trick::IPPythonEvent*	mtv_list[1]");
				}	else	{
								mtv_list	=	(Trick::IPPythonEvent	**)TMM_resize_array_1d_a(mtv_list,	mtv_count);
				}
				mtv_list[mtv_count-1]	=	in_event	;
				mtv_update_ticker++;

				return	0	;
}

/**
@details
Delete	an	event	from	mtv	list.

-#	If	the	event	is	in	the	mtv	list
	-#	Set	the	event	to	a	dummy	event.
	-#	Increment	the	update	ticker	so	mtv	knows	something	changed.
*/
int	Trick::MTV::delete_event(	Trick::IPPythonEvent	*	in_event	)	{
				unsigned	int	ii	;
				/*	Remove	it	from	mtv's	event	list.	*/
				for	(	ii	=	0	;	ii	<	mtv_count	;	ii++	)	{
								if	(in_event	==	mtv_list[ii])	{
												mtv_list[ii]	=	&dummy_event;
												mtv_update_ticker++;
												break	;
								}
				}
				return	0	;
}

/**
@details
Find	an	event	in	the	mtv	list.

-#	If	the	event	is	in	the	mtv	list	return	it
*/
Trick::IPPythonEvent	*	Trick::MTV::get_event(std::string	event_name)	{
				unsigned	int	ii	;
				Trick::IPPythonEvent*	ret	=	NULL;

				for	(	ii	=	0	;	ii	<	mtv_count	;	ii++	)	{
								if	(	!	mtv_list[ii]->get_name().compare(event_name)	)	{
												ret	=	mtv_list[ii];
												break	;
								}
				}

				return(ret)	;

}

/**
@details
Send	mtv	event	data	through	the	variable	server.

-#	For	each	event	in	the	mtv_list
	-#	call	var_add()	for	all	the	event	information
-#	Send	the	size	of	list	of	variables	we've	added
-#	Send	the	data
-#	Clear	the	var_add	list.
*/
int	Trick::MTV::send_event_data()	{

				char	varname[80];
				const	char	*mtv_name	=	"trick_ip.mtv.mtv_list";
				unsigned	int	ii	;
				int	jj;

				for	(ii=0;	ii	<	mtv_count;	ii++)	{
								snprintf(varname,	sizeof(varname),	"%s[%u][0].name",	mtv_name,ii);
								var_add(varname);
								snprintf(varname,	sizeof(varname),	"%s[%u][0].active",	mtv_name,ii);
								var_add(varname);
								snprintf(varname,	sizeof(varname),	"%s[%u][0].added",	mtv_name,ii);
								var_add(varname);
								snprintf(varname,	sizeof(varname),	"%s[%u][0].condition_count",	mtv_name,ii);
								var_add(varname);
								snprintf(varname,	sizeof(varname),	"%s[%u][0].action_count",	mtv_name,ii);
								var_add(varname);
								snprintf(varname,	sizeof(varname),	"%s[%u][0].before_after",	mtv_name,ii);
								var_add(varname);
								for	(jj=0;	jj<	mtv_list[ii]->condition_count;	jj++)	{
												snprintf(varname,	sizeof(varname),	"%s[%u][0].cond[%d].comment",	mtv_name,ii,jj);
												var_add(varname);
								}
								for	(jj=0;	jj<	mtv_list[ii]->action_count;	jj++)	{
												snprintf(varname,	sizeof(varname),	"%s[%u][0].act[%d].comment",	mtv_name,ii,jj);
												var_add(varname);
								}
								if	(mtv_list[ii]->get_before_after()	>	0)	{
												snprintf(varname,	sizeof(varname),	"%s[%u][0].target_name",	mtv_name,ii);
												var_add(varname);
								}
				}
				var_send_list_size()	;
				var_send()	;
				var_clear()	;

				return	0	;
}

