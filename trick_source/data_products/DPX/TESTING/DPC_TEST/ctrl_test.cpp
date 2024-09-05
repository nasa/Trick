
#include	<stdio.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	"DPC/DPC_product.hh"
#include	"DPM/DPM_parse_tree.hh"
#include	"DPM/DPM_session.hh"
#include	"test_view.hh"

int	main(int	argc,	char	*	argv[]	)	{
					DPM_parse_tree	*session_parse_tree;
					xmlNode	*root_node;
					DPM_session	*session;
					char	*session_file_name;

					vector	<DPC_product*>	product_list;

					test_view	*view;

					int	n_product_files;
					int	product_ix;

				if	(argc	!=	2)	{
									fprintf(stderr,"I	need	a	session	filename.\n");
									fflush(stderr);
									return	(1);
				}
				session_file_name	=	argv[1];

				cout	<<	"START	CTRL	TEST"	<<	endl;
				cout.flush();

				cout	<<	"NEW	TEST	VIEW"	<<	endl;
				cout.flush();
				view	=	new	test_view();

				try	{
								session_parse_tree	=	new	DPM_parse_tree(session_file_name);
				}	catch	(std::invalid_argument)	{
								cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	endl;
								exit(1);
				}

				root_node	=	session_parse_tree->getRootNode();

				try	{
								session	=	new	DPM_session(	NULL,	root_node);
				}	catch	(std::invalid_argument)	{
								cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	endl;
								exit(1);
				}

				delete	session_parse_tree;

				n_product_files	=	session->product_file_list.size();

				for	(product_ix	=	0;	product_ix	<	n_product_files;	product_ix++	)	{
								DPC_product	*product;
								const	char	*product_file_name	=	session->product_file_list[product_ix];

								try	{
												cout	<<	"NEW	DPC_PRODUCT"	<<	endl;
												cout.flush();
												product	=	new	DPC_product(	session,	product_file_name);
								}	catch		(std::invalid_argument)	{
												cerr	<<	"ERROR:	Unable	to	create	the	product	specified	by	file:	\""	<<
																product_file_name	<<	"\""	<<	endl;
												product	=	NULL;
								}

								if	(product	!=	NULL)	{
												product_list.push_back(	product);

												cout	<<	"RENDER	VIEW"	<<	endl;
												cout.flush();
												product->render(	view);
								}
				}

					return	0;
}
