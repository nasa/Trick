
#include	<string>
#include	<iostream>

#include	"trick/TrickView.hh"
#include	"trick/variable_server_proto.h"

Trick::TrickView::TrickView()	{
				command	+=	std::string("trick-tv");
				strip_charts_only_set	=	open_file_set	=	open_set_file_set	=	set_file_set	=	false;
}

void	Trick::TrickView::set_arguments(std::string	args)	{
				ExternalApplication::set_arguments(args);
				strip_charts_only_set	=	open_file_set	=	open_set_file_set	=	set_file_set	=	false;
}

std::string	Trick::TrickView::create_arguments_string()	{
				std::ostringstream	oss;
				oss	<<	ExternalApplication::create_arguments_string();

				if	(strip_charts_only_set	&&	strip_charts_only)	{
								oss	<<	"	--stripChartsOnly"	;
				}

				if	(open_file_set)	{
								oss	<<	"	--open	"	<<	open_file;
				}

				if	(open_set_file_set)	{
								oss	<<	"	--openSet	"	<<	open_set_file;
				}

				if	(set_file_set)	{
								oss	<<	"	--set	"	<<	set_file;
				}

				return	oss.str();
}

void	Trick::TrickView::set_strip_charts_only(bool	in_strip_charts_only)	{
				strip_charts_only	=	in_strip_charts_only;
				strip_charts_only_set	=	true;
}

bool	Trick::TrickView::get_strip_charts_only()	{
				return	strip_charts_only;
}

void	Trick::TrickView::set_auto_open_file(std::string	file)	{
				open_file	=	file;
				open_file_set	=	true;
}

std::string	Trick::TrickView::get_auto_open_file()	{
				return	open_file;
}

void	Trick::TrickView::set_auto_open_and_set_file(std::string	file)	{
				open_set_file	=	file;
				open_set_file_set	=	true;
}

std::string	Trick::TrickView::get_auto_open_and_set_file()	{
				return	open_set_file;
}

void	Trick::TrickView::set_auto_set_file(std::string	file)	{
				set_file	=	file;
				set_file_set	=	true;
}

std::string	Trick::TrickView::get_auto_set_file()	{
				return	set_file;
}
