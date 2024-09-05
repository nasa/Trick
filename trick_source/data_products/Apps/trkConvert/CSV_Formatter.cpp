#include	"CSV_Formatter.hh"
/*	================================================================================
	*	CLASS:	CSV_Formatter	isa	LogFormatter.
	*	================================================================================
	*/

void	CSV_Formatter::writeHeader(FILE*	out_fp,	int	version,	int	endianness)	{};
void	CSV_Formatter::writeColumnLabel(FILE*	out_fp,	const	char*	name,	const	char*	units)	{
				fprintf(out_fp,"%s	{%s}",	name,	units);
}
void	CSV_Formatter::writeColumnLabelSeparator(FILE*	out_fp)		{	fprintf(out_fp,",");	}
void	CSV_Formatter::writeDatumSeparator(FILE*	out_fp)								{	fprintf(out_fp,",");	}
void	CSV_Formatter::writeRecordSeparator(FILE*	out_fp)							{	fprintf(out_fp,"\n");	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	int8_t	datum)			{	fprintf(out_fp,"%d",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	uint8_t	datum)		{	fprintf(out_fp,"%u",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	int16_t	datum)		{	fprintf(out_fp,"%d",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	uint16_t	datum)	{	fprintf(out_fp,"%u",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	int32_t	datum)		{	fprintf(out_fp,"%d",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	uint32_t	datum)	{	fprintf(out_fp,"%u",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	int64_t	datum)		{	fprintf(out_fp,"%ld",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	uint64_t	datum)	{	fprintf(out_fp,"%lu",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	float	datum)				{	fprintf(out_fp,"%.7f",	datum);	}
void	CSV_Formatter::writeDatum(FILE*	out_fp,	double	datum)			{	fprintf(out_fp,"%.15g",	datum);	}
const	char*	CSV_Formatter::extension()	{	return	".csv";	};
