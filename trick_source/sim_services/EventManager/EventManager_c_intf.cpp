
#include	<string>

#include	"trick/Event.hh"
#include	"trick/EventManager.hh"
#include	"trick/EventManager_c_intf.hh"

extern	Trick::EventManager	*	the_em	;

/*	Get	an	event	by	name	*/
Trick::Event	*	event_manager_get_event(	std::string	in_name	)	{
				if	(	the_em	!=	NULL	)	{
								return	the_em->get_event(in_name)	;
				}
				return	NULL	;
}

/*	Add	an	event	*/
int	event_manager_add_event(	Trick::Event	*	in_event	)	{
				if	(	the_em	!=	NULL	)	{
								return	the_em->add_event(in_event)	;
				}
				return	-1	;
}

/*	Add	an	event	before	a	target	job	*/
int	event_manager_add_event_before(	Trick::Event	*	in_event,	std::string	target_name,	unsigned	int	target_inst	)	{
				if	(	the_em	!=	NULL	)	{
								return	the_em->add_event_before(in_event,	target_name,	target_inst)	;
				}
				return	-1	;
}

/*	Add	an	event	after	a	target	job	*/
int	event_manager_add_event_after(	Trick::Event	*	in_event,	std::string	target_name,	unsigned	int	target_inst	)	{
				if	(	the_em	!=	NULL	)	{
								return	the_em->add_event_after(in_event,	target_name,	target_inst)	;
				}
				return	-1	;
}

/*	Remove	an	event	*/
int	event_manager_remove_event(	Trick::Event	*	in_event	)	{
				if	(	the_em	!=	NULL	)	{
								return	the_em->remove_event(in_event)	;
				}
				return	-1	;
}

