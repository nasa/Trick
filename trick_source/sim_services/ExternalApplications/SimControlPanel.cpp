
#include	<string>

#include	"trick/SimControlPanel.hh"

Trick::SimControlPanel::SimControlPanel()	{
				command	+=	std::string("trick-simcontrol");
				auto_exit	=	false;
}

std::string	Trick::SimControlPanel::create_arguments_string()	{
				ExternalApplication::create_arguments_string();

				std::ostringstream	oss;

				if	(host_source	!=	NONE)	{
								oss	<<	"	"	<<	host;
				}

				if	(port_source	!=	NONE)	{
								oss	<<	"	"	<<	port;
				}

				if	(auto_exit)	{
					oss	<<	"	-auto_exit";
				}

				return	oss.str();
}
