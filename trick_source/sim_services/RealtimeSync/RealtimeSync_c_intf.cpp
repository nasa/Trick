
#include	<stdio.h>
#include	"trick/RealtimeSync.hh"
#include	"trick/realtimesync_proto.h"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"


/*	Global	singleton	pointer	to	the	real-time	synchronization	*/
extern	Trick::RealtimeSync	*	the_rts	;

/*************************************************************************/
/*	These	routines	are	the	"C"	interface	to	the	real-time	synchronization	*/
/*************************************************************************/

/**
	*	@relates	Trick::RealtimeSync
	*	@copydoc	Trick::RealtimeSync::enable
	*	C	wrapper	for	Trick::RealtimeSync::enable
	*/
extern	"C"	int	real_time_enable()	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->enable()	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::RealtimeSync
	*	@copydoc	Trick::RealtimeSync::disable
	*	C	wrapper	for	Trick::RealtimeSync::disable
	*/
extern	"C"	int	real_time_disable()	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->disable()	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::RealtimeSync
	*	@copydoc	Trick::RealtimeSync::start_realtime
	*	C	wrapper	for	Trick::RealtimeSync::start_realtime
	*/
extern	"C"	int	real_time_restart(long	long	ref_time	)	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->start_realtime(exec_get_software_frame()	,	ref_time)	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::RealtimeSync
			@userdesc	Return	whether	real-time	synchronization	is	currently	active.
			@par	Python	Usage:
			@code	<my_int>	=	trick.is_real_time()	@endcode
			@return	Trick::RealtimeSync::active	(integer	converted	from	boolean)
	*/
extern	"C"	int	is_real_time()	{
				if	(	the_rts	!=	NULL	)	{
								return((int)(the_rts->is_active()))	;
				}
				return(0)	;
}

int	real_time_change_clock(Trick::Clock	*	in_clock	)	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->change_clock(in_clock)	;
				}
				return	-1	;
}

const	char	*	real_time_clock_get_name()	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->clock_get_name()	;
				}
				return	NULL	;
}

int	real_time_change_timer(Trick::Timer	*	in_sleep_timer	)	{
				if	(	the_rts	!=	NULL	)	{
								return	the_rts->change_timer(in_sleep_timer)	;
				}
				return	-1	;
}

extern	"C"	int	real_time_set_rt_clock_ratio(double	in_clock_ratio)	{
				return	the_rts->set_rt_clock_ratio(in_clock_ratio)	;
}

//	The	lock	memory	functions	are	most	closely	related	to	real-time	but	are
//	not	required	for	syncing.		Therefore	keep	the	routines	as	stand
//	alone	C	functions.

#if	__linux
#include	<sys/mman.h>
#include	<errno.h>
#endif

extern	"C"	int	real_time_lock_memory(int	yes_no)	{
				/*	lock	or	unlock	memory	based	on	yes_no	parameter	*/
				int	ret	=	0	;
#if	__linux
				if	(	yes_no	)	{
								if	((ret	=	mlockall(MCL_CURRENT	|	MCL_FUTURE))	!=	0	)	{
												perror("Error	locking	memory.");
												message_publish(MSG_ERROR,	"Error	%d	when	requesting	memory	lock.\n",	errno);
								}	else	{
												message_publish(MSG_INFO,	"Sim	locked	memory\n");
								}
				}	else	{
								if	(	(ret	=	munlockall())	!=	0	)	{
												perror("Error	unlocking	memory.");
												message_publish(MSG_ERROR,	"Error	%d	when	requesting	memory	unlock.\n",	errno);
								}	else	{
												message_publish(MSG_INFO,	"Sim	unlocked	memory\n");
								}

				}
#endif
#if	__APPLE__
				(void)yes_no	;
				message_publish(MSG_WARNING,	"Warning:	Trick	on	Darwin	does	not	yet	support	memory	locking.\n");
#endif
				return	ret	;
}


extern	"C"	int	exec_set_lock_memory(int	yes_no)	{
				message_publish(MSG_WARNING,	"Warning:	exec_set_lock_memory	deprecated.		Use	real_time_lock_memory	(auto-called)\n");
				return	real_time_lock_memory(yes_no)	;
}

