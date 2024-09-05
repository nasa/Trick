
#include	<iostream>
#include	<string.h>

#include	"trick/VariableServerSession.hh"
#include	"trick/variable_server_sync_types.h"
#include	"trick/realtimesync_proto.h"

//	These	methods	should	be	called	from	approprate	jobs	or	from	the	VST

int	Trick::VariableServerSession::copy_and_write_freeze(long	long	curr_freeze_frame)	{
				int	ret	=	0	;

				if	(!get_enabled())
								return	ret;

				if	(get_copy_mode()	==	VS_COPY_TOP_OF_FRAME)	{
								long	long	temp_frame	=	curr_freeze_frame	%	get_freeze_frame_multiple()	;
								if	(	temp_frame	==	get_freeze_frame_offset()	)	{
												copy_sim_data()	;
												if	(	!get_pause()	and	get_write_mode()	==	VS_WRITE_WHEN_COPIED	and	is_real_time())	{
																ret	=	write_data()	;
																if	(	ret	<	0	)	{
																				set_exit_cmd();
																}
												}
								}
				}

				return	ret	;
}

int	Trick::VariableServerSession::copy_and_write_freeze_scheduled(long	long	curr_tics)	{
				int	ret	=	0	;

				if	(!get_enabled())
								return	ret;

				if	(get_copy_mode()	==	VS_COPY_SCHEDULED)	{
								if	(	get_freeze_next_tics()	<=	curr_tics	)	{
												copy_sim_data()	;
												if	(	!get_pause()	&&	get_write_mode()	==	VS_WRITE_WHEN_COPIED	&&	is_real_time())	{
																ret	=	write_data()	;
																if	(	ret	<	0	)	{
																				set_exit_cmd();
																}
												}
												set_freeze_next_tics(curr_tics	+	get_cycle_tics())	;
								}
				}
				return	ret	;
}

int	Trick::VariableServerSession::copy_and_write_scheduled(long	long	curr_tics)	{
				int	ret	=	0	;

				if	(!get_enabled())
								return	ret;

				if	(get_copy_mode()	==	VS_COPY_SCHEDULED)	{
								if	(	get_next_tics()	<=	curr_tics	)	{
												copy_sim_data()	;
												if	(	!get_pause()	&&	get_write_mode()	==	VS_WRITE_WHEN_COPIED	&&	is_real_time())	{
																ret	=	write_data()	;
																if	(	ret	<	0	)	{
																				set_exit_cmd();
																}
												}
												set_next_tics(curr_tics	+	get_cycle_tics())	;
								}
				}
				return	ret	;
}

int	Trick::VariableServerSession::copy_and_write_top(long	long	curr_frame)	{
				int	ret	=	0	;

				if	(!get_enabled())
								return	ret;

				if	(get_copy_mode()	==	VS_COPY_TOP_OF_FRAME)	{
								long	long	temp_frame	=	curr_frame	%	get_frame_multiple()	;
								if	(	temp_frame	==	get_frame_offset()	)	{
												copy_sim_data()	;
												if	(	!get_pause()	&&	get_write_mode()	==	VS_WRITE_WHEN_COPIED	&&	is_real_time())	{
																ret	=	write_data()	;
																if	(	ret	<	0	)	{
																				set_exit_cmd();
																}
												}
								}
				}
				return	ret	;
}

int	Trick::VariableServerSession::copy_and_write_async()	{
				int	ret	=	0;

				if	(!get_enabled())	
								return	ret;

				if	(get_copy_mode()	==	VS_COPY_ASYNC	)	{
								copy_sim_data()	;
				}

				//	Write	data	out	to	connection	if	async	mode	or	non-realtime,	and	not	paused
				bool	should_write_async	=	(get_write_mode()	==	VS_WRITE_ASYNC)	||	
																																(	get_copy_mode()	==	VS_COPY_ASYNC	&&		get_write_mode()	==	VS_WRITE_WHEN_COPIED)||	
																																(!	is_real_time());

				if	(	!get_pause()	&&	should_write_async)	{
								ret	=	write_data()	;
								if	(	ret	<	0	)	{
												set_exit_cmd();
								}
				}

				return	ret;
}
