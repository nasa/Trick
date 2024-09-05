
#include	<stdio.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>

#include	"DPC/DPC_product.hh"
#include	"DPM/DPM_parse_tree.hh"
#include	"DPM/DPM_session.hh"
#include	"gp_view.hh"


int	eqp_errno;	//	to	satisfy	libeqparse.a

Widget	toplevel;
gnuplot_view	*view;
vector	<DPC_product*>	product_list;


int	file_exists(const	char	*fname)	{

				FILE	*fp	=	fopen(fname,"r");
				if	(fp)	{
								fclose(fp);
								return(1);
				}	else	{
								return(0);
				}
}

void	copy_prefix(char	*dest,	const	char*	src)	{

			size_t	prefix_len;
			char	*base;

			if	((base	=	(char	*)strrchr(	src,'/'))	!=	NULL	)	{
							prefix_len	=	strlen(src)	-	strlen(base);
							strncpy(dest,	src,	prefix_len);
							dest[prefix_len]	=	'\0';
			}	else	{
							dest[0]	=	'\0';
			}
}

int	main(int	argc,	char	*	argv[]	)	{

				XtAppContext	app;

				DPM_parse_tree	*session_parse_tree;
				xmlNode	*root_node;
				DPM_session	*session;

				int	n_product_files;
				int	n_products;
				int	product_ix;
				char	*session_file_name;


				if	(argc	<	2)	{
								cerr	<<	"ERROR:	No	session	file	specified."	<<	endl;
								exit(0);
				}

				session_file_name	=	argv[1];

				toplevel	=	XtVaAppInitialize	(	&app,	"Gxplot",	NULL,	0,	&argc,	argv,	NULL,	NULL	);

				view	=	new	gnuplot_view(toplevel);


				//Make	a	DPM_session	parse	tree
				try	{
								session_parse_tree	=	new	DPM_parse_tree(session_file_name);
				}
				catch	(std::invalid_argument)	{
								cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	endl;
								exit(1);
				}

				root_node	=	session_parse_tree->getRootNode();

				//Make	a	DPM_session	object
				try	{
								session	=	new	DPM_session(	NULL,	root_node);
				}
				catch	(std::invalid_argument)	{
								cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	endl;
								exit(1);
				}

				delete	session_parse_tree;



				n_product_files	=	(int)session->product_file_list.size();

				for	(product_ix	=	0;	product_ix	<	n_product_files;	product_ix++	)	{

								DPC_product	*product;

								char	product_file_fullname[1024];

								const	char	*product_file_name	=	session->product_file_list[product_ix];


								if	(file_exists(	product_file_name))	{

													strcpy(product_file_fullname,	product_file_name	);
								}
								else	{

													copy_prefix(product_file_fullname,	session_file_name);
													strcat(product_file_fullname,	"/");
													strcat(product_file_fullname,	product_file_name);
								}

								try	{

												product	=	new	DPC_product(	session,	product_file_fullname);
								}
								catch		(std::invalid_argument)	{

												cerr	<<	"ERROR:	Unable	to	create	the	product	specified	by	file:	\""	<<	product_file_name	<<	"\""	<<	endl;
												product	=	NULL;
								}

								if	(	product	!=	NULL	)	{

												product_list.push_back(	product	);
												product->render(	view	);
								}
				}


				n_products	=	(int)product_list.size();

				for	(	product_ix	=	0;	product_ix	<	n_products;	product_ix++	)	{

								DPC_product	*product;
								product	=	product_list[product_ix];
								product->notify(	view,	gnuplot_view::DATA_UPDATE	);
				}


				//	Delete	the	view	and	all	of	the	viewable	stuff	that	it	created.
				delete	view;

				//	Delete	the	list	of	DPC_product	objects
				n_products	=	(int)product_list.size();

				for	(product_ix	=	0;	product_ix	<	n_products;	product_ix++	)	{

								DPC_product	*product;
								product	=	product_list[product_ix];
								delete	product;
				}

				return	0;
}
