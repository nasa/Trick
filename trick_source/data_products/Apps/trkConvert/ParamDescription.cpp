#include	<stdexcept>
#include	<stdint.h>
#include	"ParamDescription.hh"

ParamDescription::ParamDescription(FILE*	in_fp)	{

				int32_t	nameStringLength;
				if	(	fread(	&nameStringLength,	1,	4,	in_fp)	!=	4)
								throw	std::runtime_error("fread()	failed.");

				parameterName	=	new	char[nameStringLength+1];
				if	(	fread(	parameterName,	1,	nameStringLength,	in_fp)	!=	(size_t)nameStringLength)
								throw	std::runtime_error("fread()	failed.");

				parameterName[nameStringLength]	=	0;

				int32_t	unitsStringLength;
				if	(	fread(	&unitsStringLength,	1,	4	,	in_fp)	!=	4)
								throw	std::runtime_error("fread()	failed.");

				unitsName	=	new	char[unitsStringLength+1];
				if	(	fread(	unitsName,	1,	unitsStringLength,	in_fp)	!=	(size_t)unitsStringLength)
								throw	std::runtime_error("fread()	failed.");

				unitsName[unitsStringLength]	=	0;

				if	(	fread(	&dataType,	1,	4,	in_fp)	!=	4)
								throw	std::runtime_error("fread()	failed.");

				if	(	fread(	&dataSize,	1,	4,	in_fp)	!=	4)
								throw	std::runtime_error("fread()	failed.");
}
