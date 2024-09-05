
#include	<string>

#include	"trick/MalfunctionsTrickView.hh"

Trick::MalfunctionsTrickView::MalfunctionsTrickView()	{
				command	+=	std::string("trick-mtv");
}

std::string	Trick::MalfunctionsTrickView::create_arguments_string()	{
				ExternalApplication::create_arguments_string();

				std::ostringstream	oss;

				if	(host_source	!=	NONE)	{
								oss	<<	"	"	<<	host;
				}

				if	(port_source	!=	NONE)	{
								oss	<<	"	"	<<	port;
				}

				return	oss.str();
}
