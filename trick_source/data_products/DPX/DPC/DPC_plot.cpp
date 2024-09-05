
#include	<string.h>
#include	<float.h>
#include	"DPC_plot.hh"

//	CONSTRUCTOR
				DPC_plot::DPC_plot(	DPM_relation	*Relation	)	{
								relation	=	Relation;
								view_data	=	NULL;
								xMin	=	-DBL_MAX;
								xMax	=	DBL_MAX;
								yMin	=	-DBL_MAX;
								yMax	=	DBL_MAX;
				}

//	DESTRUCTOR
				DPC_plot::~DPC_plot(){
								int	n_curves;
								int	i;

								n_curves	=	(int)curve_list.size();
								for	(i=0	;	i<n_curves	;	i++	)	{
												DPC_curve	*curve;
												curve	=	curve_list[i];
												if	(	curve	)	{	delete	curve;	};
								}
				}

//	MEMBER	FUNCTION
				DPV_pointer	DPC_plot::render(	DPV_view	*view,
																																		DPV_pointer	parent_view_data	)	{

								if	(view	!=	NULL)	{
												DPC_curve	*curve;
												int	i,	n_curves;

												view_data	=	view->render_plot(	parent_view_data,	this);

												//	Call	the	render	member	function	of	each	of	the	children.
												n_curves	=	(int)curve_list.size();
												for	(i=0	;	i<n_curves	;	i++	)	{
																curve	=	curve_list[i];
																curve->render(	view,	view_data);
												}
												view->finalize_plot_view(	view_data);
								}	else	{
												view_data	=	NULL;
								}
								return	(view_data);
				}

//	MEMBER	FUNCTION
				int	DPC_plot::add_curve(	DPC_curve	*curve)	{

								curve_list.push_back(	curve	);
								return	0;
				}

				/**	Get	minimum	range	for	x	for	this	plot	*/
				double	DPC_plot::getXMin()	{
												return	xMin;
				}

				/**	Set	minimum	range	for	x	for	this	plot	*/
				double	DPC_plot::setXMin(double	value)	{
												xMin	=	value;
												return	xMin;
				}

				/**	Get	maximum	range	for	x	for	this	plot	*/
				double	DPC_plot::getXMax()	{
												return	xMax;
				}

				/**	Set	maximum	range	for	x	for	this	plot	*/
				double	DPC_plot::setXMax(double	value){
												xMax	=	value;
												return	xMax;
				}

				/**	Get	minimum	range	for	y	for	this	plot	*/
				double	DPC_plot::getYMin()	{
												return	yMin;
				}

				/**	Set	minimum	range	for	y	for	this	plot	*/
				double	DPC_plot::setYMin(double	value)	{
												yMin	=	value;
												return	yMin;
				}

				/**	Get	maximum	range	for	y	for	this	plot	*/
				double	DPC_plot::getYMax()	{
												return	yMax;
				}

				/**	Set	maximum	range	for	y	for	this	plot	*/
				double	DPC_plot::setYMax(double	value)	{
												yMax	=	value;
												return	yMax;
				}
