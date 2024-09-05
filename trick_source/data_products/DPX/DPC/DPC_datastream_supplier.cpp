#include	"DPC/DPC_datastream_supplier.hh"
#include	"DPC/DPC_TimeCstrDataStream.hh"
#include	"DPC/DPC_UnitConvDataStream.hh"
#include	<iostream>

//	CONSTRUCTOR
DPC_datastream_supplier::DPC_datastream_supplier(	DPM_product	*Product	)	{

				product	=	Product;
				data_stream_factory	=	new	DataStreamFactory();
}

//	DESTRUCTOR
DPC_datastream_supplier::~DPC_datastream_supplier()	{

				delete	data_stream_factory;
}

//	MEMBER	FUNCTION
DataStream	*DPC_datastream_supplier::getDataStream(	const	char	*VarName,
										const	char	*ToUnits,
										const	char	*FromUnitsHint,
										DPM_run				*Run,
										DPM_time_constraints*	in_time_constraints	)	{

				DataStream**	instreams;
				int	n_extfns,	extfn_index;

				DataStream	*ds	=	NULL;
				DataStream	*tds	=	NULL;
				DataStream	*utds	=	NULL;

				const	char	*run_dir		=	Run->getDir()	;
				const	char	*timename	=	Run->getTimeName()	;

				n_extfns	=	this->product->NumberOfExtFns();

				for	(extfn_index	=	0	;	extfn_index	<	n_extfns	;	extfn_index++	)	{
								DPM_extfn	*extfn;
								int	output_index;

								extfn	=	this->product->getExtFn(	extfn_index);

								output_index	=	extfn->index_of(	VarName	);

								if	(output_index	>=	0	)		{
												int	n_inputs,	n_outputs;
												int	input_ix;

												n_inputs		=	extfn->NumberOfInputs();
												n_outputs	=	extfn->NumberOfOutputs();

												instreams	=	(DataStream**)calloc(	sizeof(	DataStream*),	(size_t)n_inputs	);

												for	(input_ix	=0	;	input_ix	<	n_inputs	;	input_ix++)	{
																instreams[input_ix]	=
																				data_stream_factory->create(	run_dir,
																																																	extfn->getInputVar(	input_ix),
																																																	timename	);
												}

												ds	=	data_stream_factory->create(	extfn->getFName(),	n_inputs,	instreams,	n_outputs,	output_index);
								}
				}

				if	(ds	==	NULL)	{
								ds	=	data_stream_factory->create(	run_dir,	VarName,	timename);
				}

				if	(ds	==	NULL)	{
								return	NULL;
				}

				if	(	in_time_constraints	==	NULL	)		{
								tds	=	ds;
				}	else	{
								tds	=	new	DPC_TimeCstrDataStream(	ds,	in_time_constraints	);
				}

				utds	=	new	DPC_UnitConvDataStream(	tds,	ToUnits,	FromUnitsHint	);

				return	utds;
}

DataStream	*DPC_datastream_supplier::getDataStream(	const	char	*VarName,
										const	char	*ToUnits,
										const	char	*FromUnitsHint,
										const	char	*Machine,
										const	unsigned	short	Port,
										DPM_time_constraints*	in_time_constraints	)	{

				DataStream	*ds	=	NULL;
				DataStream	*tds	=	NULL;
				DataStream	*utds	=	NULL;

				ds	=	data_stream_factory->create(	Machine,	Port	,	VarName);

				if	(ds	==	NULL)	{
								return	NULL;
				}

				if	(	in_time_constraints	==	NULL	)		{
								tds	=	ds;
				}	else	{
								tds	=	new	DPC_TimeCstrDataStream(	ds,	in_time_constraints	);
				}

				utds	=	new	DPC_UnitConvDataStream(	tds,	ToUnits,	FromUnitsHint);

				return	utds;
}
