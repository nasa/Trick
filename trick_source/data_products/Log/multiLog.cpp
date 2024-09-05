#include	<stdio.h>
#include	<string.h>
#include	"log.h"

//	For	DBL_MIN,	DBL_MAX	defs
#include	<float.h>

/**
	*	LogMultiGroup	constructor
	*/
LogMultiGroup::LogMultiGroup(	vector<string>&	runDirs	)	{

								char*	path	;

								path	=	new	char[1024]	;

								vector	<	string	>::const_iterator	run;
								for	(run	=	runDirs.begin();	run	!=	runDirs.end()	;	run++)	{
																strcpy(path,	(*run).c_str());
																LogGroup*	lg	=	new	LogGroup(path)	;
																logGroups_.push_back(lg);
								}

								delete[]	path	;
}	;

/**
	*	LogMultiGroup	destructor
	*/
LogMultiGroup::~LogMultiGroup(	)
{

								vector	<	LogGroup*	>::const_iterator	lg;
								for	(lg	=	logGroups_.begin();	lg	!=	logGroups_.end()	;	lg++)	{
																delete	*lg	;
								}

}	;



/**
	*	Iterate	over	log	groups	constructor
	*/
IterOverLogGroups::IterOverLogGroups(	LogMultiGroup*	lmg,
																																						vector<string>&	runs,
																																						vector<string>&	paramNames	)
{
								int	found	;
								string	runDirName	;
								int	tIdx,	tLogIdx	;
								int	vIdx,	vLogIdx	;

								lmg_	=	lmg	;

								//	Mapping	from	log	group	to	an	array	of	log	indices
								map<	LogGroup*,	set<	int	>	>	lg2log	;

								vector	<	LogGroup*	>::const_iterator	lg;
								vector	<	string	>::const_iterator	ss;
								set	<	int	>::const_iterator	ii	;

								//	Make	list	of	log	groups	to	iterate	over
								//	Go	ahead	and	convert	to	doubles	as	well
								int	idx	=	0	;
								for	(lg	=	lmg->logGroups_.begin();	lg	!=	lmg->logGroups_.end();	lg++)	{

																(*lg)->convertToDoubleYes();

																runDirName	=	(*lg)->getDataDir()	;
																for	(ss	=	runs.begin();	ss	!=	runs.end();	ss++	)	{
																								if	(	runDirName	==	*ss	)	{
																																iterLogGroups_.push_back(*lg);
																																runDirName2lg_[*ss]	=	*lg	;
																																runDirName2runIdx_[*ss]	=	idx	;
																																break	;
																								}
																}

																idx++	;
								}

								//	Foreach	log	group,	we	need	a	log	and	param	idx	list	per	param
								for	(lg	=	iterLogGroups_.begin();	lg	!=	iterLogGroups_.end();	lg++)	{

																set<	int	>	logIndices	;

																for	(ss	=	paramNames.begin();	ss	!=	paramNames.end();	ss++	)	{

																								(*lg)->getParamIdx(	"sys.exec.out.time",
																																												&tIdx,	&tLogIdx,
																																												(*ss).c_str(),
																																												&vIdx,	&vLogIdx	)	;

																								ParamIdx*	pi	=	new	ParamIdx	;
																								pi->logIdx	=	vLogIdx	;
																								pi->varIdx	=	vIdx	;
																								paramMap_[*ss]	=	pi;

																								//	Keep	list	of	log	data	sets	with	this	group
																								//	Weed	out	duplicates
																								found	=	0	;
																								for	(ii	=	logIndices.begin();
																																	ii	!=	logIndices.end();	ii++	)	{

																																if	(vLogIdx	==	*ii)	{
																																								found	=	1	;
																																								break	;
																																}
																								}
																								if	(!found)	{
																																logIndices.insert(vLogIdx);
																								}

																}

																//	Map	this	log	group	to	its	log	data	sets	indices
																lg2log[(*lg)]	=	logIndices	;

																runs_.push_back(paramMap_)	;
								}

								//	Make	list	of	iterators	for	iterating	over	each	log	group
								LogGroup*	key	;
								vector	<	int	>	val	;
								map	<	LogGroup*,	set<	int	>	>::const_iterator	ll;
								listLogGroupIterators_.reserve(listLogGroupIterators_.size()
																																							+	lg2log.size());
								for	(ll	=	lg2log.begin();	ll	!=	lg2log.end();	ll++)	{

																key	=	(*ll).first	;

																IterPreserveTime*	iter	=	new	IterPreserveTime(
																																																key,
																																																lg2log[key],
																																																0.0,	0.0,	DBL_MAX	)	;

																listLogGroupIterators_.push_back(iter);
								}
}

IterOverLogGroups::~IterOverLogGroups()
{
}

//	For	each	log	group:	set	log	group	to	begin()
void	IterOverLogGroups::begin()
{

								vector<	LogGroup*	>::const_iterator	lg	;

								for	(lg	=	iterLogGroups_.begin();	lg	!=	iterLogGroups_.end();	lg++)	{

																(*lg)->begin();
								}
}

//	Finished	iterating	on	all	log	groups	in	multigroup?
bool	IterOverLogGroups::end()
{
								int	end	=	1	;
								vector<	LogGroup*	>::const_iterator	lg	;

								for	(lg	=	iterLogGroups_.begin();	lg	!=	iterLogGroups_.end();	lg++)	{
																if	(	!	(*lg)->end()	)	{
																								end	=	0	;
																								break	;
																}
								}

								return(	end	)	;
}

//	Increment	list	of	iterators	which	operate	on	each	log	group
void	IterOverLogGroups::operator++(int)
{

								static	vector	<	IterPreserveTime*	>::const_iterator	ili	;

								for	(ili	=	listLogGroupIterators_.begin();
																	ili	!=	listLogGroupIterators_.end();	ili++)	{

																//	*ili	is	an	element	of	list	of	iterators...
																//	*(*ili)	is	the	iterator	to	iterate		---	Confusing!
																(*(*ili))++	;
								}
}

double	IterOverLogGroups::getVal(string	runName,	string	paramName)
{
								static	LogGroup*	lg	;
								static	int	logIdx	;
								static	int	varIdx	;
								static	int	runIdx	;


								lg					=	runDirName2lg_[runName]	;
								runIdx	=	runDirName2runIdx_[runName]	;
								logIdx	=	runs_[runIdx][paramName]->logIdx	;
								varIdx	=	runs_[runIdx][paramName]->varIdx	;

								return(	*((double	*)	lg->log[logIdx]->getVal(varIdx))	)	;
}
