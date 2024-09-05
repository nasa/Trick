
#include	<iostream>
#include	<unistd.h>
#include	<string>

#include	"trick/MessageFile.hh"
#include	"trick/command_line_protos.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"



/**
@details
-#	Initializes	everything
*/
Trick::MessageFile::MessageFile()	{
				enabled	=	1	;
				color	=	0	;
}

/**
@details
-#	Overwrites	the	file	name
*/
int	Trick::MessageFile::set_file_name(std::string	in_name)	{

				file_name	=	in_name	;
				return(0)	;

}

/**
@details
-#	If	enabled	and	level	<	100
				-#	Write	the	header	and	message	to	the	file	stream
				-#	Flush	the	stream
*/
void	Trick::MessageFile::update(	unsigned	int	level	,	std::string	header,	std::string	message	)	{

				if	(	enabled	&&	level	<	100	)	{
								out_stream	<<	header	<<	message	;
								out_stream.flush()	;
				}

}

/**
@details
-#	Deletes	the	current	output	file
-#	Opens	a	new	file	with	the	name	"file_name"
*/
int	Trick::MessageFile::init()	{

				unlink((std::string(command_line_args_get_output_dir())	+	"/"	+	file_name).c_str())	;

				//	Create	the	directory	if	we	need	to
				int	pos	=	file_name.find_last_of("/");
				if	(pos	!=	std::string::npos)	{
								std::string	dir	=	std::string(command_line_args_get_output_dir())	+	"/"	+	file_name.substr(0,	pos);
								create_path(dir.c_str());
				}

				out_stream.open((std::string(command_line_args_get_output_dir())	+	"/"	+	file_name).c_str()	,	std::fstream::out	|	std::fstream::app	)	;
				if	(!out_stream.is_open())	{
								message_publish(MSG_ERROR,	"Failed	to	open	message	file	%s\n",file_name.c_str());
				}
				return(0)	;
}

int	Trick::MessageFile::restart()	{
				out_stream.open((std::string(command_line_args_get_output_dir())	+	"/"	+	file_name).c_str()	,	std::fstream::out	|	std::fstream::app	)	;
				return(0)	;
}

/**
@details
-#	Close	the	file	stream
*/
Trick::MessageFile::~MessageFile()	{
				out_stream.close()	;
}

