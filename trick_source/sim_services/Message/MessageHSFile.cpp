
#include	<iostream>
#include	<unistd.h>

#include	"trick/MessageHSFile.hh"
#include	"trick/command_line_protos.h"

/**
@details
-#	Initializes	everything
*/
Trick::MessageHSFile::MessageHSFile()	{
				name	=	"file";
				file_name	=	"send_hs";
}

/**
@details
-#	If	enabled	and	level	<	100
				-#	Write	the	header	and	message	to	the	file	stream
				-#	Flush	the	stream
*/
void	Trick::MessageHSFile::update(	unsigned	int	level	,	std::string	header,	std::string	message	)	{

				if	(	enabled	&&	level	<	100	)	{
								out_stream	<<	header	<<	message	;
								out_stream.flush()	;
				}

}


