
#include	"trick/RtiStager.hh"
#include	"trick/message_proto.h"
#include	"trick/exec_proto.h"

#include	"trick/memorymanager_c_intf.h"

Trick::RtiStager	*	the_rtis	;

Trick::RtiStager::RtiStager()	:	debug(false)	{
				the_rtis	=	this	;
}

Trick::RtiStager::~RtiStager()	{
				the_rtis	=	NULL	;
}

int	Trick::RtiStager::Add	(char	*variable,	double	value	)	{
				Trick::RtiEventBase	*rei	=	Trick::createRtiEvent	<	double	>	(variable	,	value	)	;
				AddtoList(rei)	;
				return	0	;
}

int	Trick::RtiStager::Add	(char	*variable,	long	long	value	)	{
				Trick::RtiEventBase	*rei	=	Trick::createRtiEvent	<	long	long	>	(variable	,	value	)	;
				AddtoList(rei)	;
				return	0	;
}

int	Trick::RtiStager::Fire	(unsigned	int	thread_id)	{

				pthread_t	pid	=	pthread_self()	;
				std::map	<	pthread_t,	RtiList	*	>::iterator	it	;

				if	(	thread_id	>=	executors.size()	)	{
								message_publish(MSG_ERROR,	"%s:%s:%d	RTI	Fire	failed,	thread	%d	out	of	range\n",
									__FILE__,	__func__,	__LINE__,	thread_id	)	;
								return	-1	;
				}

				if	(	debug	)	{
								message_publish(MSG_DEBUG,	"%s:%s:%d	firing	rti\n",
									__FILE__,	__func__,	__LINE__);
				}

				it	=	list_map.find(pid)	;
				if	(	it	!=	list_map.end()	)	{
								executors[thread_id]->AddToFireList(list_map[pid])	;
								list_map.erase(it)	;
				}	else	{
								message_publish(MSG_ERROR,	"%s:%s:%d	no	list	to	fire\n",
									__FILE__,	__func__,	__LINE__);
				}

				return	0	;
}

int	Trick::RtiStager::List	()	{

				unsigned	int	ii	;
				RtiList	*	temp_list	;
				std::map	<	pthread_t,	RtiList	*	>::iterator	it	;

				it	=	list_map.find(pthread_self())	;

				if	(	it	!=	list_map.end()	)	{
								temp_list	=	list_map[pthread_self()]	;
								for	(	ii	=	0	;	ii	<	temp_list->event_list.size()	;	ii++	)	{
												temp_list->event_list[ii]->print_rti()	;
								}
				}

				return	(0);
}

int	Trick::RtiStager::SetDebug(	bool	on_off	)	{
				debug	=	on_off	;
				std::vector	<	RtiExec	*	>::iterator	reit	;
				for	(	reit	=	executors.begin()	;	reit	!=	executors.end()	;	++reit	)	{
								(*reit)->SetDebug(on_off)	;
				}
				return	0	;
}

int	Trick::RtiStager::SetFrameMultiple(	unsigned	int	thread_id	,	unsigned	int	mult	)	{
				if	(	thread_id	>=	executors.size()	)	{
								return	-1	;
				}
				return	executors[thread_id]->SetFrameMultiple(mult)	;
}

int	Trick::RtiStager::SetFrameOffset(	unsigned	int	thread_id	,	unsigned	int	offset	)	{
				if	(	thread_id	>=	executors.size()	)	{
								return	-1	;
				}
				return	executors[thread_id]->SetFrameOffset(offset)	;
}


void	Trick::RtiStager::AddInjectorExecutor	(	Trick::RtiExec	*rtie	)	{
				executors.push_back(rtie)	;
}

Trick::RtiExec	*	Trick::RtiStager::GetRtiExecutor(	unsigned	int	thread_id	)	{
				if	(	thread_id	>=	executors.size()	)	{
								return	NULL	;
				}
				return	executors[thread_id]	;
}

int	Trick::RtiStager::AddtoList	(Trick::RtiEventBase	*rei	)	{
				if	(rei	!=	NULL)	{
								pthread_t	pid	=	pthread_self()	;

								std::map	<	pthread_t,	RtiList	*	>::iterator	it	;

								it	=	list_map.find(pid)	;
								if	(	it	==	list_map.end()	)	{
												list_map[pid]	=	new	RtiList()	;
								}
								list_map[pid]->event_list.push_back(rei)	;
								if	(	debug	)	{
												message_publish(MSG_DEBUG,	"%s:%s:%d	Event	List	Size	(%d)\n",
													__FILE__,	__func__,	__LINE__,	list_map[pid]->event_list.size());
								}
				}
				return	0	;
}
