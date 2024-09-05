
#include	"std_msgs/String.h"
#include	"trick/exec_proto.h"
#include	"ros_publish.hh"

#include	"Num.h"

RosPublish::RosPublish()	:	count(0)	{}

int	RosPublish::init()	{
				msg_pub	=	n.advertise<trick_msgs::Num>("num_chatter",	1000)	;
				return	0	;
}

int	RosPublish::publish()	{
				if	(	ros::ok()	)	{
								trick_msgs::Num	tn	;
								tn.first_name	=	"Hakeem"	;
								tn.last_name	=	"Olajuwon"	;
								tn.date	=	"March	29,	1990"	;
								tn.points	=	18	;
								tn.rebounds	=	16	;
								tn.blocks	=	11	;
								tn.assists	=	10	;
								std::cout	<<	tn	<<	std::endl	;
								msg_pub.publish(tn)	;

								ros::spinOnce()	;
				}	else	{
								exec_terminate(__FILE__,	"ros	not	ok")	;
				}
				return	0	;
}

