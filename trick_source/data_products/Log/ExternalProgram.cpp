#include	<math.h>
#include	<stdlib.h>

#include	"log.h"
#include	"ExternalProgram.hh"

ExternalProgram::ExternalProgram(	const	char*	sharedLibName,
																																			int	nInputStreams,	DataStream**	istreams,
																																			int	nOutputs,	int	outputIdx	)
{
								char	msg[80];
								int	ii	;

								char	next_record_str[]	=	"extGetNextRecord"	;
								unitStr_	=	"1"	;
								unitTimeStr_	=	"s"	;

								//	Open	up	external	plugin	program
								progHandle_	=	dlopen(sharedLibName,	RTLD_LAZY);
								if	(!progHandle_)	{
																snprintf(msg,	sizeof(msg),	"ERROR:	Couldn't	load	shared	program	\"%s\"	\n",
																								sharedLibName);
																fputs(msg,	stderr);
																exit(1);
								}
								cout	<<	"Loading	shared	library:	"	<<	sharedLibName	<<	"\n"	;
								external_program	=
												(int	(*)(double	*,	int,	double	*,	int))	dlsym(progHandle_,
																																																										next_record_str);
								if	((error	=	dlerror())	!=	NULL)	{
																fprintf(stderr,	"%s\n",	error);
																exit(1);
								}

								//	Load	input	streams
								for	(	ii	=	0	;	ii	<	nInputStreams	;	ii++	)	{
																dsg_.add(istreams[ii]);
																istreams_.push_back(istreams[ii])	;
								}
								delete[]	istreams	;

								//	Set	up	i/o	to	shared	lib	program
								nInputs_	=	nInputStreams;
								input_	=	new	double[nInputStreams];
								nOutputs_	=	nOutputs;
								output_	=	new	double[nOutputs];

								//	Since	this	is	a	"stream",	only	one	output	value
								//	will	be	returned	(see	get()).		The	output	index	passed
								//	in	indicates	which	output	to	pass	back.
								if	(	nOutputs_	<=	outputIdx	)	{
																cerr	<<	"ERROR:	External	program	output	index	exceeds	total	"
																					<<	"number	of	external	program	outputs.\n"	;
																exit(-1);
								}
								outputIdx_	=	outputIdx	;
}

ExternalProgram::~ExternalProgram()
{
								unsigned	int	ii	;

								delete[]	input_	;
								delete[]	output_	;

								for	(	ii	=	0	;	ii	<	istreams_.size()	;	ii++	)	{
																delete	istreams_[ii]	;
								}
}

int	ExternalProgram::step()
{
								dsg_.step();
								return(0)	;
}

/**
	*	Get	parameter	and	time	stamp	values,	don't	advance	(step)
	*/
int	ExternalProgram::peek(double*	timeStamp,	double*	paramValue)
{
								int	ret	=	getOutputValue(	timeStamp,	paramValue	)	;

								return(ret)	;
}

/**
	*	Advance	(step)	through	stream
	*	And	get	parameter	and	time	stamp	values,
	*/
int	ExternalProgram::get(double*	timeStamp,	double*	outputValue)
{
								int	ret	;
								ret	=	dsg_.matchTimeStamps();
								if	(	!	ret	)	{
																//	At	end	of	input	streams
																return	ret	;
								}

								ret	=	getOutputValue(	timeStamp,	outputValue	)	;

								return(ret)	;
}

int	ExternalProgram::getOutputValue(	double*	timeStamp,	double*	outputValue	)
{
								int	ii	;
								int	ret	;

								for	(	ii	=	0	;	ii	<	nInputs_	;	ii++	)	{
																dsg_.getLastRead(istreams_[ii],	timeStamp,	&input_[ii]	)	;
								}

								ret	=	external_program(input_,	nInputs_,	output_,	nOutputs_);
								if	(ret	<	0)	{
																fprintf(stderr,	"ERROR:	External	program	\"%s\"	"
																								"	aborted	with	an	error	status	of	%d	\n",
																								sharedLibName_,	ret);
																return(-1);
								}

								*outputValue	=	output_[outputIdx_]	;

								return(ret);
}

/**
	*	Advance	through	data
	*	Get	parameter	and	time	stamp	values
	*/
void	ExternalProgram::begin()
{
								dsg_.begin();
}

/**
	*	Seek	in	data	until	time	stamp.
	*/
int	ExternalProgram::getValueAtTime(double	time,	double*	pVal	)
{
								//	TODO
								return(-1);
}

/**
	*	Are	we	at	end	of	data	stream?
	*/
int	ExternalProgram::end()
{
								return	(dsg_.end())	;
}
