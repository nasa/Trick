/*****************************************************************************
PURPOSE:
				(Multi-Threading)
*****************************************************************************/

#include	<iostream>
#include	"../include/Thread3.h"
#include	"sim_services/Clock/include/GetTimeOfDayClock.hh"

Thread3::Thread3()
{
				m_bBusy	=	false;;
				m_iBusyNSecs	=	96000000;
				m_iCount	=	0;
}

Thread3::~Thread3()
{

}

int	Thread3::Step()
{
				STRUCT_TIME	tp;

				/*if(m_bSleep)
				{
								struct	timespec	reg,	rem;
								reg.tv_sec	=	m_iSleepSecs;
								reg.tv_nsec	=	m_iSleepNSecs;
								nanosleep(&reg,&rem);
				}*/
				bool	bCont	=	true;
				struct	timespec	ts_Start,ts_Current,ts_Difference;
				//clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Start);
				GETTIME(&tp);
				ts_Start.tv_sec		=	tp.tv_sec;
				ts_Start.tv_nsec	=	FRACTION_SECOND	*	1000;
				if(m_bBusy)
				{
								while(bCont)	//	Busy	loop
								{
												//clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Current);
												GETTIME(&tp);
												ts_Current.tv_sec		=	tp.tv_sec;
												ts_Current.tv_nsec	=	FRACTION_SECOND	*	1000;
												if	((ts_Current.tv_nsec-ts_Start.tv_nsec)<0)
												{
																ts_Difference.tv_sec	=	ts_Current.tv_sec-ts_Start.tv_sec-1;
																ts_Difference.tv_nsec	=	1000000000+ts_Current.tv_nsec-ts_Start.tv_nsec;
												}
												else
												{
																ts_Difference.tv_sec	=	ts_Current.tv_sec-ts_Start.tv_sec;
																ts_Difference.tv_nsec	=	ts_Current.tv_nsec-ts_Start.tv_nsec;
												}
												if(ts_Difference.tv_nsec	>=	m_iBusyNSecs)
												{
																bCont	=	false;
												}
								}
				}
				m_iCount	+=	4;
				return	0;
}

