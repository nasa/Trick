#include	"DataStream.hh"
#include	<float.h>
#include	<math.h>

DataStream::DataStream()	{
								unitStr_					=	"1"	;
								unitTimeStr_	=	"s"	;
}

DataStream::~DataStream()
{
								//	Nada
}

int	DataStream::getValueAtTime(double	time	,	double	*	value	)	{

								double	value_time	;
								int	ret	=	0	;

								begin()	;
								while	(	(ret	=	get(	&value_time	,	value	))	&&
																(fabs(	value_time	-	time	)	>	1e-9	))	;

								return(ret)	;

}

string	DataStream::getFileName()	{
								return(fileName_)	;
}

string	DataStream::getUnit()	{
								return(unitStr_)	;
}

string	DataStream::getTimeUnit()	{
								return(unitTimeStr_)	;
}

