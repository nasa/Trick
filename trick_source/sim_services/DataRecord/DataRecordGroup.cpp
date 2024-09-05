
#include	<algorithm>
#include	<string>
#include	<iostream>
#include	<sstream>
#include	<string.h>
#include	<stdlib.h>
#include	<iomanip>

#ifdef	__GNUC__
#include	<cxxabi.h>
#endif

#include	"trick/DataRecordGroup.hh"
#include	"trick/command_line_protos.h"
#include	"trick/exec_proto.h"
#include	"trick/reference.h"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	The	recording	group	is	enabled
-#	The	recording	group	is	set	to	always	record	data	(not	changes	only)
-#	The	recording	group	is	set	to	allocate	a	maximum	of	100,000	records	in	memory
-#	The	recording	group	is	set	to	record	at	a	cycle	period	of	0.1	seconds.
-#	The	first	recording	parameter	is	assigned	to	simulation	time.		The	name	of
			the	parmeter	is	named	"sys.exec.out.time"	to	preserve	backwards	compatibility
			with	older	%Trick	releases.
-#	The	call_function	and	call_function_double	functions	inherited	from	the	SimObject
			are	populated.		The	data_record	function	is	added	as	a	job	for	this	SimObject.

A	sim	object	is	created	named	@c	data_record_group<n>	where	<n>	is	a	unique	group	number.	A
data_record	class	job	within	this	sim	object	is	also	created	that	will	write	the	group's
recorded	data	to	disk.	The	job's	name	is	<tt>	data_record_group<n>.<in_name></tt>.

*/
Trick::DataRecordBuffer::DataRecordBuffer()	{
				buffer	=	last_value	=	NULL	;
				ref	=	NULL	;
				ref_searched	=	false	;
}

Trick::DataRecordBuffer::~DataRecordBuffer()	{
				if	(	buffer	)	{
								free(buffer)	;
				}
				if	(	last_value	)	{
								free(last_value)	;
				}

				ref_free(ref)	;
				free(ref)	;
}

Trick::DataRecordGroup::DataRecordGroup(	std::string	in_name	)	:
	record(true)	,
	inited(false)	,
	group_name(in_name)	,
	freq(DR_Always),
	start(0.0)	,
	cycle(0.1)	,
	time_value_attr()	,
	num_variable_names(0),
	variable_names(NULL),
	variable_alias(NULL),
	num_change_variable_names(0),
	change_variable_names(NULL),
	change_variable_alias(NULL),
	max_num(100000),
	buffer_num(0),
	writer_num(0),
	max_file_size(1<<30),	//	1	GB
	total_bytes_written(0),
	max_size_warning(false),
	writer_buff(NULL),
	single_prec_only(false),
	buffer_type(DR_Buffer),
	job_class("data_record"),
	curr_time(0.0)
{

				union	{
								long	l;
								char	c[sizeof(long)];
				}	byte_order_union;

				byte_order_union.l	=	1;
				if	(byte_order_union.c[sizeof(long)	-	1]	!=	1)	{
								byte_order	=	"little_endian"	;
				}	else	{
								byte_order	=	"big_endian"	;
				}

				//	sim	object	name
				name	=	std::string("trick_data_record_group_")	+	in_name	;

				//	add_jobs_to_queue	will	fill	in	job_id	later
				//	make	the	init	job	run	after	all	other	initialization	jobs	but	before	the	post	init	checkpoint
				//	job	so	users	can	allocate	memory	in	initialization	jobs	and	checkpointing	data	rec	groups	will	work
				add_job(0,	1,	(char	*)"initialization",	NULL,	cycle,	(char	*)"init",	(char	*)"TRK",	65534)	;
				add_job(0,	2,	(char	*)"end_of_frame",	NULL,	1.0,	(char	*)"write_data",	(char	*)"TRK")	;
				add_job(0,	3,	(char	*)"checkpoint",	NULL,	1.0,	(char	*)"checkpoint",	(char	*)"TRK")	;
				add_job(0,	4,	(char	*)"post_checkpoint",	NULL,	1.0,	(char	*)"clear_checkpoint_vars",	(char	*)"TRK")	;
				//	run	the	restart	job	in	phase	60001
				add_job(0,	5,	(char	*)"restart",	NULL,	1.0,	(char	*)"restart",	(char	*)"TRK",	60001)	;
				add_job(0,	6,	(char	*)"shutdown",	NULL,	1.0,	(char	*)"shutdown",	(char	*)"TRK")	;

				write_job	=	add_job(0,	99,	(char	*)job_class.c_str(),	NULL,	cycle,	(char	*)"data_record"	,	(char	*)"TRK")	;

				add_time_variable()	;
}

Trick::DataRecordGroup::~DataRecordGroup()	{
				free((void	*)time_value_attr.units)	;
}

int	Trick::DataRecordGroup::call_function(	Trick::JobData	*	curr_job	)	{

				int	ret	=	0	;

				switch	(curr_job->id)	{
								case	1:
												ret	=	init()	;
												break	;
								case	2:
												ret	=	write_data(false)	;
												break	;
								case	3:
												ret	=	checkpoint()	;
												break	;
								case	4:
												clear_checkpoint_vars()	;
												break	;
								case	5:
												ret	=	restart()	;
												break	;
								case	6:
												ret	=	shutdown()	;
												break	;
								default:
												ret	=	data_record(exec_get_sim_time())	;
												break	;
				}

				return(ret)	;

}

double	Trick::DataRecordGroup::call_function_double(	Trick::JobData	*	curr_job	)	{
				(void)	curr_job	;
				return(0.0)	;
}

void	Trick::DataRecordGroup::register_group_with_mm(void	*	address	,	const	char	*	type)	{
				//	Only	add	to	the	memory	manager	if	it	has	not	already	been	added
				if	(	TMM_var_exists(name.c_str())	==	0	)	{
								//	Declare	this	to	the	memory	manager.		Must	be	done	here	to	get	the	correct	type	name
								TMM_declare_ext_var(address	,	TRICK_STRUCTURED,	type	,	0	,	name.c_str()	,	0	,	NULL	)	;
								ALLOC_INFO	*	alloc_info	=	get_alloc_info_at(address)	;
								alloc_info->stcl	=	TRICK_LOCAL	;
								alloc_info->alloc_type	=	TRICK_ALLOC_NEW	;
				}
}

const	std::string	&	Trick::DataRecordGroup::get_group_name()	{
				return	group_name	;
}

int	Trick::DataRecordGroup::set_cycle(	double	in_cycle	)	{
				write_job->set_cycle(in_cycle)	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_phase(	unsigned	short	in_phase	)	{
				write_job->phase	=	in_phase	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_freq(	DR_Freq	in_freq	)	{
				freq	=	in_freq	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_max_buffer_size(	int	num	)	{
				max_num	=	num	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_buffer_type(	int	in_buffer_type	)	{
				buffer_type	=	(DR_Buffering)in_buffer_type	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_max_file_size(	uint64_t	bytes	)	{
				if(bytes	==	0)	{
								max_file_size	=	UINT64_MAX	;
				}	else	{
				max_file_size	=	bytes	;	
				}
				return(0)	;
}

int	Trick::DataRecordGroup::set_single_prec_only(	bool	in_single_prec_only	)	{
				single_prec_only	=	in_single_prec_only	;
				return(0)	;
}

int	Trick::DataRecordGroup::set_thread(	unsigned	int	in_thread_id	)	{

				unsigned	int	jj	;
				Trick::JobData	*	temp_job		;

				/*	make	all	data_record_groups	have	same	sim	object	id	as	data_record	*/
				for	(	jj	=	0	;	jj	<	jobs.size()	;	jj++	)	{
								temp_job	=	jobs[jj]	;
								temp_job->thread	=	in_thread_id	;
				}
				return	0	;
}

int	Trick::DataRecordGroup::set_job_class(	std::string	in_class	)	{
				write_job->job_class_name	=	job_class	=	in_class	;
				return(0)	;
}

int	Trick::DataRecordGroup::add_time_variable()	{
				REF2	*	new_ref	;

				//	Create	attributes	for	time	recorded	as	a	double
				time_value_attr.type	=	TRICK_DOUBLE	;
				time_value_attr.size	=	sizeof(double)	;
				time_value_attr.units	=	strdup("s")	;

				//	Create	a	reference	that	records	time	as	sys.exec.out.time
				new_ref	=	(REF2	*)calloc(	1	,	sizeof(REF2));
				new_ref->reference	=	strdup("sys.exec.out.time")	;
				new_ref->address	=	&curr_time	;
				new_ref->attr	=	&time_value_attr	;
				add_variable(new_ref)	;

				return	0	;
}

int	Trick::DataRecordGroup::add_variable(	std::string	in_name	,	std::string	alias	)	{

				Trick::DataRecordBuffer	*	new_var	=	new	Trick::DataRecordBuffer	;
				//	Trim	leading	spaces
				in_name.erase(	0,	in_name.find_first_not_of(	"	\t"	)	);
				//	Trim	trailing	spaces
				in_name.erase(	in_name.find_last_not_of(	"	\t"	)	+	1);
				new_var->name	=	in_name	;
				new_var->alias	=	alias	;
				rec_buffer.push_back(new_var)	;
				return	0	;
}

void	Trick::DataRecordGroup::remove_variable(	std::string	in_name	)	{
				//	Trim	leading	spaces++	
				in_name.erase(	0,	in_name.find_first_not_of(	"	\t"	)	);
				//	Trim	trailing	spaces
				in_name.erase(	in_name.find_last_not_of(	"	\t"	)	+	1);

				if	(!in_name.compare("sys.exec.out.time"))	{
								//	This	class	assumes	sim	time	is	always	the	first	variable.
								//	Removing	it	results	in	errors.
								return;
				}

				auto	remove_from	=	[&](std::vector<DataRecordBuffer*>&	buffer)	{
								for	(auto	i	=	buffer.begin();	i	!=	buffer.end();	++i)	{
												if	(!(*i)->name.compare(in_name))	{
																delete	*i;
																buffer.erase(i);
																break;
												}
								}
				};

				remove_from(rec_buffer);
				remove_from(change_buffer);
}

void	Trick::DataRecordGroup::remove_all_variables()	{
				//	remove	all	but	the	first	variable,	which	is	sim	time
				if(!rec_buffer.empty())	{
								for	(auto	i	=	rec_buffer.begin()	+	1;	i	!=	rec_buffer.end();	++i)	{
												delete	*i;
								}
								rec_buffer.erase(rec_buffer.begin()	+	1,	rec_buffer.end());
				}

				//	remove	everything
				for	(auto	variable	:	change_buffer)	{
								delete	variable;
				}

				change_buffer.clear();
}

int	Trick::DataRecordGroup::add_variable(	REF2	*	ref2	)	{

				if	(	ref2	==	NULL	||	ref2->attr	==	NULL	)	{
								return(-1)	;
				}

				Trick::DataRecordBuffer	*	new_var	=	new	Trick::DataRecordBuffer	;
				new_var->name	=	std::string(ref2->reference)	;
				new_var->ref_searched	=	true	;
				new_var->ref	=	ref2	;
				new_var->last_value	=	(char	*)calloc(1	,	new_var->ref->attr->size)	;
				//	Don't	allocate	space	for	the	temp	storage	buffer	until	"init"
				rec_buffer.push_back(new_var)	;

				return(0)	;

}

int	Trick::DataRecordGroup::add_change_variable(	std::string	in_name	)	{

				REF2	*	ref2	;

				ref2	=	ref_attributes(in_name.c_str())	;

				if	(	ref2	==	NULL	||	ref2->attr	==	NULL	)	{
								message_publish(MSG_WARNING,	"Could	not	find	Data	Record	change	variable	%s.\n",	in_name.c_str())	;
								return(-1)	;
				}

				Trick::DataRecordBuffer	*	new_var	=	new	Trick::DataRecordBuffer	;
				new_var->ref	=	ref2	;
				new_var->name	=	in_name;
				new_var->buffer	=	(char	*)malloc(ref2->attr->size)	;
				new_var->last_value	=		NULL	;
				memcpy(new_var->buffer	,	ref2->address	,	ref2->attr->size)	;
				change_buffer.push_back(new_var)	;

				return(0)	;

}

bool	Trick::DataRecordGroup::isSupportedType(REF2	*	ref2,	std::string&	message)	{
				if	(ref2->attr->type	==	TRICK_STRING	||	ref2->attr->type	==	TRICK_STL	||	ref2->attr->type	==	TRICK_STRUCTURED)	{
								message	=	"Cannot	Data	Record	variable	"	+	std::string(ref2->reference)	+	"	of	unsupported	type	"	+	std::to_string(ref2->attr->type);
								return	false;
				}
				
				//	If	this	is	an	array	and	not	a	single	value,	don't	record	it
				if	(ref2->num_index	!=	ref2->attr->num_index)	{
								message	=	"Cannot	Data	Record	arrayed	variable	"	+	std::string(ref2->reference);
								return	false;
				}

				return	true;
}

/**
@details
-#	The	simulation	output	directory	is	retrieved	from	the	CommandLineArguments
-#	The	log	header	file	is	created
			-#	The	endianness	of	the	log	file	is	written	to	the	log	header.
			-#	The	names	of	the	parameters	contained	in	the	log	file	are	written	to	the	header.
-#	Memory	buffers	are	allocated	to	store	simulation	data
-#	The	DataRecordGroupObject	(a	derived	SimObject)	is	added	to	the	Scheduler.
*/
int	Trick::DataRecordGroup::init()	{

				unsigned	int	jj	;
				int	ret	;

				//	reset	counter	here	so	we	can	"re-init"	our	recording
				buffer_num	=	writer_num	=	total_bytes_written	=	0	;

				output_dir	=	command_line_args_get_output_dir()	;
				/*	this	is	the	common	part	of	the	record	file	name,	the	format	specific	will	add	the	correct	suffix	*/
				file_name	=	output_dir	+	"/log_"	+	group_name	;

				pthread_mutex_init(&buffer_mutex,	NULL);

				//	Allocate	recording	space	for	time.
				rec_buffer[0]->buffer	=	(char	*)calloc(max_num	,	rec_buffer[0]->ref->attr->size)	;
				rec_buffer[0]->last_value	=	(char	*)calloc(1	,	rec_buffer[0]->ref->attr->size)	;

				/*	Loop	through	all	variables	looking	up	names.		Allocate	recording	space
							according	to	size	of	the	variable	*/
				for	(jj	=	1;	jj	<	rec_buffer.size()	;	jj++)	{
								Trick::DataRecordBuffer	*	drb	=	rec_buffer[jj]	;
								if	(	drb->ref_searched	==	false	)	{
												REF2	*	ref2	;

												ref2	=	ref_attributes(drb->name.c_str())	;
												if	(	ref2	==	NULL	||	ref2->attr	==	NULL	)	{
																message_publish(MSG_WARNING,	"Could	not	find	Data	Record	variable	%s.\n",	drb->name.c_str())	;
																rec_buffer.erase(rec_buffer.begin()	+	jj--)	;
																delete	drb	;
																continue	;
												}	else	{
																std::string	message;
																if	(!isSupportedType(ref2,	message))	{
																				message_publish(MSG_WARNING,	"%s\n",	message.c_str())	;
																				rec_buffer.erase(rec_buffer.begin()	+	jj--)	;
																				delete	drb	;
																				continue	;
																}	else	{
																				drb->ref	=	ref2	;
																}
												}
								}
								if	(	drb->alias.compare("")	)	{
												drb->ref->reference	=	strdup(drb->alias.c_str())	;
								}
								drb->last_value	=	(char	*)calloc(1	,	drb->ref->attr->size)	;
								drb->buffer	=	(char	*)calloc(max_num	,	drb->ref->attr->size)	;
								drb->ref_searched	=	true	;
				}

				write_header()	;

				//	call	format	specific	initialization	to	open	destination	and	write	header
				ret	=	format_specific_init()	;

				//	set	the	inited	flag	to	true	when	all	initialization	is	done
				if	(	ret	==	0	)	{
								inited	=	true	;
				}

				return(0)	;

}

int	Trick::DataRecordGroup::checkpoint()	{
				unsigned	int	jj	;

				/*
							Save	the	names	of	the	variables	and	the	aliases	to	the	checkpoint,
							the	rest	of	the	DataRecordBuffer	will	be	reconstructed	during	restart.
							The	first	variable	is	time	which	we	do	not	have	to	save.
					*/
				if	(	rec_buffer.size()	>	1	)	{
								num_variable_names	=	rec_buffer.size()	-	1	;
								variable_names	=	(char	**)TMM_declare_var_1d("char	*",	(int)rec_buffer.size()	-	1)	;
								variable_alias	=	(char	**)TMM_declare_var_1d("char	*",	(int)rec_buffer.size()	-	1)	;

								for	(jj	=	1;	jj	<	rec_buffer.size()	;	jj++)	{
												Trick::DataRecordBuffer	*	drb	=	rec_buffer[jj]	;

												variable_names[jj-1]	=	TMM_strdup((char	*)drb->name.c_str())	;
												variable_alias[jj-1]	=	TMM_strdup((char	*)drb->alias.c_str())	;
								}
				}

				/*
							Save	the	names	of	the	change	variables	and	the	aliases	to	the	checkpoint,
							the	rest	of	the	DataRecordBuffer	will	be	reconstructed	during	restart
					*/
				if	(	change_buffer.size()	>	0	)	{
								num_change_variable_names	=	change_buffer.size()	;
								change_variable_names	=	(char	**)TMM_declare_var_1d("char	*",	(int)change_buffer.size())	;
								change_variable_alias	=	(char	**)TMM_declare_var_1d("char	*",	(int)change_buffer.size())	;

								for	(jj	=	0;	jj	<	change_buffer.size()	;	jj++)	{
												Trick::DataRecordBuffer	*	drb	=	change_buffer[jj]	;

												change_variable_names[jj]	=	TMM_strdup((char	*)drb->name.c_str())	;
												change_variable_alias[jj]	=	TMM_strdup((char	*)drb->alias.c_str())	;
								}
				}

				return	0	;
}

void	Trick::DataRecordGroup::clear_checkpoint_vars()	{
				
				if	(	variable_names	)	{
								for(unsigned	int	jj	=	0;	jj	<	num_variable_names;	jj++)	{
												TMM_delete_var_a(variable_names[jj]);
								}
								TMM_delete_var_a(variable_names)	;
				}

				if	(	variable_alias	)	{
								for(unsigned	int	jj	=	0;	jj	<	num_variable_names;	jj++)	{
												TMM_delete_var_a(variable_alias[jj]);
								}
								TMM_delete_var_a(variable_alias)	;
				}

				if	(	change_variable_names	)	{
								for(unsigned	int	jj	=	0;	jj	<	num_change_variable_names;	jj++)	{
												TMM_delete_var_a(change_variable_names[jj]);
								}
								TMM_delete_var_a(change_variable_names)	;
				}

				if	(	change_variable_alias	)	{
								for(unsigned	int	jj	=	0;	jj	<	num_change_variable_names;	jj++)	{
												TMM_delete_var_a(change_variable_alias[jj]);
								}
								TMM_delete_var_a(change_variable_alias)	;
				}

				variable_names	=	NULL	;
				variable_alias	=	NULL	;
				change_variable_names	=	NULL	;
				change_variable_alias	=	NULL	;
				num_variable_names	=	0	;
				num_change_variable_names	=	0	;
}

int	Trick::DataRecordGroup::restart()	{
				std::vector	<Trick::DataRecordBuffer	*>::iterator	drb_it	;

				/*	delete	the	current	rec_buffer	*/
				for	(	drb_it	=	rec_buffer.begin()	;	drb_it	!=	rec_buffer.end()	;	++drb_it	)	{
								delete	*drb_it	;
				}
				rec_buffer.clear()	;
				/*	Add	back	the	time	variable	*/
				add_time_variable()	;

				/*	delete	the	current	change_buffer	contents	*/
				for	(	drb_it	=	change_buffer.begin()	;	drb_it	!=	change_buffer.end()	;	++drb_it	)	{
								delete	*drb_it	;
				}
				change_buffer.clear()	;

				unsigned	int	jj	;
				/*	add	the	variable	names	listed	in	the	checkpoint	file	*/
				for	(	jj	=	0	;	jj	<	num_variable_names	;	jj++	)	{
								add_variable(	variable_names[jj]	,	variable_alias[jj]	)	;
				}
				for	(	jj	=	0	;	jj	<	num_change_variable_names	;	jj++	)	{
								add_change_variable(	change_variable_names[jj]	)	;
				}

				clear_checkpoint_vars()	;

				//	set	the	write	job	class	to	what	is	in	the	checkpoint	file.
				write_job->job_class_name	=	job_class	;

				//	reset	the	sim_object	name.
				name	=	std::string("data_record_group_")	+	group_name	;

				/*	call	init	to	open	the	recording	file	and	look	up	variable	name	addresses	*/
				init()	;

				return	0	;
}

int	Trick::DataRecordGroup::write_header()	{

				unsigned	int	jj	;
				std::string	header_name	;
				std::fstream	out_stream	;

				/*!	create	the	header	file	used	by	the	GUIs	*/
				header_name	=	output_dir	+	"/log_"	+	group_name	+	".header"	;

				out_stream.open(header_name.c_str(),	std::fstream::out	)	;
				if	(	!	out_stream		||		!	out_stream.good()	)	{
								return	-1;
				}

				/*	Header	file	first	line	is	created	in	format	specific	header	*/
				out_stream	<<	"log_"	<<	group_name	;

				format_specific_header(out_stream)	;

				/*	Output	the	file	name,	variable	size,	units,	and	variable	name
					*	to	the	rest	of	recorded	data	header	file.
					*	(e.g.	file_name		C_type		units		sim_name)
					*	Note:	"sys.exec.out.time"	should	be	the	first	variable	in	the	buffer.
					*/
				for	(jj	=	0;	jj	<	rec_buffer.size()	;	jj++)	{
								/*!	recording	single	data	item	*/
								out_stream	<<	"log_"	<<	group_name	<<	"\t"
												<<	type_string(rec_buffer[jj]->ref->attr->type,
																											rec_buffer[jj]->ref->attr->size)	<<	"\t"
												<<	std::setw(6)	;

								if	(	rec_buffer[jj]->ref->attr->mods	&	TRICK_MODS_UNITSDASHDASH	)	{
												out_stream	<<	"--"	;
								}	else	{
												out_stream	<<	rec_buffer[jj]->ref->attr->units	;
								}
								out_stream	<<	"\t"	<<	rec_buffer[jj]->ref->reference	<<	std::endl	;
				}

				//	Send	all	unwritten	characters	in	the	buffer	to	its	output/file.
				out_stream.flush()	;
				out_stream.close()	;

				return(0)	;

}

int	Trick::DataRecordGroup::data_record(double	in_time)	{

				unsigned	int	jj	;
				unsigned	int	buffer_offset	;
				Trick::DataRecordBuffer	*	drb	;
				bool	change_detected	=	false	;

				//TODO:	does	not	handle	bitfields	correctly!
				if	(	record	==	true	)	{
								if	(	freq	!=	DR_Always	)	{
												for	(jj	=	0;	jj	<	change_buffer.size()	;	jj++)	{
																drb	=	change_buffer[jj]	;
																REF2	*	ref	=	drb->ref	;
																if	(	ref->pointer_present	==	1	)	{
																				ref->address	=	follow_address_path(ref)	;
																}
																if	(	memcmp(	drb->buffer	,	drb->ref->address	,	drb->ref->attr->size)	)	{
																				change_detected	=	true	;
																				memcpy(	drb->buffer	,	drb->ref->address	,	drb->ref->attr->size)	;
																}
												}

								}

								if	(	freq	==	DR_Always	||	change_detected	==	true	)	{

												//	If	this	is	not	the	ring	buffer	and
												//	we	are	going	to	have	trouble	fitting	2	data	sets	then	write	the	data	now.
												if	(	buffer_type	!=	DR_Ring_Buffer	)	{
																if	(	buffer_num	-	writer_num	>=	(max_num	-	2)	)	{
																				write_data(true)	;
																}
												}

												curr_time	=	in_time	;

												if	(	freq	==	DR_Changes_Step	)	{
																buffer_offset	=	buffer_num	%	max_num	;
																*((double	*)(rec_buffer[0]->last_value))	=	in_time	;
																for	(jj	=	0;	jj	<	rec_buffer.size()	;	jj++)	{
																				drb	=	rec_buffer[jj]	;
																				REF2	*	ref	=	drb->ref	;
																				int	param_size	=	ref->attr->size	;
																				if	(	buffer_offset	==	0	)	{
																							drb->curr_buffer	=	drb->buffer	;
																				}	else	{
																							drb->curr_buffer	+=	param_size	;
																				}
																				switch	(	param_size	)	{
																								case	8:
																												*(int64_t	*)drb->curr_buffer	=	*(int64_t	*)drb->last_value	;
																												break	;
																								case	4:
																												*(int32_t	*)drb->curr_buffer	=	*(int32_t	*)drb->last_value	;
																												break	;
																								case	2:
																												*(int16_t	*)drb->curr_buffer	=	*(int16_t	*)drb->last_value	;
																												break	;
																								case	1:
																												*(int8_t	*)drb->curr_buffer	=	*(int8_t	*)drb->last_value	;
																												break	;
																								default:
																												memcpy(	drb->curr_buffer	,	drb->last_value	,	param_size	)	;
																												break	;
																				}
																}
																buffer_num++	;
												}

												buffer_offset	=	buffer_num	%	max_num	;
												for	(jj	=	0;	jj	<	rec_buffer.size()	;	jj++)	{
																drb	=	rec_buffer[jj]	;
																REF2	*	ref	=	drb->ref	;
																if	(	ref->pointer_present	==	1	)	{
																				ref->address	=	follow_address_path(ref)	;
																}
																int	param_size	=	ref->attr->size	;
																if	(	buffer_offset	==	0	)	{
																			drb->curr_buffer	=	drb->buffer	;
																}	else	{
																			drb->curr_buffer	+=	param_size	;
																}
																switch	(	param_size	)	{
																				case	8:
																								*(int64_t	*)drb->curr_buffer	=	*(int64_t	*)ref->address	;
																								break	;
																				case	4:
																								*(int32_t	*)drb->curr_buffer	=	*(int32_t	*)ref->address	;
																								break	;
																				case	2:
																								*(int16_t	*)drb->curr_buffer	=	*(int16_t	*)ref->address	;
																								break	;
																				case	1:
																								*(int8_t	*)drb->curr_buffer	=	*(int8_t	*)ref->address	;
																								break	;
																				default:
																								memcpy(	drb->curr_buffer	,	ref->address	,	param_size	)	;
																								break	;
																}
												}
												buffer_num++	;
								}
				}

				return(0)	;

}

int	Trick::DataRecordGroup::write_data(bool	must_write)	{

				unsigned	int	local_buffer_num	;
				unsigned	int	num_to_write	;
				unsigned	int	writer_offset	;

				if	(	record	and	inited	and	(buffer_type	==	DR_No_Buffer	or	must_write)	and	(total_bytes_written	<=	max_file_size))	{

								//	buffer_mutex	is	used	in	this	one	place	to	prevent	forced	calls	of	write_data
								//	to	not	overwrite	data	being	written	by	the	asynchronous	thread.
								pthread_mutex_lock(&buffer_mutex)	;

								local_buffer_num	=	buffer_num	;
								if	(	(local_buffer_num	-	writer_num)	>	max_num	)	{
												num_to_write	=	max_num	;
								}	else	{
												num_to_write	=	(local_buffer_num	-	writer_num)	;
								}
								writer_num	=	local_buffer_num	-	num_to_write	;

								//!	This	loop	pulls	a	"row"	of	time	homogeneous	data	and	writes	it	to	the	file
								while	(	writer_num	!=	local_buffer_num	)	{

												writer_offset	=	writer_num	%	max_num	;
												//!	keep	record	of	bytes	written	to	file.	Default	max	is	1GB
												total_bytes_written	+=	format_specific_write_data(writer_offset)	;
												writer_num++	;

								}

								if(!max_size_warning	&&	(total_bytes_written	>	max_file_size))	{
												std::cerr	<<	"WARNING:	Data	record	max	file	size	"	<<	(static_cast<double>(max_file_size))/(1<<20)	<<	"MB	reached.\n"
												"https://nasa.github.io/trick/documentation/simulation_capabilities/Data-Record#changing-the-max-file-size-of-a-data-record-group-ascii-and-binary-only"	
												<<	std::endl;
												max_size_warning	=	true;
								}

								pthread_mutex_unlock(&buffer_mutex)	;

				}

				return	0	;
}

int	Trick::DataRecordGroup::enable()	{
				record	=	true	;
				return(0)	;
}

int	Trick::DataRecordGroup::disable()	{
				record	=	false	;
				return(0)	;
}

int	Trick::DataRecordGroup::shutdown()	{

				//	Force	write	out	all	data
				record	=	true	;	//	If	user	disabled	group,	make	sure	any	recorded	data	gets	written	out
				write_data(true)	;
				format_specific_shutdown()	;

				remove_all_variables();

				//	remove_all_variables	does	not	remove	sim	time
				if(!rec_buffer.empty()){
								delete	rec_buffer[0];
								rec_buffer.clear();
				}

				if	(	writer_buff	)	{
								free(writer_buff)	;
								writer_buff	=	NULL	;
				}

				return	0	;
}

std::string	Trick::DataRecordGroup::type_string(	int	item_type,	int	item_size	)	{
				switch	(item_type)	{
								case	TRICK_CHARACTER:
																return	"char";
																break;
								case	TRICK_UNSIGNED_CHARACTER:
																return	"unsigned_char";
																break;
								case	TRICK_STRING:
																return	"string";
																break;
								case	TRICK_SHORT:
																return	"short";
																break;
								case	TRICK_UNSIGNED_SHORT:
																return	"unsigned_short";
																break;
								case	TRICK_ENUMERATED:
								case	TRICK_INTEGER:
																return	"int";
																break;
								case	TRICK_UNSIGNED_INTEGER:
																return	"unsigned_int";
																break;
								case	TRICK_LONG:
																return	"long";
																break;
								case	TRICK_UNSIGNED_LONG:
																return	"unsigned_long";
																break;
								case	TRICK_FLOAT:
																return	"float";
																break;
								case	TRICK_DOUBLE:
																if	(	single_prec_only	)	{
																								return	"float";
																}
																else	{
																								return	"double";
																}
																break;
								case	TRICK_BITFIELD:
																if	(item_size	==	sizeof(int))	{
																								return	"int";
																}	else	if	(item_size	==	sizeof(short))	{
																								return	"short";
																}	else	{
																								return	"char";
																}
																break;
								case	TRICK_UNSIGNED_BITFIELD:
																if	(item_size	==	sizeof(int))	{
																								return	"unsigned_int";
																}	else	if	(item_size	==	sizeof(short))	{
																								return	"unsigned_short";
																}	else	{
																								return	"unsigned_char";
																}
																break;
								case	TRICK_LONG_LONG:
																return	"long_long";
																break;
								case	TRICK_UNSIGNED_LONG_LONG:
																return	"unsigned_long_long";
																break;
								case	TRICK_BOOLEAN:
#if	(	__sun	|	__APPLE__	)
																return	"int";
#else
																return	"unsigned_char";
#endif
																break;
				}
				return	"";
}

