#include	"trick/ExternalApplicationManager.hh"
#include	"trick/ExternalApplication.hh"

#include	<vector>

namespace	Trick	{

				namespace	{
								std::vector<ExternalApplication	*>	externalApplications;
				}

				std::vector<ExternalApplication	*>	&get_external_applications()	{
								return	externalApplications;
				}

				void	add_external_application(ExternalApplication	&externalApplication)	{
								for	(std::vector<ExternalApplication	*>::iterator	i	=	externalApplications.begin();
										i	<	externalApplications.end();	++i)	{
												if	(*i	==	&externalApplication)	{
																return;
												}
								}
								externalApplications.push_back(&externalApplication);
				}

				void	remove_external_application(ExternalApplication	&externalApplication)	{
								for	(std::vector<ExternalApplication	*>::iterator	i	=	externalApplications.begin();
										i	<	externalApplications.end();	++i)	{
												if	(*i	==	&externalApplication)	{
																externalApplications.erase(i);
																break;
												}
								}
				}

				void	remove_all_external_applications()	{
								externalApplications.clear();
				}

				void	launch_all_external_applications()	{
								for	(std::vector<ExternalApplication	*>::iterator	i	=	externalApplications.begin();
										i	<	externalApplications.end();	++i)	{
												(*i)->launch();
								}
				}

}
