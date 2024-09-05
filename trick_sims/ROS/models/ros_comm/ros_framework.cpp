
#include	"ros/ros.h"
#include	"trick/command_line_protos.h"
#include	"ros_framework.hh"

RosFramework::RosFramework(std::string	app_name)	{
				int	argc	=	command_line_args_get_argc()	;
				char	**argv	=	command_line_args_get_argv()	;
				ros::init(argc,	argv,	app_name.c_str(),	ros::init_options::NoSigintHandler)	;
}

RosFramework::~RosFramework()	{}

int	RosFramework::shutdown()	{
				ros::shutdown()	;
				return	0	;
}

