
#include	<iostream>
#include	<string>
#include	<typeinfo>

#include	"trick/ExternalApplicationManager.hh"
#include	"trick/SimControlPanel.hh"
#include	"trick/TrickView.hh"
#include	"trick/MalfunctionsTrickView.hh"
#include	"trick/MonteMonitor.hh"
#include	"trick/StripChart.hh"

static	void	*getFirstInstanceOfType(const	std::type_info	&type)	{
				std::vector<Trick::ExternalApplication	*>	&externalApplications	=
						Trick::get_external_applications();
				for	(std::vector<Trick::ExternalApplication	*>::iterator	i	=	externalApplications.begin();
						i	<	externalApplications.end();	++i)	{
								if	(typeid(**i)	==	type)	{
												return	(void	*)*i;
								}
				}
				return	NULL;
}

/*static	void	printDeprecationMessage(const	char	*file,	int	line,	const	char	*function)	{
				std::cout	<<	file	<<	":"	<<	line	<<	":	[33;1mwarning:[0m	"	<<	function	<<	"	is	deprecated.	"
														<<	"See	section	\"5.2	Runtime	GUIs\"	of	the	Trick	User's	Guide."	<<	std::endl;
}*/

extern	"C"	void	sim_control_panel_set_enabled(int	enabled)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::ExternalApplication	*externalApplication	=
						(Trick::ExternalApplication	*)getFirstInstanceOfType(typeid(Trick::SimControlPanel));

				if	(enabled	&&	!externalApplication)	{
								Trick::add_external_application(*new	Trick::SimControlPanel());
				}
				else	if	(!enabled	&&	externalApplication)	{
								Trick::remove_external_application(*externalApplication);
				}
}

extern	"C"	void	sim_control_panel_auto_exit_set_enabled(int	enabled)	{
		Trick::SimControlPanel	*simControlApplication	=
							(Trick::SimControlPanel	*)getFirstInstanceOfType(typeid(Trick::SimControlPanel));

		if	(simControlApplication)	{
			simControlApplication->auto_exit	=	enabled;
		}
}

extern	"C"	void	trick_view_set_enabled(int	enabled)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::ExternalApplication	*externalApplication	=
						(Trick::ExternalApplication	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(enabled	&&	!externalApplication)	{
								Trick::add_external_application(*new	Trick::TrickView());
				}
				else	if	(!enabled	&&	externalApplication)	{
								Trick::remove_external_application(*externalApplication);
				}
}

extern	"C"	void	malfunctions_trick_view_set_enabled(int	enabled)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::ExternalApplication	*externalApplication	=
						(Trick::ExternalApplication	*)getFirstInstanceOfType(typeid(Trick::MalfunctionsTrickView));

				if	(enabled	&&	!externalApplication)	{
								Trick::add_external_application(*new	Trick::MalfunctionsTrickView());
				}
				else	if	(!enabled	&&	externalApplication)	{
								Trick::remove_external_application(*externalApplication);
				}
}

extern	"C"	void	monte_monitor_set_enabled(int	enabled)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::ExternalApplication	*externalApplication	=
						(Trick::ExternalApplication	*)getFirstInstanceOfType(typeid(Trick::MonteMonitor));

				if	(enabled	&&	!externalApplication)	{
								Trick::add_external_application(*new	Trick::MonteMonitor());
				}
				else	if	(!enabled	&&	externalApplication)	{
								Trick::remove_external_application(*externalApplication);
				}
}

extern	"C"	void	stripchart_set_enabled(int	enabled)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::ExternalApplication	*externalApplication	=
						(Trick::ExternalApplication	*)getFirstInstanceOfType(typeid(Trick::StripChart));

				if	(enabled	&&	!externalApplication)	{
								Trick::add_external_application(*new	Trick::StripChart());
				}
				else	if	(!enabled	&&	externalApplication)	{
								Trick::remove_external_application(*externalApplication);
				}
}

extern	"C"	int	sim_control_panel_get_enabled(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);
				return	getFirstInstanceOfType(typeid(Trick::SimControlPanel))	!=	NULL;
}

extern	"C"	int	trick_view_get_enabled(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);
				return	getFirstInstanceOfType(typeid(Trick::TrickView))	!=	NULL;
}

extern	"C"	int	malfunctions_trick_view_get_enabled(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);
				return	getFirstInstanceOfType(typeid(Trick::MalfunctionsTrickView))	!=	NULL;
}

extern	"C"	int	monte_monitor_get_enabled(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);
				return	getFirstInstanceOfType(typeid(Trick::MonteMonitor))	!=	NULL;
}

extern	"C"	int	stripchart_get_enabled(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);
				return	getFirstInstanceOfType(typeid(Trick::StripChart))	!=	NULL;
}

extern	"C"	void	sim_control_panel_set_startup_command(const	char	*startup_command)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::SimControlPanel	*externalApplication	=	(Trick::SimControlPanel	*)getFirstInstanceOfType(typeid(Trick::SimControlPanel));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::SimControlPanel();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_startup_command(startup_command	?	std::string(startup_command)	:	"");
}

extern	"C"	void	trick_view_set_startup_command(const	char	*startup_command)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::TrickView	*externalApplication	=	(Trick::TrickView	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::TrickView();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_startup_command(startup_command	?	std::string(startup_command)	:	"");
}

extern	"C"	void	malfunctions_trick_view_set_startup_command(const	char	*startup_command)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MalfunctionsTrickView	*externalApplication	=	(Trick::MalfunctionsTrickView	*)getFirstInstanceOfType(typeid(Trick::MalfunctionsTrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::MalfunctionsTrickView();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_startup_command(startup_command	?	std::string(startup_command)	:	"");
}

extern	"C"	void	monte_monitor_set_startup_command(const	char	*startup_command)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MonteMonitor	*externalApplication	=	(Trick::MonteMonitor	*)getFirstInstanceOfType(typeid(Trick::MonteMonitor));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::MonteMonitor();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_startup_command(startup_command	?	std::string(startup_command)	:	"");
}

extern	"C"	void	stripchart_set_startup_command(const	char	*startup_command)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::StripChart	*externalApplication	=	(Trick::StripChart	*)getFirstInstanceOfType(typeid(Trick::StripChart));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::StripChart();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_startup_command(startup_command	?	std::string(startup_command)	:	"");
}

extern	"C"	const	char	*sim_control_panel_get_startup_command(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::SimControlPanel	*externalApplication	=	(Trick::SimControlPanel	*)getFirstInstanceOfType(typeid(Trick::SimControlPanel));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::SimControlPanel();
								Trick::add_external_application(*externalApplication);
				}

				return	externalApplication->get_startup_command_c_str();
}

extern	"C"	const	char	*trick_view_get_startup_command(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::TrickView	*externalApplication	=	(Trick::TrickView	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::TrickView();
								Trick::add_external_application(*externalApplication);
				}

				return	externalApplication->get_startup_command_c_str();
}

extern	"C"	const	char	*malfunctions_trick_view_get_startup_command(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MalfunctionsTrickView	*externalApplication	=	(Trick::MalfunctionsTrickView	*)getFirstInstanceOfType(typeid(Trick::MalfunctionsTrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::MalfunctionsTrickView();
								Trick::add_external_application(*externalApplication);
				}

				return	externalApplication->get_startup_command_c_str();
}

extern	"C"	const	char	*monte_monitor_get_startup_command(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MonteMonitor	*externalApplication	=	(Trick::MonteMonitor	*)getFirstInstanceOfType(typeid(Trick::MonteMonitor));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::MonteMonitor();
								Trick::add_external_application(*externalApplication);
				}

				return	externalApplication->get_startup_command_c_str();
}

extern	"C"	const	char	*stripchart_get_startup_command(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::StripChart	*externalApplication	=	(Trick::StripChart	*)getFirstInstanceOfType(typeid(Trick::StripChart));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::StripChart();
								Trick::add_external_application(*externalApplication);
				}

				return	externalApplication->get_startup_command_c_str();
}

extern	"C"	void	sim_control_panel_launch(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::SimControlPanel	*externalApplication	=	(Trick::SimControlPanel	*)getFirstInstanceOfType(typeid(Trick::SimControlPanel));

				if	(externalApplication)	{
								externalApplication->launch();
				}
}

extern	"C"	void	trick_view_launch(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::TrickView	*externalApplication	=	(Trick::TrickView	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(externalApplication)	{
								externalApplication->launch();
				}
}

extern	"C"	void	malfunctions_trick_view_launch(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MalfunctionsTrickView	*externalApplication	=	(Trick::MalfunctionsTrickView	*)getFirstInstanceOfType(typeid(Trick::MalfunctionsTrickView));

				if	(externalApplication)	{
								externalApplication->launch();
				}
}

extern	"C"	void	monte_monitor_launch(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::MonteMonitor	*externalApplication	=	(Trick::MonteMonitor	*)getFirstInstanceOfType(typeid(Trick::MonteMonitor));

				if	(externalApplication)	{
								externalApplication->launch();
				}
}

extern	"C"	void	stripchart_launch(void)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::StripChart	*externalApplication	=	(Trick::StripChart	*)getFirstInstanceOfType(typeid(Trick::StripChart));

				if	(externalApplication)	{
								externalApplication->launch();
				}
}

extern	"C"	void	trick_view_set_cycle_period(double	period)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::TrickView	*externalApplication	=	(Trick::TrickView	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::TrickView();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_cycle_period(period);
}

extern	"C"	void	trick_view_add_auto_load_file(const	char	*file)	{
				//printDeprecationMessage(__FILE__,	__LINE__,	__FUNCTION__);

				Trick::TrickView	*externalApplication	=	(Trick::TrickView	*)getFirstInstanceOfType(typeid(Trick::TrickView));

				if	(!externalApplication)	{
								externalApplication	=	new	Trick::TrickView();
								Trick::add_external_application(*externalApplication);
				}

				externalApplication->set_auto_open_file(file	?	std::string(file)	:	"");
}
