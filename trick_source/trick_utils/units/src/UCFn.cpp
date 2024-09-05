/*
PURPOSE:	(Conversion	Function	Class	for	Units	Conversion	Package)

PROGRAMMERS:
				(((John	M.	Penn)	(L-3Com/Titan)	(May	2006)	(v1.0)
						(Initial	Release)))
*/

#include	"trick/UCFn.hh"

UCFn::UCFn(const	char	*in_t_name,	const	char	*in_f_name,	double	C1,	double	C0)	{
				this->C[1]=C1;
				this->C[0]=C0;
				strncpy(this->t_name,	in_t_name,	sizeof(this->t_name)-1);
				strncpy(this->f_name,	in_f_name,	sizeof(this->f_name)-1);
};

double	UCFn::eval(	double	val)	{
				return	(	this->C[1]*val	+	this->C[0]	);
};

std::ostream&	operator<<	(std::ostream&	s,	const	UCFn	*p)	{
				return	s	<<	"<"	<<	p->t_name	<<	">	=	"	<<	p->C[1]	<<	"	*	<"	<<	p->f_name	<<	">	+	"	<<	p->C[0];
};
