/*****************************************************************************
PURPOSE:
				(Multi-Threading)
*****************************************************************************/

#include	<iostream>
#include	"../include/Thread1.h"
#include	"sim_services/Clock/include/clock_proto.h"

Thread1::Thread1(int	in_usec)	:	m_iBusyUSecs(in_usec)
{
				m_bBusy	=	false;
				m_iCount	=	0;
}

Thread1::~Thread1()
{

}

int	Thread1::Step()
{
				long	long	start_time	,	current_time	;

				start_time	=	clock_wall_time()	;
				if(m_bBusy)
				{
								current_time	=	clock_wall_time()	;
								while(current_time	-	start_time	<	m_iBusyUSecs	)	//	Busy	loop
								{
												current_time	=	clock_wall_time()	;
								}
				}
				m_iCount	+=	1;
				return	0;
}

