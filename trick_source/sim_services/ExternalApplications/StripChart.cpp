
#include	<string>

#include	"trick/StripChart.hh"
#include	"trick/ExternalApplicationManager.hh"

Trick::StripChart::StripChart()	{
				command	+=	std::string("trick-stripchart");
				file_set	=	false;
}

void	Trick::StripChart::set_arguments(std::string	args)	{
				ExternalApplication::set_arguments(args);
				file_set	=	false;
}

std::string	Trick::StripChart::create_arguments_string()	{
				ExternalApplication::create_arguments_string();

				std::ostringstream	oss;

				if	(host_source	!=	NONE)	{
								oss	<<	"	"	<<	host;
				}

				if	(port_source	!=	NONE)	{
								oss	<<	"	"	<<	port;
				}

				if	(file_set)	{
								oss	<<	"	-input	"	<<	input_file;
				}

				return	oss.str();
}

void	Trick::StripChart::set_input_file(std::string	filename)	{
				if	(!filename.empty())	{
								file_set	=	true;
								input_file	=	filename;

								//!	Ensure	this	application	is	on	the	queue.
								Trick::add_external_application(*this);
				}
				else	{
								file_set	=	false;
				}
}

std::string	Trick::StripChart::get_input_file()	{
				return	input_file;
}
