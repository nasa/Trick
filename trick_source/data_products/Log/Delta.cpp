//	The	delta	stream	returns	the	difference	between
//	two	streams	designated	with	the	<var:run>	syntax

#include	<stdio.h>
#include	<iostream>
#include	<string>
using	namespace	std	;

#include	"Delta.hh"
#include	"DataStreamFactory.hh"

Delta::Delta(const	char	*	deltaStatement,	const	char*	timeName)
{
								int	ret	;
								char	vr1[1024];
								char	vr2[1024];

								DataStreamFactory	dsf	;

								//	The	delta	statement	is	formatted	like:
								//	delta(<var1:run1>,	<var2:run2>)

								//	Extract	<var1:run1>	<var2:run2>
								ret	=	sscanf(deltaStatement,	"delta(%[^,],%[^)]",	vr1,	vr2);
								if	(	ret	!=	2	)	{
																fprintf(stderr,	"ERROR:	Delta::Delta()	received	bad	spec	"
																																"\"%s\"\nAborting.\n",	deltaStatement);
																exit(-1);
								}

								dataStream1_	=	dsf.create(NULL,	vr1,	timeName);
								dataStream2_	=	dsf.create(NULL,	vr2,	timeName);

								dsg_.add(dataStream1_);
								dsg_.add(dataStream2_);
}

Delta::~Delta()
{
								delete	dataStream1_	;
								delete	dataStream2_	;
}


int	Delta::get(	double	*	time	,	double	*	value	)
{
								int	ret	;
								double	val1,	val2	;

								ret	=	dsg_.matchTimeStamps();
								if	(	!	ret	)	{
																//	At	end	of	input	streams
																return	ret	;
								}

								dsg_.getLastRead(dataStream1_,	time,	&val1);
								ret	=	dsg_.getLastRead(dataStream2_,	time,	&val2);

								*value	=	val1	-	val2	;

								return(ret)	;
}

int	Delta::peek(	double	*	time	,	double	*	value	)
{
								int	ret	;
								double	val1,	val2	;

								dsg_.getLastRead(dataStream1_,	time,	&val1);
								ret	=	dsg_.getLastRead(dataStream2_,	time,	&val2);

								*value	=	val1	-	val2	;

								return(ret)	;
}

void	Delta::begin()
{
								dsg_.begin();
								return	;
}

int	Delta::end()
{
								return	(dsg_.end())	;
}

int	Delta::step()
{
								dsg_.step();
								return(0)	;
}
