/*
PURPOSE:						(Allows	clients	to	get	and	set	Trick	parameters)
PROGRAMMERS:	(((Alex	Lin)	(NASA)	(8/06)	(--)))
*/

#include	<iostream>
#include	<sstream>
#include	<pthread.h>
#include	<iomanip>	//	for	setprecision
#include	"trick/VariableServerSession.hh"
#include	"trick/parameter_types.h"
#include	"trick/bitfield_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/tc_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

#define	MAX_MSG_LEN				8192



int	Trick::VariableServerSession::write_binary_data(const	std::vector<VariableReference	*>&	given_vars,	VS_MESSAGE_TYPE	message_type)	{
				typedef	std::vector<VariableReference	*>	VarList;
				typedef	std::pair<int,VarList>	MessageData;

				//	Some	constants	to	make	size	calculations	more	readable
				static	const	int	header_size	=	12;
				static	const	int	sizeof_size	=	4;
				static	const	int	type_size	=	4;

				std::vector<MessageData>	message_sizes_and_vars;

				//	Calculate	how	many	messages	and	what	vars	in	each
				int	total_size	=	header_size;
				VarList	curr_message_vars;
				for	(int	i	=	0;	i	<	given_vars.size();	i++)	{
								VariableReference	*	var	=	given_vars[i];

								int	total_var_size	=	0;
								if	(!_binary_data_nonames)	{
												total_var_size	+=	sizeof_size;
												total_var_size	+=	var->getName().size();
								}

								total_var_size	+=	type_size;
								total_var_size	+=	sizeof_size;
								total_var_size	+=	var->getSizeBinary();

								//	Check	if	this	variable	will	fit	in	a	message	at	all
								if	(header_size	+	total_var_size	>	MAX_MSG_LEN)	{
												message_publish(MSG_WARNING,	"tag=<%s>	Variable	Server	buffer[%d]	too	small	(need	%d)	for	symbol	%s,	SKIPPING	IT.\n",	
																																_connection->getClientTag().c_str(),	MAX_MSG_LEN,	header_size	+	total_var_size,	var->getName().c_str());
												
												continue;
								}

								//	If	this	variable	won't	fit	in	the	current	message,	truncate	the	message	and	plan	to	put	this	var	in	a	new	one
								if	(total_size	+	total_var_size	>	MAX_MSG_LEN)	{
												message_sizes_and_vars.emplace_back(MessageData(total_size,	curr_message_vars));
												
												if	(_debug	>=	2)	{
																message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	buffer[%d]	too	small	(need	%d),	sending	multiple	binary	packets.\n",
																																_connection,	_connection->getClientTag().c_str(),	MAX_MSG_LEN,	total_size	+	total_var_size);
												}

												total_size	=	header_size;
												curr_message_vars.clear();
								}
								total_size	+=	total_var_size;
								curr_message_vars.push_back(var);
				}

				message_sizes_and_vars.emplace_back(MessageData(total_size,	curr_message_vars));
				
				//	Now	write	out	all	of	these	messages
				int	var_index	=	0;
				for	(const	auto&	message_info	:	message_sizes_and_vars)	{
								int	curr_message_size	=	message_info.first;
								VarList	curr_message_vars	=	message_info.second;

								std::stringstream	stream;

								int	written_message_type	=	message_type;
								int	written_header_size	=	curr_message_size	-	4;
								int	written_num_vars	=	curr_message_vars.size();

								if	(_byteswap)	{
												written_message_type	=	trick_byteswap_int(written_message_type);
												written_header_size	=	trick_byteswap_int(written_header_size);
												written_num_vars	=	trick_byteswap_int(written_num_vars);
								}

								//	Header	format:
								//	<message_indicator><message_size><num_vars>

								//	Write	the	header	first
								stream.write((char	*)(&written_message_type),	sizeof(int));	
								stream.write((char	*)(&written_header_size),	sizeof(int));	
								stream.write((char	*)(&written_num_vars),	sizeof(int));

								//	Write	variables	next
								for	(VariableReference	*	var	:	curr_message_vars)	{
												//	Each	variable	is	formatted	as:
												//	<namelength><name><type><size><value
												//	namelength	and	name	are	omitted	if	_binary_data_nonames	is	on

												if	(!_binary_data_nonames)	{
																var->writeNameLengthBinary(stream,	_byteswap);
																var->writeNameBinary(stream,	_byteswap);
												}
												var->writeTypeBinary(stream,	_byteswap);
												var->writeSizeBinary(stream,	_byteswap);
												var->writeValueBinary(stream,	_byteswap);
								}

								if	(_debug	>=	2)	{
												message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	sending	%u	binary	bytes	containing	%d	variables.\n",
																												_connection,	_connection->getClientTag().c_str(),	curr_message_size,	curr_message_vars.size());
								}

								//	Send	it	out!
								char	write_buf[MAX_MSG_LEN];
								stream.read(write_buf,	curr_message_size);
								_connection->write(write_buf,	curr_message_size);
				}

				return	0;
}

int	Trick::VariableServerSession::write_ascii_data(const	std::vector<VariableReference	*>&	given_vars,	VS_MESSAGE_TYPE	message_type	)	{
				//	Load	message	type	first
				std::stringstream	message_stream;
				message_stream	<<	(int)message_type;

				int	message_size	=	message_stream.str().size();

				//	Load	each	variable
				for	(int	i	=	0;	i	<	given_vars.size();	i++)	{
								message_stream	<<	"\t";

								std::stringstream	var_stream;
								given_vars[i]->writeValueAscii(var_stream);

								//	Unfortunately,	there	isn't	a	good	way	to	get	the	size	of	the	buffer	without	putting	it	into	a	string
								std::string	var_string	=	var_stream.str();
								int	var_size	=	var_string.size();

								//	Check	if	this	single	variable	is	too	big,	truncate	if	so
								if	(var_size	+	2	>	MAX_MSG_LEN)	{
												message_publish(MSG_WARNING,	"tag=<%s>	Variable	Server	buffer[%d]	too	small	for	symbol	%s,	TRUNCATED	IT.\n",
																												_connection->getClientTag().c_str(),	MAX_MSG_LEN,	given_vars[i]->getName().c_str());
												
												var_string	=	var_string.substr(0,	MAX_MSG_LEN-2);
												var_size	=	var_string.size();
								}

								//	Check	that	there's	enough	room	for	the	next	variable,	tab	character,	and	possible	newline
								if	(message_size	+	var_size	+	2	>	MAX_MSG_LEN)	{
				
												//	Write	out	an	incomplete	message
												std::string	message	=	message_stream.str();

												if	(_debug	>=	2)	{
																message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	buffer[%d]	too	small	(need	%d),	sending	multiple	ascii	packets.\n",
																																_connection,	_connection->getClientTag().c_str(),	MAX_MSG_LEN,	message_size	+	var_size	+	2);

																message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	sending	%d	ascii	bytes:\n%s\n",
																																_connection,	_connection->getClientTag().c_str(),	message_size,	message.c_str());
												}

												int	result	=	_connection->write(message);
												if	(result	<	0)	{
																return	result;
												}

												//	Clear	out	the	message	stream
												message_stream.str("");
												message_size	=	0;
								}

								//	Concatenate
								message_stream	<<	var_string;
								message_size	+=	var_size	+	1;
				}


				//	End	with	newline
				message_stream	<<	'\n';
				std::string	message	=	message_stream.str();

				if	(_debug	>=	2)	{
								message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	sending	%d	ascii	bytes:\n%s\n",
																								_connection,	_connection->getClientTag().c_str(),	message.size(),	message.c_str());
				}

				int	result	=	_connection->write(message);
				return	result;
}

int	Trick::VariableServerSession::write_data()	{
				return	write_data(_session_variables,	VS_VAR_LIST);
}

int	Trick::VariableServerSession::write_data(std::vector<VariableReference	*>&	given_vars,	VS_MESSAGE_TYPE	message_type)	{	
				//	do	not	send	anything	when	there	are	no	variables!
				if	(	given_vars.size()	==	0)	{
								return(0);
				}

				int	result	=	0;

				if	(	pthread_mutex_trylock(&_copy_mutex)	==	0	)	{
								//	Check	that	all	of	the	variables	are	staged
								for	(VariableReference	*	variable	:	given_vars	)	{
												if	(!variable->isStaged())	{
																pthread_mutex_unlock(&_copy_mutex)	;
																return	0;
												}
								}

								//	Swap	buffer_in	and	buffer_out	for	each	vars[ii].
								for	(VariableReference	*	variable	:	given_vars	)	{
												variable->prepareForWrite();
								}

								pthread_mutex_unlock(&_copy_mutex)	;

								//	Send	out	in	correct	format
								if	(_binary_data)	{
												result	=	write_binary_data(given_vars,	message_type	);
								}	else	{
												//	ascii	mode
												result	=	write_ascii_data(given_vars,	message_type	);
								}
				}

				return	result;
}
