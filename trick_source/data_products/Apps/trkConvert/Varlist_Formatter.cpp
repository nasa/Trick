#include	"Varlist_Formatter.hh"
/*	================================================================================
	*	CLASS:	Varlist_Formatter	isa	LogFormatter.
	*	================================================================================
	*/

				void	Varlist_Formatter::writeHeader(FILE*	out_fp,	int	version,	int	endianness)	{};
				void	Varlist_Formatter::writeColumnLabel(FILE*	out_fp,	const	char*	name,	const	char*	units)	{
								fprintf(out_fp,"%s	{%s}",	name,	units);
				}
				void	Varlist_Formatter::writeColumnLabelSeparator(FILE*	out_fp)		{	fprintf(out_fp,"\n");	}
				void	Varlist_Formatter::writeDatumSeparator(FILE*	out_fp	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeRecordSeparator(FILE*	out_fp	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	int8_t			datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	uint8_t		datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	int16_t		datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	uint16_t	datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	int32_t		datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	uint32_t	datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	int64_t		datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	uint64_t	datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	float				datum	__attribute__((unused)))	{}
				void	Varlist_Formatter::writeDatum(FILE*	out_fp	__attribute__((unused)),	double			datum	__attribute__((unused)))	{}

				const	char*	Varlist_Formatter::extension()	{	return	".varlist";	};
