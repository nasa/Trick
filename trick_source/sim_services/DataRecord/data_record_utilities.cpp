/*
PURPOSE:
				(Data	recording	access	functions)
PROGRAMMERS:
					(((Robert	W.	Bailey)	(LinCom	Corp)	(3/96)	(SES	upgrades)
					((Alex	Lin)	(NASA)	(April	2009)	(--)	(c++	port)))
*/

#include	"trick/data_record_proto.h"
#include	"trick/DataRecordDispatcher.hh"
#include	"trick/DataRecordGroup.hh"

extern	Trick::DataRecordDispatcher	*	the_drd		;

extern	"C"	int	dr_remove_files(void)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->remove_files()	;
				}
				return	-1	;
}

extern	"C"	int	dr_enable(void)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->enable()	;
				}
				return	-1	;
}

extern	"C"	int	dr_enable_group(	const	char	*	in_name	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->enable(	in_name	)	;
				}
				return	-1	;
}

extern	"C"	int	dr_disable(void)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->disable()	;
				}
				return	-1	;
}

extern	"C"	int	dr_disable_group(	const	char	*	in_name	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->disable(	in_name	)	;
				}
				return	-1	;
}

extern	"C"	int	dr_record_now_group(	const	char	*	in_name	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->record_now_group(	in_name	)	;
				}
				return	-1	;
}

extern	"C"	int	add_data_record_group(	Trick::DataRecordGroup	*	in_group,	Trick::DR_Buffering	buffering	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->add_group(in_group,	buffering)	;
				}
				return	-1	;
}

extern	"C"	int	remove_data_record_group(	Trick::DataRecordGroup	*	in_group	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->remove_group(in_group)	;
				}
				return	-1	;
}

extern	"C"	void	remove_all_data_record_groups(void)	{
				the_drd->remove_all_groups()	;
}

extern	"C"	Trick::DataRecordGroup	*	get_data_record_group(	std::string	in_name	)	{
				if	(	the_drd	!=	NULL	)	{
								return	the_drd->get_group(in_name)	;
				}
				return	NULL	;
}

extern	"C"	int	set_max_size_record_group	(const	char	*	in_name,	uint64_t	bytes	)	{
				if	(	the_drd	!=	NULL	)	{
				return	the_drd->set_group_max_file_size(in_name,	bytes	)	;
				}
				return	-1	;
}

extern	"C"	int	dr_set_max_file_size	(	uint64_t	bytes	)	{
				if	(	the_drd	!=	NULL	)	{
				return	the_drd->set_max_file_size(	bytes	)	;
				}
				return	-1	;
}
