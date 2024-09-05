#include	<sstream>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<limits>

#include	"trick/MonteVarFile.hh"
#include	"trick/message_proto.h"
#include	"trick/exec_proto.h"

Trick::MonteVarFile::MonteVarFile(std::string	in_name,	std::string	in_file_name,	unsigned	int	in_column,	std::string	in_unit)	:	input_file_stream(NULL)	{
				name	=	in_name;
				column	=	in_column;
				unit	=	in_unit;

				set_file_name(in_file_name);
}

Trick::MonteVarFile::~MonteVarFile()	{
				delete	input_file_stream;
}

//	Composite	the	various	properties	of	this	MonteVarFile.
std::string	Trick::MonteVarFile::describe_variable()	{
				std::stringstream	ss;

				ss	<<	"#NAME:\t\t"	<<	name	<<	"\n"
							<<	"#TYPE:\t\tFILE\n"
							<<	"#UNIT:\t\t"	<<	unit	<<	"\n"
							<<	"#FILE:\t\t"	<<	file_name	<<	"\n"
							<<	"#COLUMN:\t"	<<	column	<<	"\n";

				return	ss.str();
}

std::string	Trick::MonteVarFile::get_next_value()	{
				//	Open	the	file	and	seek	to	the	previous	position.
				input_file_stream->open(file_name.c_str(),	std::ifstream::in);
				input_file_stream->seekg(stream_position);

				if	(input_file_stream->good())	{
								std::string	line;
								//	Skip	the	comments	and	empty	lines	in	the	data	file.
								do	{
												std::getline(*input_file_stream,	line);

												if(input_file_stream->eof())	{
																if	(line.empty())	{
																				input_file_stream->close();
																				return	"EOF";
																}	else	{
																				input_file_stream->seekg(0,	input_file_stream->end);
																}
												}
								}
								while(line[0]	==	'#'	||	line[0]	==	'\0');

								//	Store	the	current	stream	position	and	close	the	file.
								stream_position	=	input_file_stream->tellg();
								input_file_stream->close();

								//	Count	the	number	of	columns	in	the	input	file.
								char	*token;
								unsigned	int	ntokens	=	0;
								char*	temp_str	=	strdup(line.c_str());
								token	=	strtok(temp_str,	"	\t");
								while	(token	!=	NULL)	{
												token	=	strtok(NULL,	"	\t");
												ntokens++;
								}

								//	Verify	the	input	column	number	is	valid.
								if	((column	==	0)	||	(column	>	ntokens))	{
												char	string[100];
												snprintf(string,	sizeof(string),	"Trick:MonteVarFile	An	invalid	column	number	%u,	valid	column	numbers	are	1	-	%u",	column,	ntokens);
												exec_terminate_with_return(-1,	__FILE__,	__LINE__,	string);
								}

								//	Get	the	next	value.
								if(temp_str)	{
												free(temp_str);
								}
								temp_str	=	strdup(line.c_str());
								token	=	strtok(temp_str,	"	\t");

								for(unsigned	int	i	=	1;	i	<	column;	i++)	{
												//	Iterate	through	each	token	in	the	temp_str.
												if(token	!=	NULL)
																token	=	strtok(NULL,	"	\t");
								}

								//	Return	the	value	as	a	string.
								value	=	token;
								std::stringstream	ss;

								if(unit.empty())
												ss	<<	name	<<	"	=	"	<<	token;
								else
												ss	<<	name	<<	"	=	"	<<	"trick.attach_units(\""	<<	unit	<<	"\",	"	<<	token	<<	")";
								if(temp_str)	{
												free(temp_str);
												temp_str	=	nullptr;
								}
								return	ss.str();

				}
				char	string[100];
				snprintf(string,	sizeof(string),	"Trick:MonteVarFile	the	input	file	\"%s\"	is	not	open	for	reading",	file_name.c_str());
				exec_terminate_with_return(-1,	__FILE__,	__LINE__,	string);

				return	NULL;
}

void	Trick::MonteVarFile::set_file_name(std::string	in_file_name)	{
				delete	input_file_stream;

				input_file_stream	=	new	std::ifstream(in_file_name.c_str(),	std::ifstream::in);
				if	(input_file_stream->fail())	{
								std::stringstream	string_stream;

								string_stream	<<	"Error:	"	<<	strerror(errno)	<<	std::endl
																						<<	"							Trick:MonteVarFile	input	file	\""	<<	in_file_name	<<	"\"	failed	to	open";

								exec_terminate_with_return(-1,	__FILE__,	__LINE__,	string_stream.str().c_str());
				}
				input_file_stream->close();
				file_name	=	in_file_name;
}

void	Trick::MonteVarFile::set_column(unsigned	int	in_column)	{
				column	=	in_column;
}
