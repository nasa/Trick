#include	<float.h>
#include	<math.h>
#include	"DataStreamGroup.hh"

DataStreamGroup::DataStreamGroup()
{
								startTime_	=	0.0	;
								stopTime_		=	DBL_MAX		;
								frequency_	=	0.0	;
								preserveTime_	=	1	;
								nextFrequencyCheck_	=	0.0	;
								frequencyMultiple_	=	0.0	;
								isEOF_	=	0	;
								timeMatchTolerance_	=	1.0e-9	;
}

DataStreamGroup::~DataStreamGroup()	{
}

void	DataStreamGroup::setTimeMatchTolerance(	double	tol	)
{
								timeMatchTolerance_	=	tol	;
}

void	DataStreamGroup::setFrequency(	double	freq	)	{
								frequency_	=	freq	;
}

void	DataStreamGroup::setStartTime(	double	time	)	{
								startTime_	=	time	;
}

void	DataStreamGroup::setStopTime(	double	time	)	{
								stopTime_	=	time	;
}

void	DataStreamGroup::add(	DataStream*	ds	)	{

								dataStreams_.push_back(ds)	;
								currTime_.push_back(0.0);
}

void	DataStreamGroup::clear()
{
								dataStreams_.clear()	;
}

void	DataStreamGroup::setPreserveTimeOn()	{
								preserveTime_	=	1	;
}

void	DataStreamGroup::setPreserveTimeOff()	{
								preserveTime_	=	0	;
}

void	DataStreamGroup::begin()
{
								unsigned	int	ii	;
								double	timeStamp	;

								isEOF_	=	0	;

								for	(	ii	=	0;	ii	<	dataStreams_.size();	ii++	)	{
																dataStreams_[ii]->begin();
								}

								//	Reset	frequency
								if	(	frequency_	==	0.0	)	{
																nextFrequencyCheck_	=	DBL_MAX;
								}	else	{
																if	(	startTime_	==	0.0	)	{
																								nextFrequencyCheck_	=	startTime_	+	frequency_	;
																}	else	{
																								nextFrequencyCheck_	=	startTime_	;
																}
								}

	//	Changed	from	1.0	to	0.0	since	this	variable	is	increased	by	1	first
	//	for	getting	next	frequency	check	in	stepInTime_	method.
								frequencyMultiple_	=	0.0	;

								//	If	time	<	start	time	requested	by	user,
								//	continue	marching	through	time	until	start	time	reached
								timeStamp	=	0.0	;
								while	(	timeStamp	<	startTime_	)	{
																step()	;
																timeStamp	=	getTime();
								}

								//	Grab	data	from	each	stream	without	stepping
								for	(	ii	=	0;	ii	<	dataStreams_.size();	ii++	)	{
																dataStreams_[ii]->peek(&lastRead_[dataStreams_[ii]].time,
																																							&lastRead_[dataStreams_[ii]].value);

								}
}


bool	DataStreamGroup::end()
{
								return	isEOF_	;
}

void	DataStreamGroup::step()	{

								unsigned	int	ii	;
								double	pVal	;
								bool	stepAgain	;

								while	(	!	stepInTime_()	)	{
								}

								//	Preserve	time
								//	Keep	all	time	stamps	from	data	streams	on	same	time	stamp
								if	(	preserveTime_	)	{
																pVal	=	currTime_[0]	;
																stepAgain	=	0	;
																for	(	ii	=	1	;	ii	<	currTime_.size()	;	ii++	)	{
																								if	(	pVal	!=	currTime_[ii]	)	{
																																stepAgain	=	1	;
																								}
																}
																if	(	stepAgain	)	{
																								step();
																}
								}

								return	;
}

//	This	returns	minimum	time	stamp	across	all	data	streams
//
//	If	one	streams	has	logged	more	data	in	time,
//	(i.e.	one	log	has	100	seconds	of	data,	another	300	seconds)
//	by	default	getTime()	will	ignore	the	log	that	has	expired
//	all	of	its	data.		Therefore,	the	data	from	the	short	log
//	will	not	force	the	longer	to	be	truncated.
//
//	In	the	case	of	error	plotting,	you	do	not	want	any	extra
//	data	points	if	the	time	stamps	do	not	match
double	DataStreamGroup::getTime(	)	{

								double	minTime	=	DBL_MAX;
								double	time,	val	;
								unsigned	int	ii	;

								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{

																if	(	!	dataStreams_[ii]->end()	)	{
																								dataStreams_[ii]->peek(&time,	&val)	;
																								currTime_[ii]	=	time	;
																}	else	{
																								currTime_[ii]	=	DBL_MAX	;
																}

																if	(currTime_[ii]	<	minTime)	{
																								minTime	=	currTime_[ii];
																}

								}
								return(	minTime	)	;
}


//	Advance	through	records	in	each	data	stream	synchronously
//
//	Example:	If	data	stream	"A"	is	recorded	at	1.0	second,
//	and	"B"	at	10	seconds.		Both	will	advance	at	time	zero.
//	Then	at	time=1.0,	log	"A"	will	have	a	record	read.		"B"	will	not	be	read.
//	At	time=2.0	through	time=9.0	the	same	will	occur.	A	advances,	B	stays	put.
//	At	time=10.0,	both	A	and	B	will	be	read	from.		Both	will	advance.
//	This	keeps	the	data	on	the	same	time	stamp	on	like	intervals.
//
//	Note:	If	flag	preserveTime_	is	set,	step()	will	only	return
//	when	time	stamps	match.
//
//	If	A	has	more	records	than	B	in	time,	by	default,	read	all	of	A's
//	data.		E.g.	'A'	logged	for	300	seconds,		while	'B'	100	seconds,	by	default
//	walk	through	all	of	A's	data
int	DataStreamGroup::stepInTime_()	{


#define	DPLOG_ABS(x)	((x)	<	0.0	?	-(x)	:	(x))

#define	NEXTFREQ_EQUALS_MINTIME	\
								(DPLOG_ABS(nextFrequencyCheck_	-	minTime)	<	timeMatchTolerance_)

#define	NEXTFREQ_LESSTHAN_MINTIME	\
								((nextFrequencyCheck_	-	minTime	<	0.0)	&&	!	NEXTFREQ_EQUALS_MINTIME)

#define	NEXTFREQ_INCR	\
								frequencyMultiple_++	;	\
								nextFrequencyCheck_	=	frequencyMultiple_*frequency_	;


								unsigned	int	ii	;
								double	minTime	;
								int	isEOF	;

								double	t,	x	;

								//	Get	all	values	in	all	data	log	groups
								//	Also	get	mininum	time	stamp	from	all	log	data	files
								minTime	=	getTime();

								//	Take	a	step	through	time	(sounds	like	a	song)
								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{
																if	(	currTime_[ii]	==	minTime	&&	!dataStreams_[ii]->end()	)	{
																								dataStreams_[ii]->get(&t,	&x);
																}
								}

								//	Check	for	end	of	file
								isEOF	=	1	;
								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{
																if	(	!	dataStreams_[ii]->end()	)	{
																								isEOF	=	0	;
																								break	;
																}
								}
								if	(isEOF)	{
																isEOF_	=	1	;
																return(1)	;
								}

								//	Now	that	a	time	step	has	occured
								//	Get	the	new	mininum	time	stamp	and	associated
								//	values	from	all	log	data	files
								minTime	=	getTime();


								//	Step	depending	on	frequency
								if	(minTime	<=	stopTime_)	{

																//	Handle	Frequency	(if	specified)
																if	(	nextFrequencyCheck_	!=	DBL_MAX	)	{

																								if	(	NEXTFREQ_EQUALS_MINTIME	)	{

																																//	Frequency	synced	with	a	data	point
																																//	Stops	recursion
																																NEXTFREQ_INCR	;

																								}	else	if	(	minTime	-	nextFrequencyCheck_	>	0.0	)	{

																																//	Mintime	exceeded	nextFreqCheck
																																//	Frequency	must	be	higher	than
																																//	data	frequency

																																//	Bump	nextFrequencyCheck	past	minTime
																																while	(	NEXTFREQ_LESSTHAN_MINTIME	)	{

																																								NEXTFREQ_INCR	;
																																}

																																if	(	NEXTFREQ_EQUALS_MINTIME	)	{

																																							//	We	synced
																																							NEXTFREQ_INCR	;

																																}	else	{

																																								//	No	sync,	keep	iterating	through	data
																																								return(0);
																																}
																								}	else	{

																																//	Mintime	is	less	than	nextFreq	check
																																//	Need	to	iterate	again
																																return(0)	;
																								}
																}
								}

								if	(minTime	>	stopTime_)	{
																isEOF_	=	1	;
																return(1)	;
								}

								//	Tells	caller	that	time	step	did	indeed	occur
								return(1)	;
}



//	Advance	through	records	in	each	data	stream	synchronously
//	Reading	will	stop	when	one	of	the	streams	is	out	of	data
int	DataStreamGroup::matchTimeStamps()	{

								unsigned	int	ii	;
								int	matched_time_stamps	;
								int	frequency_match	;
								double	maxTime	;

								frequency_match	=	0	;
								while	(	!frequency_match	)	{

																matched_time_stamps	=	0	;
																maxTime	=	startTime_	;
																while	(	!	matched_time_stamps	)	{

																								//	get	the	next	record	out	of	the	file	and	find	the	maximum	time	stamp
																								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{
																																if	(	!	dataStreams_[ii]->get(&lastRead_[dataStreams_[ii]].time,
																																								&lastRead_[dataStreams_[ii]].value)	)	{
																																								isEOF_	=	1	;
																																								return(0)	;
																																}
																																if	(lastRead_[dataStreams_[ii]].time	>	maxTime	)	{
																																								maxTime	=	lastRead_[dataStreams_[ii]].time	;
																																}
																								}

																								//	if	the	maximum	time	is	greater	than	the	stopTime	we	are	done	;
																								if	(	maxTime	>	stopTime_	)	{
																																	isEOF_	=	1	;
																																	return(0)	;
																								}

																								//	keep	getting	records	from	the	all	streams	until	we	past	the	largest	time	step
																								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{
																																while	(	maxTime	-
																																								lastRead_[dataStreams_[ii]].time	>	timeMatchTolerance_	)	{
																																								if	(	!	dataStreams_[ii]->get(&lastRead_[dataStreams_[ii]].time,
																																																&lastRead_[dataStreams_[ii]].value))	{
																																																isEOF_	=	1	;
																																																return(0)	;
																																								}
																																}
																								}


																								//	check	to	see	if	all	the	time	stamps	match	or	not
																								matched_time_stamps	=	1	;
																								for	(	ii	=	0	;	ii	<	dataStreams_.size()	;	ii++	)	{
																																if	(	DPLOG_ABS(lastRead_[dataStreams_[ii]].time	-	maxTime)	>	timeMatchTolerance_	)	{
																																								matched_time_stamps	=	0	;
																																								break	;
																																}
																								}

																}

																//	check	to	see	if	we	match	the	requested	frequency
																frequency_match	=	1	;
																if	(	frequency_	!=	0.0	)	{
																								while	(	nextFrequencyCheck_	<	maxTime	)	{
																																nextFrequencyCheck_	=	frequencyMultiple_	*	frequency_	;
																																frequencyMultiple_++	;
																								}
																								if	(	DPLOG_ABS(nextFrequencyCheck_	-	maxTime)	>	timeMatchTolerance_	)	{
																																frequency_match	=	0	;
																								}
																}
								}

								isEOF_	=	0	;
								return(1)	;
}

int	DataStreamGroup::getLastRead(	DataStream	*ds	,	double	*time	,	double	*value)	{

								*time	=	lastRead_[ds].time	;
								*value	=	lastRead_[ds].value	;
								return	(1)	;
}

