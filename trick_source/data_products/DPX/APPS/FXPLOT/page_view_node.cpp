
#include	<Xm/Xm.h>
#include	<Xm/DialogS.h>
#include	<Xm/Form.h>
#include	<Xm/PushB.h>
#include	<Xm/ToggleBG.h>
#include	<Xm/LabelG.h>
#include	<Xm/FileSB.h>
#include	<Xm/SelectioB.h>
#include	<Xm/MessageB.h>

#include	<iostream>
#include	<string>

#include	<stdio.h>				//	for	snprintf
#include	<stdlib.h>

#include	"page_view_node.hh"
#include	"post_dialog.h"

//	FRACTION_BASE	is	used	by	form	widgets.	I	chose	2520	because
//	it	is	evenly	divisable	by	10,9,8,7,6,5,4,3	and	2.
#define	FRACTION_BASE	2520

//	CALLBACK
static	void	print_page_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				PageViewNode*	page_view_node	=	(PageViewNode	*)client_data;
				page_view_node->print();

}

static	void	layout_page	(PageViewNode*	page_view_node	)	{

			int	i,j;
			int	n_horizontal_cells,	n_vertical_cells;
			int	cell_width,	cell_height;
			int	number_of_plots;
			int	number_of_selected_plots	=	0;
			int	number_of_display_plots	=	0;

			number_of_plots	=	(int)page_view_node->plot_node_list.size();

			//	Count	the	number	of	selected	plots.
			for	(i=0;	i<number_of_plots;	i++)	{
								XtUnmanageChild(	page_view_node->plot_node_list[i]->main_form_widget);
								if	(	page_view_node->plot_node_list[i]->selected	)	{
												number_of_selected_plots	++;
								}
			}

			//	If	the	user	has	not	selected	any	plots
			if	(	number_of_selected_plots	==	0)	{
							//	then	we	want	to	display	all	of	them
							number_of_display_plots	=	number_of_plots;
			}	else	{
							//	otherwise	we	want	to	display	only	the	selected	plots.
							number_of_display_plots	=	number_of_selected_plots;
			}

			page_view_node->get_cell_geometry(	number_of_display_plots,	n_horizontal_cells,	n_vertical_cells);

			cell_width		=	FRACTION_BASE	/	n_horizontal_cells;
			cell_height	=	FRACTION_BASE	/	n_vertical_cells;

			j=0;
			for	(i=0;	i<number_of_plots;	i++)	{
								PlotViewNode*	plot_view_node;
								int	x,y;

								plot_view_node	=	page_view_node->plot_node_list[i];

								if	((number_of_selected_plots	==	0)	||	plot_view_node->selected	)	{
													y	=	(j	/	n_horizontal_cells);
													x	=	(j	%	n_horizontal_cells);

													//	if	odd	number	of	plots	in	2	columns,	span	last	plot	over	both	columns	(like	Trick7)
													int	right_pos;
													right_pos	=	(x+1)	*	cell_width;
													if	(	(n_horizontal_cells==2)	&&	(number_of_plots	%	2)	&&	(j==number_of_plots-1)	)	{
																	right_pos	=	2	*	cell_width;
													}

													XtVaSetValues(	plot_view_node->main_form_widget,
																												XmNtopAttachment,				XmATTACH_POSITION,
																												XmNtopPosition,						y	*	cell_height,
																												XmNleftAttachment,			XmATTACH_POSITION,
																												XmNleftPosition,					x	*	cell_width,
																												XmNrightAttachment,		XmATTACH_POSITION,
																												XmNrightPosition,				right_pos,
																												XmNbottomAttachment,	XmATTACH_POSITION,
																												XmNbottomPosition,			(y+1)	*	cell_height,
																												NULL);

													XtManageChild(	plot_view_node->main_form_widget);
													j++;

													plot_view_node->selected	=	0;
													XmToggleButtonGadgetSetState(	plot_view_node->toggle4,	False,	False);
									}
				}
}

//	CALLBACK
static	void	expand_plot_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				PageViewNode*	page_view_node	=	(PageViewNode	*)client_data;
				layout_page	(	page_view_node);
}

//	CALLBACK
void	PageViewNode::pdf_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

	Widget	dialog	;
	XmString	msg	;
	long	pageIdx	;
	int	nargs	;
	Arg	wargs[5];
	PageViewNode*	page_view_node	=	(PageViewNode	*)client_data;
	//	Make	sure	that	user	has	ps2pdf
	if	(	!	page_view_node->exists_program(	"ps2pdf"	)	)	{

					msg	=	XmStringCreateLocalized((char*)"Couldn't	find	utility	ps2pdf.	Can't	create	pdf	files.");

					nargs	=	0	;
					XtSetArg(wargs[nargs],	XmNmessageString,	msg);
					nargs++	;

					dialog	=	XmCreateErrorDialog(	page_view_node->toplevel,	(char*)"ps2pdf_not_found",	wargs,	nargs	)	;

					XtUnmanageChild(XmMessageBoxGetChild(	dialog,	XmDIALOG_CANCEL_BUTTON));
					XtUnmanageChild(XmMessageBoxGetChild(	dialog,	XmDIALOG_HELP_BUTTON));

					XtManageChild(dialog);

					XmStringFree(msg);

					return	;
				}

	//	Pass	page	index	down	to	selection	pdfFileCB
	XtVaGetValues(w,	XmNuserData,	(XtPointer	*)	&	pageIdx,	NULL);
	nargs	=	0	;
	XtSetArg(wargs[nargs],	XmNuserData,	(XtPointer)pageIdx);
	nargs++	;

	dialog	=	XmCreateFileSelectionDialog(page_view_node->toplevel,	(char*)"filesb",	wargs,	nargs);

	XtAddCallback(dialog,	XmNcancelCallback,	pdf_cancel_cb,	(XtPointer)	client_data);
	XtAddCallback(dialog,	XmNokCallback,	pdf_file_cb,	(XtPointer)	client_data);
	XtSetSensitive(	XmSelectionBoxGetChild(dialog,	XmDIALOG_HELP_BUTTON),	False);
				XtManageChild(dialog);

	return;
}


void	PageViewNode::pdf_cancel_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				XtPopdown(	XtParent(	w	)	)	;
				return	;
}

void	PageViewNode::pdf_file_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				char	tmp_file[256];
				char	cmd[1024];
				char*	file_name	;
				long	long	pageIdx;
				PageViewNode*	page_view_node	=	(PageViewNode	*)client_data;
				XmFileSelectionBoxCallbackStruct	*cbs	=	(XmFileSelectionBoxCallbackStruct	*)	call_data	;

				if	(	!	XmStringGetLtoR(cbs->value,	XmFONTLIST_DEFAULT_TAG,	&file_name)){
								//	Internal	error	(p.182	Volume	6A	in	the	X	Bible!)
								return	;
				}

				if	(	!*file_name	)	{	/*	Nothing	typed	*/
								XtFree(file_name);
								return;
				}

				if	(!access("/tmp",	W_OK))	{
								strncpy(tmp_file,	"/tmp/dpx_postscript_XXXXXX",	sizeof(tmp_file));
				}	else	if	(!access("/var/tmp",	W_OK))	{
								strncpy(tmp_file,	"/var/tmp/dpx_postscript_XXXXXX",	sizeof(tmp_file));
				}	else	{
								post_dialog(	page_view_node->toplevel,	XmDIALOG_ERROR,
																"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
																"Postscript	file	(that	represents	your	print	out)	needs\n"
																"to	be	generated.	Please	check	your	file	permissions.\n");
								return;
				}

				if	(mkstemp(	tmp_file)	<	0)	{
								post_dialog(	page_view_node->toplevel,	XmDIALOG_ERROR,
																"Unable	to	generate	a	temporary	file	name	for	some	mind	boggling	reason."	);
								return;
				}

				//	Get	page	index	to	create	pdf	file	of
				XtVaGetValues(w,	XmNuserData,	(XtPointer	*)	&	pageIdx,	NULL);

				page_view_node->generate_PS(tmp_file);

				//	Convert	PS	tmp	file	to	PDF
				snprintf(cmd,	sizeof(cmd),	"ps2pdf	%s	%s",	tmp_file,	file_name);
				system(cmd);

				XtFree(file_name);

				XtPopdown(	XtParent(	w	)	)	;

				return	;
}

int	PageViewNode::exists_program(const	char	*programName)
{
				char	*getPath;
				char	path[1024];
				char	*tokptr;
				char	*strptr;
				char	*program;

				//	if	string	passed	in	is	null,	bail
				if	(programName	==	NULL)	{
								return	(0);
				}
				//	Search	user	$PATH	for	program
				//	(have	to	do	this	strange	copy	since	getenv
				//	returns	a	pointer	and	strtok	messes	with	environment)
				getPath	=	getenv("PATH");
				strcpy(path,	getPath);

				strptr	=	path;
				while	((tokptr	=	strtok(strptr,	":"))	!=	NULL)	{
								size_t	program_len	=	strlen(tokptr)	+	strlen(programName)	+	2;
								program	=	(char	*)	malloc(	program_len);
								snprintf(program,	program_len,	"%s/%s",	tokptr,	programName);
								if	(access(program,	F_OK	|	X_OK)	==	0)	{
												free(program);
												return	(1);
								}
								free(program);
								strptr	=	NULL;
				}

				//	Found	with	out	any	need	for	$PATH
				if	(access(programName,	F_OK	|	X_OK)	==	0)	{
								return	(1);
				}
				//	Not	found
				return	(0);
}

//	CLASS	VARIABLE	INITIALIZATION
int	PageViewNode::instance_count	=	0;


//	DESTRUCTOR
PageViewNode::~PageViewNode()	{

				PlotViewNode	*plot_view_node;
				int	n,i;

				#ifdef	DEBUG
				std::cout	<<	"PageViewNode	DESTRUCTOR."	<<	std::endl;
				#endif

				n	=	(int)plot_node_list.size();
				for	(i=0	;	i<n;	i++	)	{
								plot_view_node	=	plot_node_list[i];
								if	(plot_view_node)	{	delete	plot_view_node;	}
				}

				XtDestroyWidget(plots_form_widget);
				XtDestroyWidget(trick_label);
				XtDestroyWidget(expand_plot_button_widget);
				XtDestroyWidget(print_button_widget);
				XtDestroyWidget(pdf_button_widget);
				XtDestroyWidget(ctrls_form_widget);
				XtDestroyWidget(main_form_widget);
				XtDestroyWidget(dialog_shell_widget);

}


//	CONSTRUCTOR
PageViewNode::PageViewNode(Widget	Toplevel,	DPC_page*	Page,	int	Xpos,	int	Ypos,	int	Width,	int	Height)	{

				Arg	wargs[50];
				int	nargs;

				const	char	*page_title;

				char	charbuf[256];
				char	default_page_title[256];

				const	char	*ncells_s;
				Pixel	fg,bg;
				Pixmap	pixmap;

				page	=	Page;
				toplevel	=	Toplevel;

				if	((ncells_s	=	Page->getAttribute("hcells"))	!=	NULL)	{
								n_hcells	=	atoi(ncells_s);
								if	((n_hcells	<	0)||(n_hcells	>	10))	{
								post_dialog(	toplevel,	XmDIALOG_ERROR,
												"Value	for	hcells	attribute	is	out	of	range.\n"
												"Valid	range	for	hcells	is	1	..	10.\n"
												"Ignoring	hcells	value.\n"	);
												n_hcells	=	1;
								}
				}	else	{
								n_hcells	=	1;
				}

				if	((ncells_s	=	Page->getAttribute("vcells"))	!=	NULL)	{
								n_vcells	=	atoi(ncells_s);
								if	((n_vcells	<	0)||(n_vcells	>	10))	{
								post_dialog(	toplevel,	XmDIALOG_ERROR,
												"Value	for	vcells	attribute	is	out	of	range.\n"
												"Valid	range	for	vcells	is	1	..	10.\n"
												"Ignoring	vcells	value.\n");
												n_vcells	=	1;
								}
				}	else	{
								n_vcells	=	1;
				}

				if	((page_title	=	Page->getTitle())	==	NULL	)	{
								snprintf(	default_page_title,	sizeof(default_page_title),	"Page	%d",	instance_count);
								page_title	=	default_page_title;
				}
				title	=	strdup(page_title);

				//	---------------------------
				//	Create	the	page	widget	name.
				//	---------------------------
				snprintf(	charbuf,	sizeof(charbuf),	"Page_dialog_%d",	instance_count);

				//	--------------------------------------
				//	Create	the	dialog	shell	for	this	page.
				//	--------------------------------------
				nargs	=	0;
				XtSetArg(	wargs[nargs],	XmNtitle,	page_title	);	nargs++;
				XtSetArg(	wargs[nargs],	XmNmappedWhenManaged,	False	);	nargs++;
				dialog_shell_widget	=	XmCreateDialogShell(	toplevel,	charbuf,	wargs,	nargs);

				//	----------------------------------------------
				//	Create	the	form	manager	widget	for	this	page.
				//	----------------------------------------------
				main_form_widget	=	XtVaCreateWidget(	"PageForm",	xmFormWidgetClass,	dialog_shell_widget,
																																				XmNresizePolicy,	XmRESIZE_GROW,
																																				XmNfractionBase,	10,
																																				NULL);

				ctrls_form_widget	=	XtVaCreateManagedWidget(	"CtrlsForm",	xmFormWidgetClass,	main_form_widget,
																																												XmNresizePolicy,					XmRESIZE_GROW,
																																												XmNfractionBase,					10,
																																												XmNtopAttachment,				XmATTACH_FORM,
																																												XmNleftAttachment,			XmATTACH_FORM,
																																												XmNrightAttachment,		XmATTACH_FORM,
																																												XmNbottomAttachment,	XmATTACH_NONE,
																																												NULL);

				XtVaGetValues	(ctrls_form_widget,	XmNforeground,	&fg,	XmNbackground,	&bg,	NULL);

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"print_page.xpm",	fg,	bg);

				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								print_button_widget	=	XtVaCreateManagedWidget(	"PrintPage",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNleftAttachment,			XmATTACH_FORM,
																																																NULL);
								XtVaSetValues(print_button_widget,
																						XmNheight,	34,
																						NULL);
				}	else	{
								print_button_widget	=	XtVaCreateManagedWidget(	"PrintPage",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNleftAttachment,			XmATTACH_FORM,
																																																XmNlabelType,								XmPIXMAP,
																																																XmNlabelPixmap,						pixmap,
																																																NULL);
				}

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"expand.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								expand_plot_button_widget	=	XtVaCreateManagedWidget(	"Expand",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNleftAttachment,			XmATTACH_WIDGET,
																																																XmNleftWidget,							print_button_widget,
																																																NULL);
								XtVaSetValues(expand_plot_button_widget,
																XmNheight,	34,
																NULL);
				}	else	{
								expand_plot_button_widget	=	XtVaCreateManagedWidget(	"Expand",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNleftAttachment,			XmATTACH_WIDGET,
																																																XmNleftWidget,							print_button_widget,
																																																XmNlabelType,								XmPIXMAP,
																																																XmNlabelPixmap,						pixmap,
																																																NULL);
				}

				pdf_button_widget	=	XtVaCreateManagedWidget(	"PDF",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																				XmNleftAttachment,			XmATTACH_WIDGET,
																																																				XmNleftWidget,							expand_plot_button_widget,
																																																				NULL);
				XtVaSetValues(pdf_button_widget,
												XmNwidth,	34,
												XmNheight,	34,
												NULL);


				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"trick_small.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
									trick_label	=	XtVaCreateManagedWidget(	"Trick",	xmLabelGadgetClass,	ctrls_form_widget,
																							XmNrightAttachment,		XmATTACH_FORM,
																							NULL);
				}	else	{
									trick_label	=	XtVaCreateManagedWidget(	"Trick",	xmLabelGadgetClass,	ctrls_form_widget,
																							XmNrightAttachment,		XmATTACH_FORM,
																							XmNlabelType,								XmPIXMAP,
																							XmNlabelPixmap,						pixmap,
																							NULL);
				}

				XtManageChild(ctrls_form_widget);

				plots_form_widget	=	XtVaCreateManagedWidget(	"PlotsForm",	xmFormWidgetClass,	main_form_widget,
																																												XmNresizePolicy,					XmRESIZE_GROW,
																																												XmNfractionBase,					FRACTION_BASE,
																																												XmNtopAttachment,				XmATTACH_WIDGET,
																																												XmNtopWidget,								ctrls_form_widget,
																																												XmNleftAttachment,			XmATTACH_FORM,
																																												XmNrightAttachment,		XmATTACH_FORM,
																																												XmNbottomAttachment,	XmATTACH_FORM,
																																												NULL);

				XtManageChild(main_form_widget);

				XtAddCallback(	print_button_widget,	XmNactivateCallback,	print_page_cb,	(XtPointer)this);
				XtAddCallback(	expand_plot_button_widget,	XmNactivateCallback,	expand_plot_cb,	(XtPointer)this);
				XtAddCallback(	pdf_button_widget,	XmNactivateCallback,	pdf_cb,	(XtPointer)this);

				XtVaSetValues(dialog_shell_widget,
																		XmNx,							Xpos,
																		XmNy,							Ypos,
																		XmNwidth,			Width,
																		XmNheight,		Height,
																		NULL);

				instance_count	++;
}

void	PageViewNode::finalize()	{

				layout_page	(	this);
}

void	PageViewNode::print()	{

				char	ps_file_name[256];
				char	system_cmd[256];
				char	*trick_print_cmd;
				char	*trick_printer_name;

				if	((trick_print_cmd	=	getenv("TRICK_PRINT_CMD"))	==	NULL)	{
								trick_print_cmd	=	(char	*)"lpr";
				}
				trick_printer_name	=	getenv("TRICK_PRINTER_NAME");

				if	(!access("/tmp",	W_OK))	{
								strncpy(ps_file_name,	"/tmp/dpx_postscript_XXXXXX",	sizeof(ps_file_name));
				}	else	if	(!access("/var/tmp",	W_OK))	{
								strncpy(ps_file_name,	"/var/tmp/dpx_postscript_XXXXXX",	sizeof(ps_file_name));
				}	else	{
								post_dialog(	toplevel,	XmDIALOG_ERROR,
																"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
																"Postscript	file	(that	represents	your	print	out)	needs\n"
																"to	be	generated.	Please	check	your	file	permissions.\n");
								return;
				}

				if	(mkstemp(	ps_file_name)	<	0)	{
								post_dialog(	toplevel,	XmDIALOG_ERROR,
																"Unable	to	generate	a	temporary	file	name	for	some	mind	boggling	reason."	);
								return;
				}

				//	Generate	Postscript
				generate_PS(	ps_file_name);

				if	(!strcmp(trick_print_cmd,	"lpr"))	{
								if	((trick_printer_name	==	NULL)	||	(strlen(trick_printer_name)	==	0))	{
												snprintf(system_cmd,	sizeof(system_cmd),	"%s	%s",	trick_print_cmd,	ps_file_name);
								}	else	{
												snprintf(system_cmd,	sizeof(system_cmd),	"%s	-P	%s	%s",	trick_print_cmd,	trick_printer_name,	ps_file_name);
								}
				}	else	{
								snprintf(system_cmd,	sizeof(system_cmd),	"%s	%s",	trick_print_cmd,	ps_file_name);
				}

				std::cout	<<	"Printing	with	command:	"	<<	system_cmd	<<	std::endl;
				system(	system_cmd);

}

void	PageViewNode::generate_PS(const	char*	ps_file_name)	{

				//	Generate	Postscript
				int	i,	n_plots	;
				int	n_horizontal_cells,	n_vertical_cells;
				Widget	*xy_widget_list;
				n_plots	=	(int)plot_node_list.size();

				xy_widget_list	=	(Widget	*)calloc	((size_t)n_plots,	sizeof(Widget));

				for	(i=0;	i<n_plots;	i++)	{
								PlotViewNode	*plot_view_node	=	plot_node_list[i];
								xy_widget_list[i]	=	plot_view_node->xy_widget;
				}

				get_cell_geometry(n_plots,	n_horizontal_cells,	n_vertical_cells);

				char	unconst_filename[256];
				strncpy(	unconst_filename,	ps_file_name,	sizeof(unconst_filename));

				XYPrintMultiple(	xy_widget_list,				/*	XY	widgets	*/
																					0,																	/*	landscape	*/
																					n_plots,											/*	number	of	plots	*/
																					n_vertical_cells,		/*	number	of	rows	*/
																					n_horizontal_cells,/*	number	of	columns	*/
																					title,													/*	page	title	*/
																					(char	*)"",								/*	page	subtitle	*/
																					False,													/*	DestroyOldfile	*/
																					unconst_filename);	/*	Postscript	file	name	*/

				free(xy_widget_list);
}

void	PageViewNode::get_cell_geometry(	int		number_of_display_plots,
																																						int&	n_horizontal_cells,
																																						int&	n_vertical_cells	)	{

			//	Set	the	table	dimensions	based	on	hcells	and	vcells
			n_horizontal_cells	=	n_hcells;
			n_vertical_cells			=	n_vcells;

			//	If	less	than	9	plots,	arrange	them	the	same	way	Trick7	did	it
			//	if	ncells	and	vcells	does	not	equal	the	number	of	plots
			if	(number_of_display_plots	>	(n_horizontal_cells	*	n_vertical_cells)	)	{
							if	(number_of_display_plots	<	9)	{
											if	(number_of_display_plots==3)	{
															n_horizontal_cells	=	1;
															n_vertical_cells	=	3;
											}	else	if	(number_of_display_plots==1)	{
															n_vertical_cells	=	1;
															n_horizontal_cells	=	1;
											}	else	{
															n_vertical_cells	=	2;
															n_horizontal_cells	=	(number_of_display_plots	/	2)	+	(number_of_display_plots	%	2);
											}
							}	else	while((n_horizontal_cells	*	n_vertical_cells)	<	number_of_display_plots	)	{
											if	(n_horizontal_cells	<=	n_vertical_cells)	{
															n_horizontal_cells++;
											}	else	{
															n_vertical_cells++;
											}
							}
			}
}
