
#include	<sstream>
#include	<iostream>
#include	<stdlib.h>
#include	<dlfcn.h>
#include	<vector>
#include	<cstring>

#include	"trick/ExternalApplication.hh"
#include	"trick/ExternalApplicationManager.hh"
#include	"trick/variable_server_proto.h"
#include	"trick/env_proto.h"
#include	"trick/command_line_protos.h"
#include	"trick/MemoryManager.hh"
extern	Trick::MemoryManager*	trick_MM;

Trick::ExternalApplication::ExternalApplication()	:
				command(std::string(env_get_var("TRICK_HOME")	+	std::string("/bin/")))	{
				host_source	=	port_source	=	AUTO;
				cycle_period_set	=	minimum_cycle_period_set	=	disconnect_behavior_set	=	height_set	=
						width_set	=	x_set	=	y_set	=	auto_reconnect_set	=	false;
						
						//	c_intf	uses	char	*,	we	manage	the	memory	here	in	external	application
						command_c_str	=	(char*)trick_MM->declare_var("char",	(command.size()	+	1)	);
						strcpy(command_c_str,	command.c_str());
						allocations.push_back(command_c_str);
}

Trick::ExternalApplication::~ExternalApplication()	{
				for(std::vector<char*>::iterator	it	=	allocations.begin();	it	!=	allocations.end();	++it)	{
								trick_MM->delete_var(	(void*)*it	);
				}
				allocations.clear();
}

void	Trick::ExternalApplication::set_startup_command(std::string	in_command)	{
				command	=	in_command;
				command_c_str	=	(char*)trick_MM->declare_var("char",	(command.size()	+	1)	);
				strcpy(command_c_str,	command.c_str());
				allocations.push_back((command_c_str));
}

std::string	Trick::ExternalApplication::get_startup_command()	{
				return	command;
}

const	char	*	Trick::ExternalApplication::get_startup_command_c_str()	{
				return	command_c_str;
}

void	Trick::ExternalApplication::add_arguments(std::string	args)	{
				arguments	<<	"	"	<<	args;
}

void	Trick::ExternalApplication::set_arguments(std::string	args)	{
				arguments.str(args);
				host_source	=	port_source	=	NONE;
				cycle_period_set	=	disconnect_behavior_set	=	height_set	=	width_set	=	x_set	=	y_set	=	false;
}

std::string	Trick::ExternalApplication::get_arguments()	{
				return	arguments.str()	+	create_arguments_string();
}

void	Trick::ExternalApplication::set_host(std::string	in_host)	{
				host	=	in_host;
				host_source	=	USER;
}

std::string	Trick::ExternalApplication::get_host()	{
				return	host;
}

void	Trick::ExternalApplication::set_port(unsigned	short	in_port)	{
				port	=	in_port;
				port_source	=	USER;
}

unsigned	short	Trick::ExternalApplication::get_port()	{
				return	port;
}

void	Trick::ExternalApplication::set_auto_reconnect(bool	in_auto_reconnect)	{
				auto_reconnect	=	in_auto_reconnect;
				auto_reconnect_set	=	true;
}

bool	Trick::ExternalApplication::get_auto_reconnect()	{
				return	auto_reconnect;
}

void	Trick::ExternalApplication::set_cycle_period(double	in_cycle_period)	{
				cycle_period	=	in_cycle_period;
				cycle_period_set	=	true;
}

double	Trick::ExternalApplication::get_cycle_period()	{
				return	cycle_period;
}

void	Trick::ExternalApplication::set_minimum_cycle_period(double	in_minimum_cycle_period)	{
				minimum_cycle_period	=	in_minimum_cycle_period;
				minimum_cycle_period_set	=	true;
}

double	Trick::ExternalApplication::get_minimum_cycle_period()	{
				return	minimum_cycle_period;
}

void	Trick::ExternalApplication::set_disconnect_behavior(DisconnectBehavior	in_disconnect_behavior)	{
				disconnect_behavior	=	in_disconnect_behavior;
				disconnect_behavior_set	=	true;
}

Trick::ExternalApplication::DisconnectBehavior	Trick::ExternalApplication::get_disconnect_behavior()	{
				return	disconnect_behavior;
}

void	Trick::ExternalApplication::set_height(int	in_height)	{
				height	=	in_height;
				height_set	=	true;
}

int	Trick::ExternalApplication::get_height()	{
				return	height;
}

void	Trick::ExternalApplication::set_width(int	in_width)	{
				width	=	in_width;
				width_set	=	true;
}

int	Trick::ExternalApplication::get_width()	{
				return	width;
}

void	Trick::ExternalApplication::set_x(int	in_x)	{
				x	=	in_x;
				x_set	=	true;
}

int	Trick::ExternalApplication::get_x()	{
				return	x;
}

void	Trick::ExternalApplication::set_y(int	in_y)	{
				y	=	in_y;
				y_set	=	true;
}

int	Trick::ExternalApplication::get_y()	{
				return	y;
}

/**	@par	Detailed	Design	*/
void	Trick::ExternalApplication::launch()	{
				/**	<ul><li>	If	the	application	is	enabled:	*/
				if	(var_server_get_enabled())	{
								/**	<ul><li>	Execute	the	command	and	pass	it	any	arguments.	*/
								std::ostringstream	oss;

								int	argc;
								char	**argv;

								argc	=	command_line_args_get_argc()	;
								argv	=	command_line_args_get_argv()	;

								oss	<<	command	<<	"	"	<<	arguments.str()	<<	"	"	<<	create_arguments_string()	;
								oss	<<	"	&";

								std::cout	<<	oss.str()	<<	std::endl;
								system(oss.str().c_str());
				}
}

std::string	Trick::ExternalApplication::create_arguments_string()	{
				std::ostringstream	oss;

				switch	(host_source)	{
								case	AUTO:
												host	=	var_server_get_hostname();
								case	USER:
												oss	<<	"	--host	"	<<	host;
								case	NONE:
												break;
				}

				switch	(port_source)	{
								case	AUTO:
												port	=	var_server_get_port();
								case	USER:
												oss	<<	"	--port	"	<<	port;
								case	NONE:
												break;
				}

				if	(minimum_cycle_period_set)	{
								oss	<<	"	--minCycle	"	<<	minimum_cycle_period;
				}

				if	(cycle_period_set)	{
								oss	<<	"	--cycle	"	<<	cycle_period;
				}

				if	(disconnect_behavior_set)	{
								switch(disconnect_behavior)	{
												case	CLOSE:
																oss	<<	"	--disconnectBehavior	Close";
																break;
												case	NOTIFY:
																oss	<<	"	--disconnectBehavior	Notify";
																break;
												case	NOTHING:
																oss	<<	"	--disconnectBehavior	Nothing";
																break;
												default:
																std::cout	<<	__FILE__	<<	":"	<<	__LINE__	<<	":	[33;1mwarning:[0m	"
																										<<	"Ignoring	invalid	value	("	<<	disconnect_behavior
																										<<	")	for	disconnect_behavior."	<<	std::endl;
																break;
								}
				}

				if	(height_set)	{
								oss	<<	"	--height	"	<<	height;
				}

				if	(width_set)	{
								oss	<<	"	--width	"	<<	width;
				}

				if	(x_set)	{
								oss	<<	"	--x	"	<<	x;
				}

				if	(y_set)	{
								oss	<<	"	--y	"	<<	y;
				}

				if	(auto_reconnect_set)	{
								oss	<<	"	--autoReconnect	"	<<	(auto_reconnect	?	"true"	:	"false");
				}

				return	oss.str();
}
