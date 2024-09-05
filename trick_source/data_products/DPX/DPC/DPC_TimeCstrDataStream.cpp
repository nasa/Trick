
//	NOTES:
//	Both	the	current	and	next	time/value	pair	in	a	DataStream	are	kept
//	track	of	in	this	code.	eos	(end-of-stream)	is	an	int	array	representing
//	whether	or	not	the	current/next	pair	exist.
//	0	-	exists
//	1	-	does	not	exist
//
//	!bix:	index	of	the	current	time/value	pair
//	bix:	index	of	the	next	time/value	pair
//
//	This	holds	in	every	function	until	bix	values	are	switched
//			bix	=	!bix

#include	"DPC/DPC_TimeCstrDataStream.hh"

//	CONSTRUCTOR
DPC_TimeCstrDataStream::DPC_TimeCstrDataStream(DataStream*	in_ds,
																											DPM_time_constraints	*time_constraints	)	{
		this->tstart	=	time_constraints->getStart();
		this->tstop		=	time_constraints->getStop();
		this->period	=	time_constraints->getPeriod();

		this->ds	=	in_ds;
		this->bix	=	0;
		this->eos[0]	=	0;
		this->eos[1]	=	0;
		this->begin();
}

//	DESTRUCTOR
DPC_TimeCstrDataStream::~DPC_TimeCstrDataStream()	{
		if	(ds)	delete	ds;
}

//	MEMBER	FUNCTION
int	DPC_TimeCstrDataStream::get(double*	timestamp,	double*	paramValue)	{

		step();
		*timestamp	=	time[bix];
		*paramValue	=	value[bix];

		if	(eos[!bix])	return	(0);
		return	(1);
}

//	MEMBER	FUNCTION
//	Original:	return	-1	if	no	value,	return	0	if	value
//	New:			return		0	if	no	value,	return	1	if	value
int	DPC_TimeCstrDataStream::peek(double*	timestamp,	double*	paramValue)	{

		if	(eos[!bix])	return	(0);

		*timestamp	=	time[!bix];
		*paramValue	=	value[!bix];

		return(1);
}

//	MEMBER	FUNCTION
std::string	DPC_TimeCstrDataStream::getFileName()	{
		return	ds->getFileName();
}

//	MEMBER	FUNCTION
std::string	DPC_TimeCstrDataStream::getUnit()	{
		return	ds->getUnit();
}

//	MEMBER	FUNCTION
std::string	DPC_TimeCstrDataStream::getTimeUnit()	{
		return	ds->getTimeUnit();
}

//	MEMBER	FUNCTION
void	DPC_TimeCstrDataStream::begin()	{

		bix	=	0;
		eos[0]	=	0;
		eos[1]	=	0;
		ds->begin();
		step();
}

//	MEMBER	FUNCTION
int	DPC_TimeCstrDataStream::end()	{
		if	(eos[!bix])	return	(1);
		return(0);
}

//	MEMBER	FUNCTION
int	DPC_TimeCstrDataStream::step()	{

		if	(eos[!bix])	return	(0);
		do	{
				if	(!	ds->get(&time[bix],	&value[bix]))	{
						eos[bix]	=	1;
				}
				//	times	in	log	file	may	not	be	sequential
		}	while	((!eos[bix])	&&	((time[bix]	<	tstart)	||	(time[bix]	>	tstop))	);
		bix	=	!bix;
		return	(1);
}
