
#include	<stdlib.h>
#include	<math.h>
#include	<stdexcept>
#include	<iostream>		//	std::cout,	std::cerr
#include	<algorithm>	//	std::copy
#include	"SAIntegrator.hh"

static	void	nil_dfunc(	double	x,
																							double	state[]	__attribute__((unused)),
																							double	derivs[],
																							void*		udata	__attribute__((unused)))	{
				derivs[0]	=	0.0;
}
static	void	nil_gfunc(	double	t,
																							double	x[]	__attribute__((unused)),
																							double	v[]	__attribute__((unused)),
																							double	derivs[],
																							void*		udata	__attribute__((unused)))	{
				derivs[0]	=	0.0;
}
//	------------------------------------------------------------
//	Class	Integrator
//	------------------------------------------------------------
//	Default	Constructor
SA::Integrator::Integrator()
:	X_in(0.0),	X_out(0.0),	default_h(1.0),	user_data(NULL)	{};
//	Constructor
SA::Integrator::Integrator(double	h,	void*	udata)
:	X_in(0.0),	X_out(0.0),	default_h(h),	user_data(udata)	{};

void	SA::Integrator::advanceIndyVar()	{
				X_out	=	X_in	+	default_h;
}
void	SA::Integrator::step()	{
				advanceIndyVar();
}
void	SA::Integrator::load()	{
				X_in	=	X_out;
}
void	SA::Integrator::unload()	{
				std::cout	<<	"SA::Integrator	:	Nothing	to	unload."	<<	std::endl;
}
void	SA::Integrator::integrate()	{
				load();
				step();
				unload();
}
double	SA::Integrator::undo_integrate()	{
				X_out	=	X_in;
				return	(default_h);
}
double	SA::Integrator::getIndyVar()	{
				return	X_out;
}
void	SA::Integrator::setIndyVar(double	v)	{
				X_out	=	v;
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	Integrator&	I)	{
				os	<<	"\n---	Integrator	---";
				os	<<	"\nX_in						:	"	<<	I.X_in;
				os	<<	"\nX_out					:	"	<<	I.X_out;
				os	<<	"\ndefault_h	:	"	<<	I.default_h;
				os	<<	"\nuser_data	:	"	<<	I.user_data;
				return	os;
}

//	------------------------------------------------------------
//	Class	FirstOrderODEIntegrator
//	------------------------------------------------------------
//	Default	Constructor
SA::FirstOrderODEIntegrator::FirstOrderODEIntegrator()
:	Integrator()	{
				state_size	=	0;
				inVars	=	NULL;
				outVars	=	NULL;
				inState	=	new	double[state_size];
				outState	=	new	double[state_size];
				derivs_func	=	nil_dfunc;
}
//	Constructor
SA::FirstOrderODEIntegrator::FirstOrderODEIntegrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	Integrator(h,	udata)	{
				state_size	=	N;
				inVars	=	in_vars;
				outVars	=	out_vars;
				inState	=	new	double[state_size];
				outState	=	new	double[state_size];
				if	(dfunc	==	NULL)	throw	std::invalid_argument("dfunc	must	be	non-NULL.");
				derivs_func	=	dfunc;
}
//	Copy	Constructor
SA::FirstOrderODEIntegrator::FirstOrderODEIntegrator(const	FirstOrderODEIntegrator&	other)
:	Integrator(other)	{
				state_size	=	other.state_size;
				inVars	=	other.inVars;
				outVars	=	other.outVars;
				inState	=	new	double[state_size];
				std::copy(	other.inState,	other.inState	+	state_size,	inState);
				outState	=	new	double[state_size];
				std::copy(	other.outState,	other.outState	+	state_size,	outState);
				derivs_func	=	other.derivs_func;
}
//	Assignment	Operator
SA::FirstOrderODEIntegrator&	SA::FirstOrderODEIntegrator::operator=(	const	SA::FirstOrderODEIntegrator&	rhs)	{
				if	(this	!=	&rhs)	{
								//	Call	base	assignment	operator
								Integrator::operator=(rhs);
								//	Duplicate	rhs	arrays
								double*	new_inState	=	new	double[rhs.state_size];
								std::copy(	rhs.inState,	rhs.inState	+	rhs.state_size,	new_inState);
								double*	new_outState	=	new	double[rhs.state_size];
								std::copy(	rhs.outState,	rhs.outState	+	rhs.state_size,	new_outState);
								//	Delete	lhs	arrays	&	replace	with	rhs	arrays
								if	(inState	!=	NULL)	delete	inState;
								inState	=	new_inState;
								if	(outState	!=	NULL)	delete	outState;
								outState	=	new_outState;
								//	Copy	primitive	members
								state_size	=	rhs.state_size;
								inVars	=	rhs.inVars;
								outVars	=	rhs.outVars;
								derivs_func	=	rhs.derivs_func;
				}
				return	*this;
}
//	Destructor
SA::FirstOrderODEIntegrator::~FirstOrderODEIntegrator()	{
				if	(inState	!=	NULL)	delete	inState;
				if	(outState	!=	NULL)	delete	outState;
}
void	SA::FirstOrderODEIntegrator::load()	{
				if	(inVars	!=	NULL)	{
								for	(unsigned	int	i=0	;	i<state_size;	i++	)	{
												inState[i]	=	*(inVars[i]);
								}
								Integrator::load();
				}	else	{
								std::cerr	<<	"Error:	SA::FirstOrderODEIntegrator::load().	inVars	is	not	set."	<<	std::endl;
				}
}
void	SA::FirstOrderODEIntegrator::unload()	{
				if	(outVars	!=	NULL)	{
								for	(unsigned	int	i=0	;	i<state_size;	i++	)	{
												*(outVars[i])	=	outState[i];
								}
				}	else	{
								std::cerr	<<	"Error:	SA::FirstOrderODEIntegrator::load().	outVars	is	not	set."	<<	std::endl;
				}
}
void	SA::FirstOrderODEIntegrator::load_from_outState(){
				for	(unsigned	int	i=0	;	i<state_size;	i++	)	{
								inState[i]	=	outState[i];
				}
				Integrator::load();
}
double**	SA::FirstOrderODEIntegrator::set_in_vars(	double*	in_vars[]){
				double	**ret	=	inVars;
				inVars	=	in_vars;
				return	(ret);
}
double**	SA::FirstOrderODEIntegrator::set_out_vars(	double*	out_vars[]){
				double	**ret	=	outVars;
				outVars	=	out_vars;
				return	(ret);
}
double	SA::FirstOrderODEIntegrator::undo_integrate()	{
				if	(inVars	!=	NULL)	{
								for	(unsigned	int	i=0	;	i<state_size;	i++	)	{
												*(inVars[i])	=	inState[i];
								}
								std::copy(inState,	inState	+	state_size,	outState);
								return	(SA::Integrator::undo_integrate());
				}	else	{
								std::cerr	<<	"Error:	SA::FirstOrderODEIntegrator::load().	inVars	is	not	set."	<<	std::endl;
				}
				return	(0.0);
}
void	SA::FirstOrderODEIntegrator::step()	{
				double	derivs[state_size];
				(*derivs_func)(	X_in,	inState,	derivs,	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	derivs[i]	*	default_h;
				}
				advanceIndyVar();
}

static	std::ostream&	stream_double_array(std::ostream&	os,	unsigned	int	n,	double*	d_array)	{
				if	(d_array	==	NULL)	{
								os	<<	"	NULL\n";
				}	else	{
								os	<<	"[";
								for	(int	i=0;	i<n	;	i++)	{
												if	(i==0)	{	os	<<	d_array[i];	}
												else	{	os	<<	"	,"	<<	d_array[i];	}
								}
								os	<<	"]";
				}
				return	os;
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	FirstOrderODEIntegrator&	I)	{
								os	<<	(SA::Integrator)I;
								os	<<	"\n---	FirstOrderODEIntegrator	---";
								os	<<	"\nstate_size:	"	<<	I.state_size;
								os	<<	"\ninState			:";
								stream_double_array(os,	I.state_size,	I.inState);
								os	<<	"\noutState		:";
								stream_double_array(os,	I.state_size,	I.outState);
								os	<<	"\ninVars				:	"	<<	(void*)I.inVars;
								os	<<	"\noutVars			:	"	<<	(void*)I.outVars;
								return	os;
}

//	------------------------------------------------------------
//	Class	FirstOrderODEVariableStepIntegrator
//	------------------------------------------------------------
//	Default	Constructor
SA::FirstOrderODEVariableStepIntegrator::FirstOrderODEVariableStepIntegrator()
:	FirstOrderODEIntegrator()	{
				root_finder	=	(RootFinder*)	NULL;
				root_error_func	=	(RootErrorFunc)	NULL;
}
//	Constructor
SA::FirstOrderODEVariableStepIntegrator::FirstOrderODEVariableStepIntegrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	FirstOrderODEIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{
				root_finder	=	(RootFinder*)	NULL;
				root_error_func	=	(RootErrorFunc)	NULL;
}

//	Copy	Constructor
SA::FirstOrderODEVariableStepIntegrator::FirstOrderODEVariableStepIntegrator(
const	FirstOrderODEVariableStepIntegrator&	other)
:	FirstOrderODEIntegrator(other)	{
				if	(other.root_finder	!=	NULL)	{
								root_finder	=	new	RootFinder(*(other.root_finder));
				}	else	{
								root_finder	=	NULL;
				}
				root_error_func	=	other.root_error_func;
}
//	Assignment	Operator
SA::FirstOrderODEVariableStepIntegrator&	SA::
FirstOrderODEVariableStepIntegrator::operator=(	const	SA::FirstOrderODEVariableStepIntegrator&	rhs)	{
				FirstOrderODEIntegrator::operator=(rhs);
				if	(root_finder	!=	NULL)	delete	root_finder;
				if	(rhs.root_finder	!=	NULL)	{
								root_finder	=	new	RootFinder(*(rhs.root_finder));
				}	else	{
								root_finder	=	NULL;
				}
				root_error_func	=	rhs.root_error_func;
				return	*this;
}
//	Destructor
SA::FirstOrderODEVariableStepIntegrator::~FirstOrderODEVariableStepIntegrator()	{
				if	(root_finder	!=	NULL)	{
								delete	root_finder;
				}
}
double	SA::FirstOrderODEVariableStepIntegrator::undo_integrate()	{
				SA::FirstOrderODEIntegrator::undo_integrate();
				return	(last_h);
}
void	SA::FirstOrderODEVariableStepIntegrator::add_Rootfinder(	double	tolerance,	SlopeConstraint	constraint,	RootErrorFunc	rfunc)	{
				root_finder	=	new	RootFinder(tolerance,	constraint);
				if	(rfunc	==	NULL)	{
								throw	std::invalid_argument("OOPS!	RootErrorFunc	function-pointer	is	NULL.");
				}
				root_error_func	=	rfunc;
}
void	SA::FirstOrderODEVariableStepIntegrator::advanceIndyVar(double	h)	{
				last_h	=	h;	X_out	=	X_in	+	h;
}
void	SA::FirstOrderODEVariableStepIntegrator::variable_step(	double	h)	{
				double	derivs[state_size];
				(*derivs_func)(	X_in,	inState,	derivs,	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	derivs[i]	*	h;
				}
				advanceIndyVar(h);
}
void	SA::FirstOrderODEVariableStepIntegrator::find_roots(double	h,	unsigned	int	depth)	{
				double	new_h;
				double	h_correction	=	(*root_error_func)(	getIndyVar(),	outState,	root_finder,	user_data	);
				if	(h_correction	<	0.0)	{
								while	(h_correction	!=	0.0)	{
												new_h	=	undo_integrate()	+	h_correction;
												variable_step(new_h);
												h_correction	=	(*root_error_func)(	getIndyVar(),	outState,	root_finder,	user_data	);
								}
								load_from_outState();
								variable_step(h-new_h);
								if	(depth	>	0)	{
												find_roots(h-new_h,	depth-1);
								}
				}
}
void	SA::FirstOrderODEVariableStepIntegrator::step()	{
				variable_step(	default_h	);
				if	((root_finder	!=	NULL)	&&	(root_error_func	!=	NULL))	{
								find_roots(	default_h,	5	);
				}
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	FirstOrderODEVariableStepIntegrator&	I)	{
				os	<<	(SA::FirstOrderODEIntegrator)I;
				os	<<	"\n---	FirstOrderODEVariableStepIntegrator	---";
				if	(I.root_error_func	==	NULL)	{
								os	<<	"\nroot_error_func:	NULL.";
				}	else	{
								os	<<	"\nroot_error_func:	"	<<	(void*)I.root_error_func;
				}
				if	(I.root_finder	==	NULL)	{
								os	<<	"\nroot_finder:	NULL.";
				}	else	{
								os	<<	"\nroot_finder	->	"	<<	*(I.root_finder);
				}
				return	os;
}

//	------------------------------------------------------------
//	Class	EulerIntegrator
//	------------------------------------------------------------
//	Default	Constructor
SA::EulerIntegrator::EulerIntegrator()
:	FirstOrderODEVariableStepIntegrator()	{}
//	Constructor
SA::EulerIntegrator::EulerIntegrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEVariableStepIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{}
//	Constructor
SA::EulerIntegrator::EulerIntegrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	EulerIntegrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::EulerIntegrator::EulerIntegrator(const	EulerIntegrator&	other)
				:	FirstOrderODEVariableStepIntegrator(other)	{}
//	Assignment	Operator
SA::EulerIntegrator&	SA::EulerIntegrator::operator=(	const	SA::EulerIntegrator&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEVariableStepIntegrator::operator=(rhs);
				}
				return	*this;
}
//Destructor
SA::EulerIntegrator::~EulerIntegrator()	{}

void	SA::EulerIntegrator::variable_step(	double	h)	{
				double	derivs[state_size];
				(*derivs_func)(	X_in,	inState,	derivs,	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	derivs[i]	*	h;
				}
				advanceIndyVar(h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	EulerIntegrator&	I)	{
				os	<<	(SA::FirstOrderODEVariableStepIntegrator)I;
				return	os;
}
//	------------------------------------------------------------
//	Class	HeunsMethod
//	------------------------------------------------------------
//	Default	Constructor
SA::HeunsMethod::HeunsMethod()
:	FirstOrderODEVariableStepIntegrator()	{}
//	Constructor
SA::HeunsMethod::HeunsMethod(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEVariableStepIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{}
//	Constructor
SA::HeunsMethod::HeunsMethod(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	HeunsMethod(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::HeunsMethod::HeunsMethod(const	HeunsMethod&	other)
				:	FirstOrderODEVariableStepIntegrator(other)	{}
//	Assignment	Operator
SA::HeunsMethod&	SA::HeunsMethod::operator=(	const	SA::HeunsMethod&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEVariableStepIntegrator::operator=(rhs);
				}
				return	*this;
}
//	Destructor
SA::HeunsMethod::~HeunsMethod()	{}
void	SA::HeunsMethod::variable_step(	double	h)	{
				double	wstate[state_size];
				double	derivs[2][state_size];

				(*derivs_func)(	X_in,	inState,	derivs[0],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[i]	=	inState[i]	+	h	*	derivs[0][i];
				}
				(*derivs_func)(	X_in,	wstate,	derivs[1],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	(h/2)	*	(	derivs[0][i]	+		derivs[1][i]	);
				}
				advanceIndyVar(h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	HeunsMethod&	I)	{
				os	<<	(SA::FirstOrderODEVariableStepIntegrator)I;
				return	os;
}
//	------------------------------------------------------------
//	Class	RK2Integrator
//	------------------------------------------------------------
//	Default	Constructor
SA::RK2Integrator::RK2Integrator()
				:	FirstOrderODEVariableStepIntegrator()	{}
//	Constructor
SA::RK2Integrator::RK2Integrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEVariableStepIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{}
SA::RK2Integrator::RK2Integrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	RK2Integrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::RK2Integrator::RK2Integrator(const	RK2Integrator&	other)
				:	FirstOrderODEVariableStepIntegrator(other)	{}
//	Assignment	Operator
SA::RK2Integrator&	SA::RK2Integrator::operator=(	const	SA::RK2Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEVariableStepIntegrator::operator=(rhs);
				}
				return	*this;
}
//	Destructor
SA::RK2Integrator::~RK2Integrator()	{}

void	SA::RK2Integrator::variable_step(	double	h)	{
				double	wstate[state_size];
				double	derivs[2][state_size];
				(*derivs_func)(	X_in,	inState,	derivs[0],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[i]	=	inState[i]	+	0.5	*	h	*	derivs[0][i];
				}
				(*derivs_func)(	X_in	+	0.5	*	h	,	wstate,	derivs[1],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	h	*	derivs[1][i];
				}
				advanceIndyVar(h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	RK2Integrator&	I)	{
				os	<<	(SA::FirstOrderODEVariableStepIntegrator)I;
				return	os;
}
//	------------------------------------------------------------
//	Class	RK4Integrator
//	------------------------------------------------------------
//	Default	Constructor
SA::RK4Integrator::RK4Integrator()
				:	FirstOrderODEVariableStepIntegrator()	{}
//	Constructor
SA::RK4Integrator::RK4Integrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEVariableStepIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{}
//	Constructor
SA::RK4Integrator::RK4Integrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	RK4Integrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::RK4Integrator::RK4Integrator(const	RK4Integrator&	other)
				:	FirstOrderODEVariableStepIntegrator(other)	{}
//	Assignment	Operator
SA::RK4Integrator&	SA::RK4Integrator::operator=(	const	SA::RK4Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEVariableStepIntegrator::operator=(rhs);
				}
				return	*this;
}
//	Destructor
SA::RK4Integrator::~RK4Integrator()	{}

void	SA::RK4Integrator::variable_step(	double	h)	{
				double	wstate[3][state_size];
				double	derivs[4][state_size];

				(*derivs_func)(	X_in,	inState,	derivs[0],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[0][i]	=	inState[i]	+	0.5	*	derivs[0][i]	*	h;
				}
				(*derivs_func)(	X_in	+	0.5	*	h	,	wstate[0],	derivs[1],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[1][i]	=	inState[i]	+	0.5	*	derivs[1][i]	*	h;
				}
				(*derivs_func)(	X_in	+	0.5	*	h	,	wstate[1],	derivs[2],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[2][i]	=	inState[i]	+	derivs[2][i]	*	h;
				}
				(*derivs_func)(	X_in	+	h	,	wstate[2],	derivs[3],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	((1/6.0)*	derivs[0][i]	+
																																	(1/3.0)*	derivs[1][i]	+
																																	(1/3.0)*	derivs[2][i]	+
																																	(1/6.0)*	derivs[3][i])	*	h;
				}
				advanceIndyVar(h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	RK4Integrator&	I)	{
				os	<<	(SA::FirstOrderODEVariableStepIntegrator)I;
				return	os;
}
//	------------------------------------------------------------
//	Class	RK3_8Integrator
//	------------------------------------------------------------
//	Default	Constructor
SA::RK3_8Integrator::RK3_8Integrator()
				:	FirstOrderODEVariableStepIntegrator()	{}
//	Constructor
SA::RK3_8Integrator::RK3_8Integrator(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEVariableStepIntegrator(h,	N,	in_vars,	out_vars	,dfunc,	udata)	{}
//	Constructor
SA::RK3_8Integrator::RK3_8Integrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	RK3_8Integrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::RK3_8Integrator::RK3_8Integrator(const	RK3_8Integrator&	other)
				:	FirstOrderODEVariableStepIntegrator(other)	{}
//	Assignment	Operator
SA::RK3_8Integrator&	SA::RK3_8Integrator::operator=(	const	SA::RK3_8Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEVariableStepIntegrator::operator=(rhs);
				}
				return	*this;
}
//	Destructor
SA::RK3_8Integrator::~RK3_8Integrator()	{}

void	SA::RK3_8Integrator::variable_step(	double	h)	{
				double	wstate[3][state_size];
				double	derivs[4][state_size];

				(*derivs_func)(	X_in,	inState,	derivs[0],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[0][i]	=	inState[i]	+	h	*	(1/3.0)	*	derivs[0][i];
				}
				(*derivs_func)(	X_in	+	(1/3.0)	*	h	,	wstate[0],	derivs[1],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[1][i]	=	inState[i]	+	h	*	((-1/3.0)	*	derivs[0][i]	+	derivs[1][i]);
				}
				(*derivs_func)(	X_in	+	(2/3.0)	*	h	,	wstate[1],	derivs[2],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								wstate[2][i]	=	inState[i]	+	h	*	(derivs[0][i]	-	derivs[1][i]	+	derivs[2][i]);
				}
				(*derivs_func)(	X_in	+	h	,	wstate[2],	derivs[3],	user_data);
				for	(unsigned	int	i=0;	i<state_size;	i++)	{
								outState[i]	=	inState[i]	+	((1/8.0)*	derivs[0][i]	+
																																	(3/8.0)*	derivs[1][i]	+
																																	(3/8.0)*	derivs[2][i]	+
																																	(1/8.0)*	derivs[3][i])	*	h;
				}
				advanceIndyVar(h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	RK3_8Integrator&	I)	{
				os	<<	(SA::FirstOrderODEVariableStepIntegrator)I;
				return	os;
}

//	------------------------------------------------------------
//	Class	RKF45Integrator
//	------------------------------------------------------------
//	Default	Constructor
SA::RKF45Integrator::RKF45Integrator()	:	FirstOrderODEIntegrator()	{
				epsilon	=	0.00000000001;
				next_h	=	default_h;
}
//	Constructor
SA::RKF45Integrator::RKF45Integrator(double	eps,
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{
								epsilon	=	eps;
								next_h	=	h;
				}
//	Constructor
SA::RKF45Integrator::RKF45Integrator(double	eps,
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	RKF45Integrator(eps,	h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}
//	Copy	Constructor
SA::RKF45Integrator::RKF45Integrator(const	RKF45Integrator&	other)
				:	FirstOrderODEIntegrator(other)	{
								epsilon	=	other.epsilon;
								next_h	=	other.next_h;
				}
//	Assignment	Operator
SA::RKF45Integrator&	SA::RKF45Integrator::operator=(	const	SA::RKF45Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								FirstOrderODEIntegrator::operator=(rhs);
				}
				epsilon	=	rhs.epsilon;
				next_h		=	rhs.next_h;
				return	*this;
}
//	Destructor
SA::RKF45Integrator::~RKF45Integrator()	{}

void	SA::RKF45Integrator::advanceIndyVar(double	h)	{
				last_h	=	h;	X_out	=	X_in	+	h;
}
void	SA::RKF45Integrator::step()	{
				adaptive_step(	next_h	);
}

double	SA::RKF45Integrator::getLastStepSize()	{return	last_h;}

double	SA::RKF45Integrator::adaptive_step(double	h)	{
				double	wstate[5][state_size];
				double	derivs[6][state_size];
				double	w2[state_size];
				double	R;
				do	{

								(*derivs_func)(	X_in,	inState,	derivs[0],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												wstate[0][i]	=	inState[i]	+	h	*	(1/4.0)	*	derivs[0][i];
								}
								(*derivs_func)(	X_in	+	(1/4.0)	*	h	,	wstate[0],	derivs[1],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												wstate[1][i]	=	inState[i]	+	h	*	((3/32.0)*derivs[0][i]	+
																																													(9/32.0)*derivs[1][i]);
								}
								(*derivs_func)(	X_in	+	(3/8.0)	*	h	,	wstate[1],	derivs[2],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												wstate[2][i]	=	inState[i]	+	h	*	((	1932/2197.0)	*	derivs[0][i]	+
																																													(-7200/2197.0)	*	derivs[1][i]	+
																																													(	7296/2197.0)	*	derivs[2][i]);
								}
								(*derivs_func)(	X_in	+	(12/13.0)	*	h	,	wstate[2],	derivs[3],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												wstate[3][i]	=	inState[i]	+	h	*	((	439/216.0)		*	derivs[0][i]	+
																																													(-8.0)								*	derivs[1][i]	+
																																													(	3680/513.0)	*	derivs[2][i]	+
																																													(-845/4104.0)	*	derivs[3][i]);
								}
								(*derivs_func)(	X_in	+	h	,	wstate[3],	derivs[4],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												wstate[4][i]	=	inState[i]	+	h	*	((-8/27.0)	*	derivs[0][i]	+
																																													(	2.0)	*	derivs[1][i]	+
																																													(-3544/2565.0)	*	derivs[2][i]	+
																																													(	1859/4104.0)	*	derivs[3][i]	+
																																													(-11/40.0)	*	derivs[4][i]);
								}
								(*derivs_func)(	X_in	+	(1/2.0)*	h	,	wstate[4],	derivs[5],	user_data);
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												outState[i]	=	inState[i]	+	((25/216.0)	*	derivs[0][i]	+
																																	(1408/2565.0)	*	derivs[2][i]	+
																																	(2197/4104.0)	*	derivs[3][i]	+
																																	(-1/5.0)	*	derivs[4][i])	*	h;
								}
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												w2[i]	=	inState[i]	+	((	16/135.0)	*	derivs[0][i]	+
																																		(	6656/12825.0)	*	derivs[2][i]	+
																																		(	28561/56430.0)	*	derivs[3][i]	+
																																		(-9/50.0)	*	derivs[4][i]	+
																																		(	2/55.0)	*	derivs[5][i])	*	h;
								}
								last_h	=	h;
								R	=	0.0;
								for	(unsigned	int	i=0;	i<state_size;	i++)	{
												double	RI	=	fabs(outState[i]	-	w2[i])	/	h;
												if	(RI>R)	R	=	RI;
								}
								if	(R	==	0.0)	{
												next_h	=	default_h;
								}	else	{
												double	delta	=	0.84	*	pow((epsilon/R),	0.25);
												next_h	=	delta	*	h;
												if	(next_h	>	default_h)
																next_h	=	default_h;
								}
								h	=	next_h;
				}	while	(R	>	epsilon);

				advanceIndyVar(last_h);
				return	(next_h);
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	RKF45Integrator&	I)	{
				os	<<	(SA::FirstOrderODEIntegrator)I;
				os	<<	"\nepsilon:	"	<<	I.epsilon;
				os	<<	"\nnext_h:	"	<<	I.next_h;
				return	os;
}

//	------------------------------------------------------------
//	Class	EulerCromerIntegrator
//	------------------------------------------------------------
//	Default	Constructor
SA::EulerCromerIntegrator::EulerCromerIntegrator()
				:Integrator()	{
				nDimensions		=	1;
				last_h	=	0.0;
				pos_p	=	NULL;
				pos_in		=	new	double[nDimensions];
				pos_out	=	new	double[nDimensions];
				vel_p	=	NULL;
				vel_in		=	new	double[nDimensions];
				vel_out	=	new	double[nDimensions];
				g_out			=	new	double[nDimensions];
				gderivs	=	nil_gfunc;
}
//	Constructor
SA::EulerCromerIntegrator::EulerCromerIntegrator(
				double	dt,	unsigned	int	N,	double*	xp[],	double*	vp[],	Derivs2Func	gfunc,	void*	udata)
				:Integrator(dt,	udata)	{
				nDimensions		=	N;
				last_h	=	0.0;
				pos_p	=	xp;
				pos_in		=	new	double[N];
				pos_out	=	new	double[N];
				vel_p	=	vp;
				vel_in		=	new	double[N];
				vel_out	=	new	double[N];
				g_out			=	new	double[N];
				gderivs	=	gfunc;
}
//	Copy	Constructor
SA::EulerCromerIntegrator::EulerCromerIntegrator(const	EulerCromerIntegrator&	other)
:	Integrator(other.default_h,	other.user_data)	{
				nDimensions	=	other.nDimensions;
				last_h	=	other.last_h;
				pos_p	=	other.pos_p;
				pos_in	=	new	double[nDimensions];
				std::copy(	other.pos_in,	other.pos_in	+	nDimensions,	pos_in);
				pos_out	=	new	double[nDimensions];
				std::copy(	other.pos_out,	other.pos_out	+	nDimensions,	pos_out);
				vel_p	=	other.vel_p;
				std::copy(	other.vel_p,	other.vel_p	+	nDimensions,	vel_p);
				vel_in	=	new	double[nDimensions];
				std::copy(	other.vel_in,	other.vel_in	+	nDimensions,	vel_in);
				vel_out	=	new	double[nDimensions];
				std::copy(	other.vel_out,	other.vel_out	+	nDimensions,	vel_out);
				g_out	=	new	double[nDimensions];
				std::copy(	other.g_out,	other.g_out	+	nDimensions,	g_out);
				gderivs	=	other.gderivs;
}
//	Assignment	Operator
SA::EulerCromerIntegrator&	SA::EulerCromerIntegrator::operator=(	const	SA::EulerCromerIntegrator&	rhs)	{
				if	(this	!=	&rhs)	{
								//	Call	base	assignment	operator
								Integrator::operator=(rhs);
								//	Duplicate	the	rhs	arrays
								double*	new_pos_in		=	new	double[rhs.nDimensions];
								std::copy(	rhs.pos_in,	rhs.pos_in	+	rhs.nDimensions,	new_pos_in);
								double*	new_pos_out	=	new	double[rhs.nDimensions];
								std::copy(	rhs.pos_out,	rhs.pos_out	+	rhs.nDimensions,	new_pos_out);
								double*	new_vel_in		=	new	double[rhs.nDimensions];
								std::copy(	rhs.vel_in,	rhs.vel_in	+	rhs.nDimensions,	new_vel_in);
								double*	new_vel_out	=	new	double[rhs.nDimensions];
								std::copy(	rhs.vel_out,	rhs.vel_out	+	rhs.nDimensions,	new_vel_out);
								double*	new_g_out	=	new	double[rhs.nDimensions];
								std::copy(	rhs.g_out,	rhs.g_out	+	rhs.nDimensions,	new_g_out);
								//	Delete	lhs	arrays	&	replace	with	rhs	arrays
								if	(pos_in	!=	NULL)	delete	pos_in;
								pos_in	=	new_pos_in;
								if	(pos_out	!=	NULL)	delete	pos_out;
								pos_out	=	new_pos_out;
								if	(vel_in	!=	NULL)	delete	vel_in;
								vel_in	=	new_vel_in;
								if	(vel_out	!=	NULL)	delete	vel_out;
								vel_out	=	new_vel_out;
								if	(g_out	!=	NULL)	delete	g_out;
								g_out	=	new_g_out;
								//	Copy	primitive	members
								last_h	=	rhs.last_h;
								pos_p	=	rhs.pos_p;
								vel_p	=	rhs.vel_p;
								nDimensions	=	rhs.nDimensions;
								gderivs	=	rhs.gderivs;
				}
				return	*this;
}
//	Destructor
SA::EulerCromerIntegrator::~EulerCromerIntegrator()	{
				if	(pos_in	!=	NULL)		delete	pos_in;
				if	(pos_out	!=	NULL)	delete	pos_out;
				if	(vel_in	!=	NULL)		delete	vel_in;
				if	(vel_out	!=	NULL)	delete	vel_out;
				if	(g_out	!=	NULL)			delete	g_out;
}
void	SA::EulerCromerIntegrator::advanceIndyVar(double	h)	{
				last_h	=	h;	X_out	=	X_in	+	h;
}
void	SA::EulerCromerIntegrator::step(	double	dt)	{
				(*gderivs)(	X_in,	pos_in,	vel_in,	g_out,	user_data);
				for	(int	i=0	;	i<nDimensions;	i++	)	{
								vel_out[i]	=	vel_in[i]	+	g_out[i]	*	dt;
								pos_out[i]	=	pos_in[i]	+	vel_out[i]	*	dt;
				}
				advanceIndyVar(dt);
}
void	SA::EulerCromerIntegrator::step()	{
				step(default_h);
}
void	SA::EulerCromerIntegrator::load()	{
				if	((pos_in	!=	NULL)	&&	(vel_in	!=	NULL))	{
								for	(int	i=0	;	i<nDimensions;	i++	)	{
												pos_in[i]	=	*(pos_p[i]);
												vel_in[i]	=	*(vel_p[i]);
								}
								Integrator::load();
				}	else	{
								std::cerr	<<	"Error:	SA::EulerCromerIntegrator::load().	Input	variable	pointers	(pos_in,	vel_in)	aren't	set."	<<	std::endl;
				}
}
void	SA::EulerCromerIntegrator::unload(){
				if	((pos_in	!=	NULL)	&&	(vel_in	!=	NULL))	{
								for	(int	i=0	;	i<nDimensions;	i++	)	{
												*(pos_p[i])	=	pos_out[i];
												*(vel_p[i])	=	vel_out[i];
								}
				}	else	{
								std::cerr	<<	"Error:	SA::EulerCromerIntegrator::load().	Input	variable	pointers	(pos_in,	vel_in)	aren't	set."	<<	std::endl;
				}
}
double	SA::EulerCromerIntegrator::undo_integrate()	{
				if	((pos_in	!=	NULL)	&&	(vel_in	!=	NULL))	{
								for	(unsigned	int	i=0	;	i<nDimensions;	i++	)	{
												*(pos_p[i])	=	pos_in[i];
												*(vel_p[i])	=	vel_in[i];
								}
								std::copy(pos_in,	pos_in	+	nDimensions,	pos_out);
								std::copy(vel_in,	vel_in	+	nDimensions,	vel_out);
								return	(SA::Integrator::undo_integrate());
				}	else	{
								std::cerr	<<	"Error:	SA::EulerCromerIntegrator::undo_integrate().	Input	variable	pointers	(pos_in,	vel_in)	aren't	set."	<<	std::endl;
				}
				return	(last_h);
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	EulerCromerIntegrator&	I)	{
				os	<<	(SA::Integrator)I;
				os	<<	"\n---	EulerCromerIntegrator	---";
				os	<<	"\nnDimensions	:	"	<<	I.nDimensions;
				os	<<	"\nlast_h						:	"	<<	I.last_h;
				os	<<	"\npos_p							:"	<<	I.pos_p;
				os	<<	"\nvel_p							:"	<<	I.vel_p;
				os	<<	"\npos_in				:";
				stream_double_array(os,	I.nDimensions,	I.pos_in);
				os	<<	"\npos_out				:";
				stream_double_array(os,	I.nDimensions,	I.pos_out);
				os	<<	"\nvel_in				:";
				stream_double_array(os,	I.nDimensions,	I.vel_in);
				os	<<	"\nvel_out				:";
				stream_double_array(os,	I.nDimensions,	I.vel_out);
				os	<<	"\ng_out				:";
				stream_double_array(os,	I.nDimensions,	I.g_out);
				return	os;
}

//	------------------------------------------------------------
//	Class	ABMIntegrator
//	------------------------------------------------------------
//	//	Adams-Bashforth	Coefficients
static	const	double	ABCoeffs[5][5]	=	{
		{1.0,											0.0,											0.0,										0.0,											0.0},
		{(3/2.0),						(-1/2.0),							0.0,										0.0,											0.0},
		{(23/12.0),				(-16/12.0),				(5/12.0),						0.0,											0.0},
		{(55/24.0),				(-59/24.0),				(37/24.0),				(-9/24.0),						0.0},
		{(1901/720.0),	(-2774/720.0),	(2616/720.0),	(-1274/720.0),	(251/720.0)}
};
//
//	//	Adams-Moulton	Coefficients
static	const	double	AMCoeffs[5][5]	=	{
		{1.0,										0.0,									0.0,										0.0,									0.0},
		{(1/2.0),					(1/2.0),						0.0,										0.0,									0.0},
		{(5/12.0),				(8/12.0),				(-1/12.0),					0.0,									0.0},
		{(9/24.0),				(19/24.0),			(-5/24.0),				(1/24.0),					0.0},
		{(251/720.0),	(646/720.0),	(-264/720.0),	(106/720.0),	(-19/720.0)}
};

//	Default	Constructor

//	Constructor
SA::ABMIntegrator::ABMIntegrator	(	unsigned	int	history_size,	double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	func,	void*	udata)
				:	FirstOrderODEIntegrator(h,	N,	in_vars,	out_vars	,func,	udata)	{
				if	((history_size	<	1)	||	(history_size	>	5))	{
								throw	std::invalid_argument("history_size	must	be	in	the	range	[1..5].");
				}
				algorithmHistorySize=history_size;	//	The	amount	of	history	that	we	intend	to	use	in	this	ABM	integrator.
				bufferSize=algorithmHistorySize+1;	//	The	size	of	the	buffer	needs	to	be	one	more	than	the	history	so	that	an	integration	step	can	be	reset.
				hix=0;
				currentHistorySize=0;														//	How	much	derivative	history	is	currently	stored	int	the	history	buffer.	Initially	there	will	be	none	until	we've	primed	the	integrator.
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
				}
				composite_deriv	=	new	double[state_size];
}
//	Constructor
SA::ABMIntegrator::ABMIntegrator(
unsigned	int	history_size,	double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	ABMIntegrator(history_size,	h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}

//	Copy	Constructor
SA::ABMIntegrator::ABMIntegrator(const	ABMIntegrator&	other)
:	FirstOrderODEIntegrator(other)	{
				bufferSize	=	other.bufferSize;
				algorithmHistorySize	=	other.algorithmHistorySize;
				hix	=	other.hix;
				currentHistorySize	=	other.currentHistorySize;
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
								std::copy(other.deriv_history[i],	other.deriv_history[i]+state_size,	deriv_history[i]);
				}
				composite_deriv	=	new	double[state_size];
				std::copy(other.composite_deriv,	other.composite_deriv+state_size,	composite_deriv);
}
//	Assignment	Operator
SA::ABMIntegrator&	SA::ABMIntegrator::operator=(	const	SA::ABMIntegrator&	rhs)	{
				if	(this	!=	&rhs)	{
								//	Call	base	assignment	operator
								FirstOrderODEIntegrator::operator=(rhs);
								//	Duplicate	rhs	arrays
								double**	new_deriv_history	=	new	double*[rhs.bufferSize];
								for	(unsigned	int	i=0	;	i<rhs.bufferSize;	i++	)	{
												new_deriv_history[i]	=	new	double[rhs.state_size];
												std::copy(rhs.deriv_history[i],	rhs.deriv_history[i]	+	rhs.state_size,	new_deriv_history[i]);
								}
								double*	new_composite_deriv	=	new	double[rhs.state_size];
								std::copy(rhs.composite_deriv,	rhs.composite_deriv+rhs.state_size,	new_composite_deriv);
								//	Delete	lhs	arrays	&	replace	with	rhs	arrays
								for	(unsigned	int	i=0	;	i<bufferSize;	i++	)	{
												delete	deriv_history[i];
								}
								delete	deriv_history;
								deriv_history	=	new_deriv_history;
								delete	composite_deriv;
								composite_deriv	=	new_composite_deriv;
								//	Copy	primitive	members
								bufferSize	=	rhs.bufferSize;
								algorithmHistorySize	=	rhs.algorithmHistorySize;
								hix	=	rhs.hix;
								currentHistorySize	=	rhs.currentHistorySize;
				}
				return	*this;
}
//	Destructor
SA::ABMIntegrator::~ABMIntegrator()	{
				for	(int	i=0;	i<bufferSize	;	i++)	{
								delete	(deriv_history[i]);
				}
				delete(deriv_history);
				delete(composite_deriv);
}
void	SA::ABMIntegrator::step()	{

				hix	=	(hix+1)%bufferSize;																																						//	Move	history	index	forward
				(*derivs_func)(	X_in,	inState,	deriv_history[hix],	user_data);	//	Calculated	and	store	the	deriv	for	current,	corrected	state.
				//	Increase	the	size	of	the	stored	history,	up	to	the	limit	specified	by	the	user.
				currentHistorySize	=	(currentHistorySize	<	algorithmHistorySize)	?	currentHistorySize+1	:	algorithmHistorySize;
				//	(Predictor)	Predict	the	next	state	using	the	Adams-Bashforth	explicit	method.
				for	(int	i=0;	i<state_size;	i++)	{
								composite_deriv[i]	=	0.0;
								for	(int	n=0,j=hix;	n<currentHistorySize	;	n++,j=(j+bufferSize-1)%bufferSize)	{
												composite_deriv[i]	+=	ABCoeffs[currentHistorySize-1][n]	*	deriv_history[j][i];
								}
								outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
				}
				//	Move	history	index	forward,	so	we	can	temporarily	store	the	derivative	of	the	predicted	next	state.
				//	We	do	not	increase	the	currentHistorySize.
				hix	=	(hix+1)%bufferSize;
				(*derivs_func)(	X_out,	outState,	deriv_history[hix],	user_data);	//	Calc	deriv	for	predicted	next	state.
				//	(Corrector)	Refine	the	next	state	using	the	Adams-Moulton	implicit	method.	This	is	the	corrected	next	state.
				for	(int	i=0;	i<state_size;	i++)	{
								composite_deriv[i]	=	0.0;
								for	(int	n=0,j=hix;	n<currentHistorySize	;	n++,j=(j+bufferSize-1)%bufferSize)	{
												composite_deriv[i]	+=	AMCoeffs[currentHistorySize-1][n]	*	deriv_history[j][i];
								}
								outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
				}
				//	Move	history	index	backward,	so	we	over-write	the	predicted	state	with	the	corrected	state	on	our	next	step().
				hix	=	(hix+bufferSize-1)%bufferSize;
				advanceIndyVar();
}
double	SA::ABMIntegrator::undo_integrate()	{
				hix	=	(hix+bufferSize-1)%bufferSize;
				return	(FirstOrderODEIntegrator::undo_integrate());
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	ABMIntegrator&	I)	{
				os	<<	(SA::FirstOrderODEIntegrator)I;
				os	<<	"\n---	ABMIntegrator	---";
				os	<<	"\nbufferSize											:	"	<<	I.bufferSize;
				os	<<	"\nalgorithmHistorySize	:	"	<<	I.algorithmHistorySize;
				os	<<	"\nhix																		:	"	<<	I.hix;
				os	<<	"\ncurrentHistorySize			:	"	<<	I.currentHistorySize;
				os	<<	"\nderiv_history								:[";
				for	(int	i=0;	i<I.bufferSize	;	i++)	{
								os	<<	"\n";
								stream_double_array(os,	I.state_size,	I.deriv_history[i]);
				}
				os	<<	"\n]";
				os	<<	"\ncomposite_deriv					:";
				stream_double_array(os,	I.state_size,	I.composite_deriv);
				return	os;
}

//	************************************************************
//																							ATTENTION!!!
//	ABM	Integrators	CAN	NOT	have	a	variable	step,	because	of	its
//	derivative	history.
//	************************************************************

//	------------------------------------------------------------
//	Class	ABM2Integrator
//	------------------------------------------------------------
static	const	double	AB2Coeffs[2]	=	{(3/2.0),	(-1/2.0)};
static	const	double	AM2Coeffs[2]	=	{(1/2.0),		(1/2.0)};

SA::ABM2Integrator::ABM2Integrator	(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{
				//	The	amount	of	history	that	we	intend	to	use	in	this	ABM	integrator.
				bufferSize=2+1;	//	The	size	of	the	buffer	needs	to	be	one	more	than	the	history	so	that	an	integration	step	can	be	reset.
				hix=0;
				currentHistorySize=0;												//	How	much	derivative	history	is	currently	stored	int	the	history	buffer.	Initially	there	will	be	none	until	we've	primed	the	integrator.
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
				}
				composite_deriv	=	new	double[state_size];

				//	Create	a	priming	integrator.
				double**	priming_integrator_in_vars	=	new	double*[state_size];
				for	(unsigned	int	i=0;	i<N	;	i++)	{
								priming_integrator_in_vars[i]	=	&(inState[i]);
				}
				double**	priming_integrator_out_vars	=	new	double*[state_size];
				for	(unsigned	int	i=0;	i<N	;	i++)	{
								priming_integrator_out_vars[i]	=	&(outState[i]);
				}
				priming_integrator	=	new	SA::RK2Integrator(h,	N,	priming_integrator_in_vars,	priming_integrator_out_vars,	dfunc,	udata);
}
//	Constructor
SA::ABM2Integrator::ABM2Integrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	ABM2Integrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}

SA::ABM2Integrator::ABM2Integrator(const	ABM2Integrator&	other)
:	FirstOrderODEIntegrator(other)	{
				bufferSize	=	other.bufferSize;
				hix	=	other.hix;
				currentHistorySize	=	other.currentHistorySize;
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
								std::copy(other.deriv_history[i],	other.deriv_history[i]+state_size,	deriv_history[i]);
				}
				composite_deriv	=	new	double[state_size];
				std::copy(other.composite_deriv,	other.composite_deriv+state_size,	composite_deriv);

				priming_integrator	=	new	RK2Integrator(*other.priming_integrator);
}
//	Assignment	Operator
SA::ABM2Integrator&	SA::ABM2Integrator::operator=(	const	SA::ABM2Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								//	Call	base	assignment	operator
								FirstOrderODEIntegrator::operator=(rhs);
								//	Duplicate	rhs	arrays
								double**	new_deriv_history	=	new	double*[rhs.bufferSize];
								for	(unsigned	int	i=0	;	i<rhs.bufferSize;	i++	)	{
												new_deriv_history[i]	=	new	double[rhs.state_size];
												std::copy(rhs.deriv_history[i],	rhs.deriv_history[i]	+	rhs.state_size,	new_deriv_history[i]);
								}
								double*	new_composite_deriv	=	new	double[rhs.state_size];
								std::copy(rhs.composite_deriv,	rhs.composite_deriv+rhs.state_size,	new_composite_deriv);
								//	Delete	lhs	arrays	&	replace	with	rhs	arrays
								for	(unsigned	int	i=0	;	i<bufferSize;	i++	)	{
												delete	deriv_history[i];
								}
								delete	deriv_history;
								deriv_history	=	new_deriv_history;
								delete	composite_deriv;
								composite_deriv	=	new_composite_deriv;
								//	Copy	primitive	members
								bufferSize	=	rhs.bufferSize;
								hix	=	rhs.hix;
								currentHistorySize	=	rhs.currentHistorySize;
				}
				return	*this;
}

SA::ABM2Integrator::~ABM2Integrator()	{
				for	(int	i=0;	i<bufferSize	;	i++)	{
								delete	(deriv_history[i]);
				}
				delete(deriv_history);
				delete(composite_deriv);
				delete	priming_integrator;
}
void	SA::ABM2Integrator::step()	{

				hix	=	(hix+1)%bufferSize;																										//	Move	history	index	forward
				(*derivs_func)(	X_in,	inState,	deriv_history[hix],	user_data);	//	Calculated	and	store	the	deriv	for	current,	corrected	state.
				//	Increase	the	size	of	the	stored	history,	up	to	the	limit	specified	by	the	user.
				currentHistorySize	=	(currentHistorySize	<	2)	?	currentHistorySize+1	:	2;

				if	(	currentHistorySize	<	2	)	{
								priming_integrator->load();
								priming_integrator->step();
								priming_integrator->unload();
				}	else	{
								//	(Predictor)	Predict	the	next	state	using	the	Adams-Bashforth	explicit	method.
								for	(int	i=0;	i<state_size;	i++)	{
												composite_deriv[i]	=	0.0;
												for	(int	n=0,j=hix;	n<2	;	n++,j=(j+bufferSize-1)%bufferSize)	{
																composite_deriv[i]	+=	AB2Coeffs[n]	*	deriv_history[j][i];
												}
												outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
								}
								//	Move	history	index	forward,	so	we	can	temporarily	store	the	derivative	of	the	predicted	next	state.
								//	We	do	not	increase	the	currentHistorySize.
								hix	=	(hix+1)%bufferSize;
								(*derivs_func)(	X_in,	outState,	deriv_history[hix],	user_data);	//	Calc	deriv	for	predicted	next	state.
								//	(Corrector)	Refine	the	next	state	using	the	Adams-Moulton	implicit	method.	This	is	the	corrected	next	state.
								for	(int	i=0;	i<state_size;	i++)	{
												composite_deriv[i]	=	0.0;
												for	(int	n=0,j=hix;	n<2	;	n++,j=(j+bufferSize-1)%bufferSize)	{
																composite_deriv[i]	+=	AM2Coeffs[n]	*	deriv_history[j][i];
												}
												outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
								}
								//	Move	history	index	backward,	so	we	over-write	the	predicted	state	with	the	corrected	state	on	our	next	step().
								hix	=	(hix+bufferSize-1)%bufferSize;
				}
				advanceIndyVar();
}
double	SA::ABM2Integrator::undo_integrate()	{
				hix	=	(hix+bufferSize-1)%bufferSize;
				return	(	FirstOrderODEIntegrator::undo_integrate());
}

//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	ABM2Integrator&	I)	{
				os	<<	(SA::FirstOrderODEIntegrator)I;
				os	<<	"\n---	ABM2Integrator	---";
				os	<<	"\nbufferSize											:	"	<<	I.bufferSize;
				os	<<	"\nhix																		:	"	<<	I.hix;
				os	<<	"\ncurrentHistorySize			:	"	<<	I.currentHistorySize;
				os	<<	"\nderiv_history								:[";
				for	(int	i=0;	i<I.bufferSize	;	i++)	{
								os	<<	"\n";
								stream_double_array(os,	I.state_size,	I.deriv_history[i]);
				}
				os	<<	"\n]";
				os	<<	"\ncomposite_deriv					:";
				stream_double_array(os,	I.state_size,	I.composite_deriv);
				return	os;
}
//	------------------------------------------------------------
//	Class	ABM4Integrator
//	------------------------------------------------------------
static	const	double	AB4Coeffs[4]	=	{(55/24.0),	(-59/24.0),	(37/24.0),	(-9/24.0)};
static	const	double	AM4Coeffs[4]	=	{(9/24.0),	(19/24.0),	(-5/24.0),	(1/24.0)};

SA::ABM4Integrator::ABM4Integrator	(
				double	h,	unsigned	int	N,	double*	in_vars[],	double*	out_vars[],	DerivsFunc	dfunc,	void*	udata)
				:	FirstOrderODEIntegrator(h,	N,	in_vars,	out_vars,	dfunc,	udata)	{

				//	The	amount	of	history	that	we	intend	to	use	in	this	ABM	integrator.
				bufferSize=4+1;	//	The	size	of	the	buffer	needs	to	be	one	more	than	the	history	so	that	an	integration	step	can	be	reset.
				hix=0;
				currentHistorySize=0;												//	How	much	derivative	history	is	currently	stored	int	the	history	buffer.	Initially	there	will	be	none	until	we've	primed	the	integrator.
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
				}
				composite_deriv	=	new	double[state_size];
				//	NEW:	Create	a	priming	integrator.
				double**	priming_integrator_in_vars	=	new	double*[state_size];
				for	(unsigned	int	i=0;	i<N	;	i++)	{
								priming_integrator_in_vars[i]	=	&(inState[i]);
				}
				double**	priming_integrator_out_vars	=	new	double*[state_size];
				for	(unsigned	int	i=0;	i<N	;	i++)	{
								priming_integrator_out_vars[i]	=	&(outState[i]);
				}
				priming_integrator	=	new	SA::RK4Integrator(h,	N,	priming_integrator_in_vars,	priming_integrator_out_vars,	dfunc,	udata);
}
//	Constructor
SA::ABM4Integrator::ABM4Integrator(
double	h,	unsigned	int	N,	double*	in_out_vars[],	DerivsFunc	dfunc,	void*	udata)
:	ABM4Integrator(h,	N,	in_out_vars,	in_out_vars,	dfunc,	udata)	{}

SA::ABM4Integrator::ABM4Integrator(const	ABM4Integrator&	other)
:	FirstOrderODEIntegrator(other)	{
				bufferSize	=	other.bufferSize;
				hix	=	other.hix;
				currentHistorySize	=	other.currentHistorySize;
				deriv_history	=	new	double*[bufferSize];
				for	(unsigned	int	i=0;	i<bufferSize	;	i++)	{
								deriv_history[i]	=	new	double[state_size];
								std::copy(other.deriv_history[i],	other.deriv_history[i]+state_size,	deriv_history[i]);
				}
				composite_deriv	=	new	double[state_size];
				std::copy(other.composite_deriv,	other.composite_deriv+state_size,	composite_deriv);

				priming_integrator	=	new	RK4Integrator(*other.priming_integrator);
}
//	Assignment	Operator
SA::ABM4Integrator&	SA::ABM4Integrator::operator=(	const	SA::ABM4Integrator&	rhs)	{
				if	(this	!=	&rhs)	{
								//	Call	base	assignment	operator
								FirstOrderODEIntegrator::operator=(rhs);
								//	Duplicate	rhs	arrays
								double**	new_deriv_history	=	new	double*[rhs.bufferSize];
								for	(unsigned	int	i=0	;	i<rhs.bufferSize;	i++	)	{
												new_deriv_history[i]	=	new	double[rhs.state_size];
												std::copy(rhs.deriv_history[i],	rhs.deriv_history[i]	+	rhs.state_size,	new_deriv_history[i]);
								}
								double*	new_composite_deriv	=	new	double[rhs.state_size];
								std::copy(rhs.composite_deriv,	rhs.composite_deriv+rhs.state_size,	new_composite_deriv);
								//	Delete	lhs	arrays	&	replace	with	rhs	arrays
								for	(unsigned	int	i=0	;	i<bufferSize;	i++	)	{
												delete	deriv_history[i];
								}
								delete	deriv_history;
								deriv_history	=	new_deriv_history;
								delete	composite_deriv;
								composite_deriv	=	new_composite_deriv;
								//	Copy	primitive	members
								bufferSize	=	rhs.bufferSize;
								hix	=	rhs.hix;
								currentHistorySize	=	rhs.currentHistorySize;
				}
				return	*this;
}
SA::ABM4Integrator::~ABM4Integrator()	{
				for	(int	i=0;	i<bufferSize	;	i++)	{
								delete	(deriv_history[i]);
				}
				delete(deriv_history);
				delete(composite_deriv);
				delete	priming_integrator;
}
void	SA::ABM4Integrator::step()	{

				hix	=	(hix+1)%bufferSize;																										//	Move	history	index	forward
				(*derivs_func)(	X_in,	inState,	deriv_history[hix],	user_data);	//	Calculated	and	store	the	deriv	for	current,	corrected	state.
				//	Increase	the	size	of	the	stored	history,	up	to	the	limit	specified	by	the	user.
				currentHistorySize	=	(currentHistorySize	<	4)	?	currentHistorySize+1	:	4;
				if	(	currentHistorySize	<	4	)	{
								priming_integrator->load();
								priming_integrator->step();
								priming_integrator->unload();
				}	else	{
								//	(Predictor)	Predict	the	next	state	using	the	Adams-Bashforth	explicit	method.
								for	(int	i=0;	i<state_size;	i++)	{
												composite_deriv[i]	=	0.0;
												for	(int	n=0,j=hix;	n<4	;	n++,j=(j+bufferSize-1)%bufferSize)	{
																composite_deriv[i]	+=	AB4Coeffs[n]	*	deriv_history[j][i];
												}
												outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
								}
								//	Move	history	index	forward,	so	we	can	temporarily	store	the	derivative	of	the	predicted	next	state.
								//	We	do	not	increase	the	currentHistorySize.
								hix	=	(hix+1)%bufferSize;
								(*derivs_func)(	X_out,	outState,	deriv_history[hix],	user_data);	//	Calc	deriv	for	predicted	next	state.
								//	(Corrector)	Refine	the	next	state	using	the	Adams-Moulton	implicit	method.	This	is	the	corrected	next	state.
								for	(int	i=0;	i<state_size;	i++)	{
												composite_deriv[i]	=	0.0;
												for	(int	n=0,j=hix;	n<4	;	n++,j=(j+bufferSize-1)%bufferSize)	{
																composite_deriv[i]	+=	AM4Coeffs[n]	*	deriv_history[j][i];
												}
												outState[i]	=	inState[i]	+	default_h	*	composite_deriv[i];
								}
								//	Move	history	index	backward,	so	we	over-write	the	predicted	state	with	the	corrected	state	on	our	next	step().
								hix	=	(hix+bufferSize-1)%bufferSize;
				}
				advanceIndyVar();
}
double	SA::ABM4Integrator::undo_integrate()	{
				hix	=	(hix+bufferSize-1)%bufferSize;
				return	(FirstOrderODEIntegrator::undo_integrate());
}
//	Insertion	Operator
std::ostream&	SA::operator<<(std::ostream&	os,	const	ABM4Integrator&	I)	{
				os	<<	(SA::FirstOrderODEIntegrator)I;
				os	<<	"\n---	ABM4Integrator	---";
				os	<<	"\nbufferSize											:	"	<<	I.bufferSize;
				os	<<	"\nhix																		:	"	<<	I.hix;
				os	<<	"\ncurrentHistorySize			:	"	<<	I.currentHistorySize;
				os	<<	"\nderiv_history								:[";
				for	(int	i=0;	i<I.bufferSize	;	i++)	{
								os	<<	"\n";
								stream_double_array(os,	I.state_size,	I.deriv_history[i]);
				}
				os	<<	"\n]";
				os	<<	"\ncomposite_deriv					:";
				stream_double_array(os,	I.state_size,	I.composite_deriv);
				return	os;
}
