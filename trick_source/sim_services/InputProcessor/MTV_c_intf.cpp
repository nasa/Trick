
#include	"trick/MTV.hh"

extern	Trick::MTV	*	the_mtv	;

/**
@details
-#	Add	an	event	to	the	mtv	list
*/
int	mtv_add_event(Trick::IPPythonEvent	*	in_event)	{
				if	(	the_mtv	!=	NULL	)	{
								the_mtv->add_event(in_event)	;
				}
				return	0	;
}

/**
@details
-#	Delete	an	event	from	the	mtv	list
*/
int	mtv_delete_event(Trick::IPPythonEvent	*	in_event)	{
				if	(	the_mtv	!=	NULL	)	{
								the_mtv->delete_event(in_event)	;
				}
				return	0	;
}

/**
@details
-#	Get	an	event	by	name	from	the	mtv	list
*/
Trick::IPPythonEvent	*	mtv_get_event(std::string	event_name)	{
				if	(	the_mtv	!=	NULL	)	{
								return	the_mtv->get_event(event_name)	;
				}
				return	NULL	;
}

/**
@details
-#	Sends	the	mtv	list	information	through	the	variable	server
*/
int	mtv_send_event_data()	{
				if	(	the_mtv	!=	NULL	)	{
								the_mtv->send_event_data()	;
				}
				return	0	;
}

