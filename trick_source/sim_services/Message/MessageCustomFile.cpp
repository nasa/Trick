
#include	<iostream>
#include	<unistd.h>


#include	"trick/MessageCustomFile.hh"
#include	"trick/Message_proto.hh"


int	Trick::MessageCustomFile::level_counter	=	200;

/**
@details
-#	Initializes	everything
*/
Trick::MessageCustomFile::MessageCustomFile()	:	_this_level(level_counter++)	{
}

/**
@details
-#	If	enabled	and	level	is	this	file's	level
				-#	Write	the	header	and	message	to	the	file	stream
-#	Flush	the	stream
*/
void	Trick::MessageCustomFile::update(	unsigned	int	level	,	std::string	header,	std::string	message	)	{
				if	(	enabled	&&	level	==	_this_level	)	{
								out_stream	<<	header	<<	message	;
								out_stream.flush()	;
				}
}

int	Trick::MessageCustomFile::get_level()	const	{
				return	_this_level;
}

void	Trick::MessageCustomFile::set_level(int	level)	{
				if	(level	>=	0)
								_this_level	=	level;
}

void	Trick::MessageCustomFile::set_name(std::string	name)	{
				if	(name	==	"")	{
								name	=	"CustomLog"	+	std::to_string(_this_level);
				}				
				this->name	=	name;
}
void	Trick::MessageCustomFile::set_file_name(std::string	file_name)	{
				this->file_name	=	file_name;
}


