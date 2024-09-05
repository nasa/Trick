#include	<stdexcept>
#include	<iostream>
#include	<string>

#include	<stdio.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>

#include	<Xm/MainW.h>
#include	<Xm/RowColumn.h>
#include	<Xm/List.h>
#include	<Xm/CascadeB.h>
#include	<Xm/PushBG.h>
#include	<Xm/SeparatoG.h>
#include	<Xm/MessageB.h>
#include	<Xm/Form.h>
#include	<Xm/LabelG.h>
#include	<Xm/ScrolledW.h>
#include	<Xm/Text.h>

#include	"DPC/DPC_product.hh"
#include	"DPM/DPM_parse_tree.hh"
#include	"DPM/DPM_session.hh"
#include	"fermi_view.hh"
#include	"post_dialog.h"

//	FIXME:	Consider	changing	the	ugly	page/table/product	selection	lists	with	option	menus.


int	eqp_errno;	//	to	satisfy	libeqparse.a
int	run_app	=	1;

Widget	toplevel;
Widget	main_window;
fermi_view	*view;
std::vector	<DPC_product*>	product_list;
std::vector	<ProductViewNode	*>	x_product_list;
Widget	product_list_widget;
Widget	page_list_widget;
Widget	table_list_widget;
Widget	make_pdf_button;
Widget	print_button;
int	selected_product_index	=	-1;
int	selected_page_index	=	-1;
int	selected_table_index	=	-1;


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

void	select_page(int	page_index)	{
				int	n_products;
				DPC_product	*selected_product;
				DPC_page				*selected_page;

				n_products	=	(int)product_list.size();
				if	((selected_product_index	>=	0)	&&	(selected_product_index	<	n_products))	{
								int	n_pages;
								selected_product	=	product_list[selected_product_index];
								n_pages	=	selected_product->NumberOfPages();
								if	((page_index	>=	0)	&&	(page_index	<	n_pages))	{
												selected_page_index	=	page_index;
												selected_page	=	selected_product->getPage(	selected_page_index);
												selected_page->notify(	view,	fermi_view::RAISE_WINDOW	);
												XRaiseWindow(	XtDisplay(main_window),XtWindow(main_window));
								}
				}
}

void	select_table(int	table_index)	{
				int	n_products;
				DPC_product	*selected_product;
				DPC_table			*selected_table;

				n_products	=	(int)product_list.size();
				if	((selected_product_index	>=	0)	&&	(selected_product_index	<	n_products))	{
								int	n_tables;
								selected_product	=	product_list[selected_product_index];
								n_tables	=	selected_product->NumberOfTables();
								if	((table_index	>=	0)	&&	(table_index	<	n_tables))	{
												selected_table_index	=	table_index;
												selected_table	=	selected_product->getTable(	selected_table_index);
												selected_table->notify(	view,	fermi_view::RAISE_WINDOW	);
								}
				}
}

void	select_product(int	product_index)	{
				int	n_products;
				DPC_product	*selected_product;

				n_products	=	(int)product_list.size();

				if	((product_index	>=	0)	&&	(product_index	<	n_products))	{
								int	page_ix,	table_ix;
								int	n_pages,	n_tables;
								selected_product_index	=	product_index;
								selected_page_index	=	-1;
								selected_table_index	=	-1;
								selected_product	=	product_list[selected_product_index];
								//	Raise	all	of	the	windows	associated	with	this	product.
								selected_product->notify(	view,	fermi_view::RAISE_WINDOW	);

								XmListDeleteAllItems(	page_list_widget);

								n_pages	=	selected_product->NumberOfPages();
								for	(page_ix=0;	page_ix<n_pages;	page_ix++	)	{
												DPC_page	*page;
												char	*temp_str;
												const	char	*const_temp_str;
												XmString	temp_xm_str;

												page	=	selected_product->getPage(	page_ix);
												if	((const_temp_str	=	page->getTitle())	==	NULL)	{
																char	charbuf[256];
																snprintf(	charbuf,	sizeof(charbuf),	"Page_%d",	page_ix);
																temp_str	=	strdup(	charbuf);
												}	else	{
																temp_str	=	strdup(	const_temp_str	);
												}
												temp_xm_str	=	XmStringCreateLocalized(	temp_str	);
												free(temp_str);
												XmListAddItem(page_list_widget,	temp_xm_str,	0);
												XmStringFree(temp_xm_str);
								}

								XmListDeleteAllItems(	table_list_widget);

								n_tables	=	selected_product->NumberOfTables();
								for	(table_ix=0;	table_ix<n_tables;	table_ix++	)	{
												DPC_table	*table;
												char	*temp_str;
												const	char	*const_temp_str;
												XmString	temp_xm_str;

												table	=	selected_product->getTable(	table_ix);
												if	((const_temp_str	=	table->getTitle())	==	NULL)	{
																char	charbuf[256];
																snprintf(	charbuf,	sizeof(charbuf),	"Table_%d",	table_ix);
																temp_str	=	strdup(	charbuf);
												}	else	{
																temp_str	=	strdup(	const_temp_str	);
												}
												temp_xm_str	=	XmStringCreateLocalized(	temp_str	);
												free(temp_str);
												XmListAddItem(table_list_widget,	temp_xm_str,	0);
												XmStringFree(temp_xm_str);
								}
				}
}

void	terminate_app_callback(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				run_app	=	0;
}

void	select_product_callback(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				int	product_index;
				XmListCallbackStruct	*cbs	=	(XmListCallbackStruct	*)call_data;
				product_index	=	cbs->item_position	-	1;
				select_product(	product_index);
}

void	select_page_callback(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				int	page_index;
				XmListCallbackStruct	*cbs	=	(XmListCallbackStruct	*)call_data;
				page_index	=	cbs->item_position	-	1;
				select_page(	page_index);
}

void	select_table_callback(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				int	table_index;
				XmListCallbackStruct	*cbs	=	(XmListCallbackStruct	*)call_data;
				table_index	=	cbs->item_position	-	1;
				select_table(	table_index);
}

void	pdf_all(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

	char	tmp_file[256];
				int	ii;

				ProductViewNode	*p_node;
				int	n_products	=	(int)x_product_list.size();

				if	(!access("/tmp",	W_OK))	{
								strncpy(tmp_file,	"/tmp/fxplot_XXXXXX",	sizeof(tmp_file));
				}	else	if	(!access("/var/tmp",	W_OK))	{
					strncpy(tmp_file,	"/var/tmp/fxplot_XXXXXX",	sizeof(tmp_file));
				}	else	{
								post_dialog(	toplevel,	XmDIALOG_ERROR,
																		"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
																		"Postscript	file	(that	represents	your	print	out)	needs\n"
																		"to	be	generated.	Please	check	your	file	permissions.\n");
								return;
				}

				if	(mkstemp(	tmp_file)	<	0)	{
					post_dialog(	toplevel,	XmDIALOG_ERROR,	"Unable	to	generate	a	temporary	file	name	for	some	mind	boggling	reason."	);
					return;
				}

				for	(ii	=	0;	ii	<	n_products;	ii++)	{
					p_node	=	x_product_list[ii];
					p_node->generate_PS(tmp_file);
				}

				//	Changed	back	use	ghostscript
				int	ret	=	system("gs	-dQUIET	-dNOPAUSE	-sDEVICE=pdfwrite	-dBATCH	-sOutputFile=fxplot.PDF	/tmp/fxplot_*");

				if	(ret	!=	0)	{
					std::cout	<<	"Couldn't	find	utility	ghostscript.	Can't	make	a	PDF	of	all	plots!\n";
				}	else	{
//	show	the	generated	PDF	file
#if	__APPLE__
				ret	=	system("open	-a	Preview	fxplot.PDF	&");
#else
				//	check	to	see	if	xdg-open	is	available
				ret	=	system("xdg-open	--version	>	/dev/null	2>&1");
				if	(ret	==	0)	{
								//	xdg-open	opens	the	file	using	the	default	associated	application
								system("xdg-open	fxplot.PDF	&");
				}	else	{
								std::cout	<<	"Couldn't	find	a	proper	application	to	view	the	PDF	file.\nThe	PDF	file	of	all	plots	is	saved	as	fxplot.PDF!\n";
				}
								/*if	(ret	!=	0)	{
						ret	=	system("acroread	fxplot.PDF	&");
						if	(ret	!=	0)	{
							ret	=	system("xpdf	fxplot.PDF	&");
							if	(ret	!=	0)	{
								ret	=	system("kghostview	fxplot.PDF	&");
								if	(ret	!=	0)	{
									ret	=	system("kpdf	fxplot.PDF	&");
									std::cout	<<	"Couldn't	find	a	proper	application	to	view	the	PDF	file.\nThe	PDF	file	of	all	plots	is	saved	as	fxplot.PDF!\n";
								}
							}
						}
					}*/
#endif
								system("rm	-rf	/tmp/fxplot_*");
				}
}

void	pdf_confirm(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				int	nargs;
				Arg	wargs[1];
				Widget	w_pdf_confirm;
				Widget	w_help_button;

				nargs	=	0;

				XtSetArg(wargs[nargs],	XmNmessageString,	XmStringCreateLocalized((char	*)"Make	PDF	of	all	plots?"));

				nargs++;

				w_pdf_confirm	=	XmCreateQuestionDialog(toplevel,	(char	*)"dialog_confirmation",	wargs,	nargs);

				XtAddCallback(w_pdf_confirm,	XmNokCallback,	pdf_all,	NULL);

				XtManageChild(w_pdf_confirm);

				w_help_button	=	XmMessageBoxGetChild(w_pdf_confirm,	XmDIALOG_HELP_BUTTON);

				XtUnmanageChild(w_help_button);
}

void	print_all(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				int	ii,	jj;
				ProductViewNode	*p_node;
				int	n_products	=	(int)x_product_list.size();

				for	(ii	=	0;	ii	<	n_products;	ii++)	{
								p_node	=	x_product_list[ii];
								int	n_pages	=	(int)p_node->page_node_list.size();

								//	prints	each	page
								for	(jj	=	0;	jj	<	n_pages;	jj++)	{
												p_node->page_node_list[jj]->print();
								}
				}
}

void	print_confirm(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				int	nargs;
				Arg	wargs[1];
				Widget	w_print_confirm;
				Widget	w_help_button;

				nargs	=	0;
				XtSetArg(wargs[nargs],	XmNmessageString,	XmStringCreateLocalized((char	*)"Print	-all-	plots?"));
				nargs++;

				w_print_confirm	=	XmCreateQuestionDialog(toplevel,	(char	*)"dialog_confirmation",	wargs,	nargs);

				XtAddCallback(w_print_confirm,	XmNokCallback,	print_all,	NULL);

				XtManageChild(w_print_confirm);

				w_help_button	=	XmMessageBoxGetChild(w_print_confirm,	XmDIALOG_HELP_BUTTON);
				XtUnmanageChild(w_help_button);
}


/*	===========	*/
Widget	Create_Main_Window(	Widget	top)	{

				Widget	menu_bar;
				Widget	file_pull_down;
				XmString	label_str;
				Widget	exit_button;
				Widget	form_widget;

				Widget	label_widget_1;
				Widget	label_widget_2;
				Widget	label_widget_3;
				Widget	scrolled_w_widget_1;
				Widget	scrolled_w_widget_2;
				Widget	scrolled_w_widget_3;


				main_window	=	XtVaCreateManagedWidget("main_w",	xmMainWindowWidgetClass,	top,
																																										NULL);
				menu_bar	=	XmCreateMenuBar(main_window,	(char	*)"Menu",	NULL,	0);

				file_pull_down	=	XmCreatePulldownMenu(	menu_bar,	(char	*)"file_pull_down"	,	NULL,	0);

				label_str=	XmStringCreateLocalized((char	*)"File");

				XtVaCreateManagedWidget("file",	xmCascadeButtonWidgetClass,	menu_bar,
																												XmNlabelString,	label_str,
																												XmNsubMenuId,	file_pull_down,
																												NULL);
				XmStringFree(label_str);
				print_button	=	XtVaCreateManagedWidget("Print",	xmPushButtonGadgetClass,	file_pull_down,	NULL);
				make_pdf_button		=	XtVaCreateManagedWidget("Make	PDF",	xmPushButtonGadgetClass,	file_pull_down,	NULL);
				XtVaCreateManagedWidget("separator",	xmSeparatorGadgetClass,	file_pull_down,	NULL);

				exit_button		=	XtVaCreateManagedWidget("Exit",	xmPushButtonGadgetClass,	file_pull_down,	NULL);

				XtAddCallback(	exit_button,	XmNactivateCallback,	terminate_app_callback,	NULL);

				XtManageChild(	menu_bar);

				form_widget	=
								XtVaCreateWidget(	"topform",
																										xmFormWidgetClass,	main_window,
																										XmNfractionBase,	3,
																										XmNwidth,																	400,
																										XmNheight,																200,
																										NULL);

				label_str=	XmStringCreateLocalized((char	*)"Products");
				label_widget_1	=
								XtVaCreateManagedWidget(	"ProductListLabel",
																																	xmLabelGadgetClass,		form_widget,
																																	XmNlabelString,						label_str	,
																																	XmNtopAttachment,				XmATTACH_FORM,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					0,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				1,
																																	XmNbottomAttachment,	XmATTACH_NONE,
																																	NULL);
				XmStringFree(label_str);

				label_str=	XmStringCreateLocalized((char	*)"Pages");
				label_widget_2	=
								XtVaCreateManagedWidget(	"PageListLabel",
																																	xmLabelGadgetClass,		form_widget,
																																	XmNlabelString,						label_str	,
																																	XmNtopAttachment,				XmATTACH_FORM,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					1,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				2,
																																	XmNbottomAttachment,	XmATTACH_NONE,
																																	NULL);
				XmStringFree(label_str);

				label_str=	XmStringCreateLocalized((char	*)"Tables");
				label_widget_3	=
								XtVaCreateManagedWidget(	"TableListLabel",
																																	xmLabelGadgetClass,		form_widget,
																																	XmNlabelString,						label_str	,
																																	XmNtopAttachment,				XmATTACH_FORM,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					2,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				3,
																																	XmNbottomAttachment,	XmATTACH_NONE,
																																	NULL);
				XmStringFree(label_str);

				scrolled_w_widget_1	=
								XtVaCreateManagedWidget(	"scrolled_w_1",
																																	xmScrolledWindowWidgetClass,	form_widget,
																																	XmNscrollingPolicy,		XmAUTOMATIC,
																																	XmNtopAttachment,				XmATTACH_WIDGET,
																																	XmNtopWidget,								label_widget_1,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					0,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				1,
																																	XmNbottomAttachment,	XmATTACH_FORM,
																																	NULL);
				scrolled_w_widget_2	=
								XtVaCreateManagedWidget(	"scrolled_w_2",
																																	xmScrolledWindowWidgetClass,	form_widget,
																																	XmNscrollingPolicy,		XmAUTOMATIC,
																																	XmNtopAttachment,				XmATTACH_WIDGET,
																																	XmNtopWidget,								label_widget_2,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					1,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				2,
																																	XmNbottomAttachment,	XmATTACH_FORM,
																																	NULL);
				scrolled_w_widget_3	=
								XtVaCreateManagedWidget(	"scrolled_w_3",
																																	xmScrolledWindowWidgetClass,	form_widget,
																																	XmNscrollingPolicy,		XmAUTOMATIC,
																																	XmNtopAttachment,				XmATTACH_WIDGET,
																																	XmNtopWidget,								label_widget_3,
																																	XmNleftAttachment,			XmATTACH_POSITION,
																																	XmNleftPosition,					2,
																																	XmNrightAttachment,		XmATTACH_POSITION,
																																	XmNrightPosition,				3,
																																	XmNbottomAttachment,	XmATTACH_FORM,
																																	NULL);

				product_list_widget	=
								XtVaCreateManagedWidget(	"list_of_products",
																																	xmListWidgetClass,	scrolled_w_widget_1,
																																	XmNselectionPolicy,	XmSINGLE_SELECT,
																																	XmNheight,	500,
																																	NULL);

				XtAddCallback(	product_list_widget,	XmNdefaultActionCallback,	select_product_callback,	NULL);
				XtAddCallback(	product_list_widget,	XmNsingleSelectionCallback,	select_product_callback,	NULL);

				page_list_widget	=
								XtVaCreateManagedWidget(	"list_of_pages",
																																	xmListWidgetClass,	scrolled_w_widget_2,
																																	XmNselectionPolicy,	XmSINGLE_SELECT,
																																	XmNheight,	500,
																																	NULL);

				XtAddCallback(	page_list_widget,	XmNdefaultActionCallback,	select_page_callback,	NULL);
				XtAddCallback(	page_list_widget,	XmNsingleSelectionCallback,	select_page_callback,	NULL);

				table_list_widget	=
								XtVaCreateManagedWidget(	"list_of_tables",
																																	xmListWidgetClass,	scrolled_w_widget_3,
																																	XmNselectionPolicy,	XmSINGLE_SELECT,
																																	XmNheight,	500,
																																	NULL);

				XtAddCallback(	table_list_widget,	XmNdefaultActionCallback,	select_table_callback,	NULL);
				XtAddCallback(	table_list_widget,	XmNsingleSelectionCallback,	select_table_callback,	NULL);

				XtManageChild(form_widget);

				return(	main_window);

}

class	FplotArgs	{
				public:
				const	char*	presentation;
				char*	device;
				const	char*	session_file;

				FplotArgs(int	argc,	char*	argv[])	{
								int	ii;
								char*	cp	;

								device	=	NULL;
								session_file	=	NULL;

								for	(	ii	=	1	;	ii	<	argc	;	ii++	)	{
												if	(	!	strncmp(	"-device",	argv[ii],	7	)	)	{
																cp	=	strchr(	argv[ii],	'='	)	;
																cp++	;
																device	=	strdup(cp);

												}	else	{
																session_file	=	argv[ii];
																if	(	access(	session_file,	R_OK|F_OK	)	)	{
																				if	(session_file	==	NULL)	{
																								throw	std::invalid_argument("Can't	find/read	session	file.");
																				}
																}
												}
								}
								if	(session_file	==	NULL)	{
												throw	std::invalid_argument("No	session	file	specified.");
								}

				}

				private:
							FplotArgs()	{};
};

void	usage()
{
				std::cout	<<	"USAGE:"	<<	std::endl
														<<	"								fxplot	[-device=terminal|printer|file:file_name]	<session	file>"	<<	std::endl	;
				std::cout.flush();
}

int	main(int	argc,	char*	argv[]	)	{

				FplotArgs*	fplot_args;

				XEvent	event;

				XtAppContext	app;

				DPM_parse_tree	*session_parse_tree;
				DPM_session	*session;
				xmlNode	*root_node;

				int	n_product_files;
				int	n_products;
				int	product_ix;

				const	char	*const_temp_str;

				try	{
								fplot_args	=	new	FplotArgs(argc,	argv);
				}	catch	(std::invalid_argument)	{
								usage();
								exit(1);
				}

				XtSetLanguageProc	(NULL,	NULL,	NULL);

				toplevel	=	XtVaAppInitialize	(&app,	"Fxplot",	NULL,	0,	&argc,	argv,	NULL,	NULL	);

				main_window	=	Create_Main_Window(	toplevel);

				view	=	new	fermi_view(toplevel);

				//	============================================
				//	Create	the	DPM_session	object.
				//	============================================

				try	{
								session_parse_tree	=	new	DPM_parse_tree(	fplot_args->session_file);
				}	catch	(std::invalid_argument)	{
								std::cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	std::endl;
								exit(1);
				}

				root_node	=	session_parse_tree->getRootNode();

				try	{
								session	=	new	DPM_session(	NULL,	root_node);
				}	catch	(std::invalid_argument)	{
								std::cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	std::endl;
								exit(1);
				}

				delete	session_parse_tree;

				//	============================================
				//	Create	the	data	products	listed	in	the	DPM_session.
				//	============================================
				n_product_files	=	(int)session->product_file_list.size();

				for	(product_ix	=	0;	product_ix	<	n_product_files;	product_ix++	)	{
								DPC_product	*product;
								char	product_file_fullname[1024];

								const	char	*product_file_name	=	session->product_file_list[product_ix];

								if	(file_exists(	product_file_name))	{
													strcpy(product_file_fullname,	product_file_name	);
								}	else	{
													//	Otherwise	assume	that	the	product	file	location	is
													//	relative	to	that	of	the	session	file.
													copy_prefix(product_file_fullname,	fplot_args->session_file);
													strcat(product_file_fullname,	"/");
													strcat(product_file_fullname,	product_file_name);
								}

								try	{
												product	=	new	DPC_product(	session,	product_file_fullname);
								}	catch		(std::invalid_argument)	{
												std::cerr	<<	"ERROR:	Unable	to	create	the	product	specified	by	file:	\""	<<
																product_file_name	<<	"\""	<<	std::endl;
												product	=	NULL;
								}

								if	(product	!=	NULL)	{
												char	*temp_str;
												XmString	product_xm_str;

												product_list.push_back(	product);

												ProductViewNode*	X_Product	=	(ProductViewNode*)product->render(	view);
												x_product_list.push_back(X_Product);

												if	((const_temp_str	=	product->getTitle())	==	NULL)	{
																char	charbuf[256];
																char	*p;
																snprintf(	charbuf,	sizeof(charbuf),	"%s",	product_file_name);
																if	((p=(char	*)strrchr(	product_file_name,'/'))	!=	NULL)	{
																				strncpy(charbuf,	p+1,	sizeof(charbuf)-(size_t)1);
																}	else	{
																				strncpy(charbuf,	product_file_name,	sizeof(charbuf)-(size_t)1);
																}
																temp_str	=	strdup(	charbuf);
												}	else	{
																temp_str	=	strdup(	const_temp_str	);
												}
												product_xm_str	=	XmStringCreateLocalized(	temp_str	);
												free(temp_str);
												XmListAddItem(product_list_widget,	product_xm_str,	0);
								}
				}

				if	(fplot_args->device)	{
								session->setDevice(fplot_args->device);
				}

				if	(	session->getDevice()	==	DPM_session::PRINTER	||	session->getDevice()	==	DPM_session::FILE)	{

								n_products	=	(int)x_product_list.size();
								for	(product_ix	=	0;	product_ix	<	n_products;	product_ix++	)	{
												ProductViewNode	*x_product;
												x_product	=	x_product_list[product_ix];
												if	(session->getDevice()	==	DPM_session::PRINTER)	{
																x_product->print();
												}	else	{
																x_product->generate_PS(session->getDeviceFileName());
												}
								}

				}	else	{
								//	Make	the	main	app	window	visible.
								XtRealizeWidget(toplevel);
								XRaiseWindow(	XtDisplay(main_window),XtWindow(main_window));

								bool	pages_exist	=	false;

								//	Make	the	pages	and	tables	of	each	product	visible.
								n_products	=	(int)product_list.size();
								for	(product_ix	=	0;	product_ix	<	n_products;	product_ix++	)	{
												DPC_product	*product;
												product	=	product_list[product_ix];
												product->notify(	view,	fermi_view::SHOW_WINDOW	);
												if	(product->NumberOfPages()	>	0)	{
													pages_exist	=	true;
												}
								}

								if	(pages_exist)	{
									//	if	there	are	plots,	add	the	callback	for	the	button
									XtAddCallback(make_pdf_button,	XmNactivateCallback,	pdf_confirm,	NULL);
									XtAddCallback(print_button,	XmNactivateCallback,	print_confirm,	NULL);
								}	else	{
									//	if	there	is	no	plot,	don't	manage	this	as	it's	only	for	plots	not	tables
									XtUnmanageChild(make_pdf_button);
									XtUnmanageChild(print_button);
								}

								while	(run_app)	{
												XtAppNextEvent(app,	&event);
												XtDispatchEvent(&event);
								}
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

				//	Since	each	ProductViewNode	of	x_product_list	is	not	created	by	new	here,
				//	only	clearing	the	vector	and	underlying	destructors	should	be	called.
				x_product_list.clear();

				return	0;
}

