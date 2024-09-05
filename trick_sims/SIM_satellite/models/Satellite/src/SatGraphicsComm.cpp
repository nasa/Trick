#include	<unistd.h>
#include	<iostream>
#include	"SatGraphicsComm.hh"

SatGraphicsComm::SatGraphicsComm()	{

					syscmd	=	"Scene	&";
					connection.port	=	9000;
					connection.hostname	=	"localhost";

}

int	SatGraphicsComm::connect()	{

				int	ret,	num_attempts;

				system(syscmd);
				std::cout	<<	"SatGraphicsComm:	Starting	graphics	server	using:	\""	<<	syscmd	<<	"\""	<<	std::endl;

				tc_error(	&connection,	0);

				num_attempts	=	0;
				while(1)	{
								ret	=	tc_connect(	&connection);
								if		(ret	==	TC_SUCCESS)	{
												std::cerr	<<	"SatGraphicsComm:	CONNECT."	<<	std::endl;
												break;
								}	else	{
												num_attempts	++;
												if	(	num_attempts	==	30	)	{
																std::cerr	<<	"SatGraphicsComm:	Couldn't	connect	to	graphics	server."	<<	std::endl;
																return	1;
												}
								}
								sleep(1);
				}

				return	0;
}


int	SatGraphicsComm::send_packet(Satellite*	sat)	{

				double	packet[15];
//				double	packet[24];

				packet[0]	=	sat->pos[0];
				packet[1]	=	sat->pos[1];
				packet[2]	=	sat->pos[2];

				packet[3]	=	sat->vel[0];
				packet[4]	=	sat->vel[1];
				packet[5]	=	sat->vel[2];

				packet[6]		=	sat->R[0][0];
				packet[7]		=	sat->R[0][1];
				packet[8]		=	sat->R[0][2];
				packet[9]		=	sat->R[1][0];
				packet[10]	=	sat->R[1][1];
				packet[11]	=	sat->R[1][2];
				packet[12]	=	sat->R[2][0];
				packet[13]	=	sat->R[2][1];
				packet[14]	=	sat->R[2][2];

//				packet[15]	=	sat->Rplat[0][0];
//				packet[16]	=	sat->Rplat[0][1];
//				packet[17]	=	sat->Rplat[0][2];
//				packet[18]	=	sat->Rplat[1][0];
//				packet[19]	=	sat->Rplat[1][1];
//				packet[20]	=	sat->Rplat[1][2];
//				packet[21]	=	sat->Rplat[2][0];
//				packet[22]	=	sat->Rplat[2][1];
//				packet[23]	=	sat->Rplat[2][2];

				tc_write(&connection,	(char*)&packet,	sizeof(packet));

				return	0;
}
