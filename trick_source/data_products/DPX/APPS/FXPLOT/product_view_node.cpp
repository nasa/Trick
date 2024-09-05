
#include	<iostream>
#include	<string>
#include	"product_view_node.hh"

extern	"C"{
#include	<X11/Intrinsic.h>
}

int	ProductViewNode::dpIndex	=	0;

ProductViewNode::ProductViewNode(Widget	Toplevel,	DPC_product*	Product)	{

				product	=	Product;
				toplevel	=	Toplevel;
}


//DESTRUCTOR
ProductViewNode::~ProductViewNode()	{

				#ifdef	DEBUG
				std::cout	<<	"ProductViewNode	DESTRUCTOR."	<<	std::endl;
				#endif

				PageViewNode		*page_view_node;
				TableViewNode	*table_view_node;
				int	n,i;

				n	=	(int)page_node_list.size();
				for	(i=0	;	i<n;	i++	)	{
								page_view_node	=	page_node_list[i];
								if	(page_view_node)	{	delete	page_view_node;	}
				}

				n	=	(int)table_node_list.size();
				for	(i=0	;	i<n	;	i++	)	{
								table_view_node	=	table_node_list[i];
								if	(table_view_node)	{	delete	table_view_node;	}
				}
}

void	ProductViewNode::generate_PS(const	char*	file_name)	{
				dpIndex++;
				int	ii;
				int	n	=	(int)page_node_list.size();
				std::string::size_type	idx	;
				std::string	file_name_str	;
				char	indexedFileName[1024];
				file_name_str	=	file_name	;

				//	as	the	file_name	is	already	provided	with	.ps	extension,
				//	remove	it	for	adding	the	index	for	multiple	pages	and
				//	the	.ps	extension	will	be	appended	when	file	is	generated.
				idx	=	file_name_str.rfind('.');
				if	(	idx	!=	string::npos	)	{
								if	(file_name_str.substr(idx)	==	".ps")	{
												file_name_str.erase(idx);
								}
				}

				for	(ii=0	;	ii<n;	ii++	)	{
								if	(n	>	1)	{
												snprintf(indexedFileName,	sizeof(indexedFileName),	"%s.%d.%d.ps",	file_name_str.c_str(),	dpIndex,	ii+1);
								}	else	{
												snprintf(indexedFileName,	sizeof(indexedFileName),	"%s.%d.ps",	file_name_str.c_str(),	dpIndex);
								}
								page_node_list[ii]->generate_PS(indexedFileName);
								std::cout	<<	"Generating	"	<<	indexedFileName	<<	"	..."	<<	std::endl;
				}

}

void	ProductViewNode::print()	{
				int	ii;
				int	n	=	(int)page_node_list.size();

				for	(ii	=	0;	ii	<	n;	ii++)	{
								page_node_list[ii]->print();
				}
}
