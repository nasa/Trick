
#include	<Xm/Xm.h>
#include	<Xm/DialogS.h>
#include	<Xm/Form.h>
#include	<Xm/PushB.h>
#include	<Xm/LabelG.h>
#include	<Xm/Text.h>
#include	<Xm/FileSB.h>

#include	<iostream>
#include	<string>

#include	<stdio.h>				//	for	snprintf
#include	<string.h>			//	for	strcpy,	strlen,	strdup
#include	<libgen.h>			//	for	dirname,	basename
#include	<unistd.h>			//	for	access
#include	<fcntl.h>				//	for	open,	write,	close

#define	exists(filename)	(!access(filename,	F_OK))

#include	"table_view_node.hh"
#include	"parse_format.h"
#include	"post_dialog.h"

#define	BYTES_PER_MALLOC	0xFFFF

//	CALLBACK
static	void	FSB_cb(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				char	*filename;
				XmFileSelectionBoxCallbackStruct	*cbs	=
								(XmFileSelectionBoxCallbackStruct	*)	call_data;

				TableViewNode*	table_view_node	=	(TableViewNode	*)client_data;

				if	(cbs->reason	==	XmCR_OK)	{

								//	Get	the	filename
								if	(!XmStringGetLtoR	(cbs->value,	XmFONTLIST_DEFAULT_TAG,	&filename))
												return;

								if	(!*filename)	{
												post_dialog(	table_view_node->toplevel,	XmDIALOG_ERROR,	"No	file	selected.");
								}	else	if	(exists	(filename))	{
												post_dialog(	table_view_node->toplevel,	XmDIALOG_ERROR,	"File	already	exists.");
								}	else	{
												int	fd;
												if	((fd	=	creat(filename,	0660))	<	0	)	{
																post_dialog(	table_view_node->toplevel,	XmDIALOG_ERROR,	"Error	occured	attempting	to	create	file.");
												}	else	{
																write(fd,	table_view_node->table_text_buf,	table_view_node->table_buf_size	);
																close(fd);
																XtDestroyWidget(w);
												}
								}
								XtFree(	filename);

				}	else	{
								XtDestroyWidget(w);
				}
}

//	CALLBACK
static	void	save_table_cb(	Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{

				Widget	dialog;

				TableViewNode*	table_view_node	=	(TableViewNode	*)client_data;

				dialog	=	XmCreateFileSelectionDialog(table_view_node->toplevel,	(char	*)"filesb",	NULL,	0	);
				XtAddCallback	(	dialog,	XmNokCallback,	FSB_cb	,	(XtPointer)table_view_node);
				XtAddCallback	(	dialog,	XmNcancelCallback,	FSB_cb	,	(XtPointer)table_view_node);

				XtManageChild(	dialog);

}

static	char*	twprint(	char*	text_buf,	size_t	*text_buf_size,	size_t	*insertion_pos,	char*	format,	...)	{
				char	message[4096];
				va_list	vargs;
				size_t	new_insertion_pos;
				size_t	message_len;

				va_start(vargs,	format);
				vsprintf(message,	format,	vargs);	//	vsnprint	??
				va_end(vargs);

				message_len	=	strlen(message);
				new_insertion_pos	=	*insertion_pos	+	message_len;

				if	(text_buf	==	NULL)	{
								text_buf	=	(char*)calloc(1,	size_t(BYTES_PER_MALLOC));
				}

				while	(	new_insertion_pos	>	*text_buf_size)	{
								*text_buf_size	+=	(size_t)BYTES_PER_MALLOC;
								if	((text_buf	=	(char	*)realloc(	text_buf,	*text_buf_size	))	==	NULL)	{
												std::cerr	<<	"OUT_OF_MEMORY	in	twprint."	<<	std::endl;
												exit(1);
								}
				}

				strcpy(&text_buf[*insertion_pos],	message);
				*insertion_pos	=	new_insertion_pos;

				return	(text_buf);
}

//	CLASS	VARIABLE	INITIALIZATION
int	TableViewNode::instance_count	=	0;

//	CONSTRUCTOR
TableViewNode::TableViewNode(	Widget	Toplevel,	DPC_table*	Table,	int	Xpos,	int	Ypos,	int	Width,	int	Height)	{

				Arg	wargs[50];
				int	nargs;

				int											n_columns;
				int											colix;
				double								*row_values;
				char										**column_heading_format;
				char										**column_data_format;
				int											*column_width;
				int											total_column_width;
				char										*heading_line_string;
				double								time;

				const	char	*	foreground_color;
				const	char	*	background_color;
				Pixel	fg,bg;
				Pixmap	pixmap;

				const	char	*table_title;
				const	char	*run_dir;

				char	charbuf[256];
				char	default_table_title[256];

				const	char	*	var_scale,	*	var_bias;
				double	var_scale_val,	var_bias_val;

				table	=	Table;
				toplevel	=	Toplevel;

				table_text_buf	=	NULL;
				table_insertion_pos	=	0;
				table_buf_size	=	0;

				//	-----------------------------
				//	Create	the	table	widget	name.
				//	-----------------------------

				snprintf(	charbuf,	sizeof(charbuf),	"Table_Dialog_%d",	instance_count);

				//	---------------------------
				//
				//	---------------------------
				if	((table_title	=	table->getTitle())	==	NULL	)	{
								snprintf(	default_table_title,	sizeof(default_table_title),	"Table	%d",	instance_count);
								table_title	=	default_table_title;
				}

				//title	=	strdup(table_title);
				run_dir	=	table->getRunDir();

				//	--------------------------------------
				//	Create	the	dialog	shell	for	this	page.
				//	--------------------------------------
				nargs	=	0;
				XtSetArg(	wargs[nargs],	XmNtitle,	table_title	);	nargs++;
				XtSetArg(	wargs[nargs],	XmNx,	100	);	nargs++;
				XtSetArg(	wargs[nargs],	XmNy,	100	);	nargs++;
				XtSetArg(	wargs[nargs],	XmNmappedWhenManaged,	False	);	nargs++;

				dialog_shell_widget	=	XmCreateDialogShell(	toplevel,	charbuf,	wargs,	nargs);

				//	---------------------------------------------------------------------------------
				//	Create	the	form	manager	widget	for	this	table	whose	parent	is	the	dialog	shell.
				//	---------------------------------------------------------------------------------
				snprintf(	charbuf,	sizeof(charbuf),	"FormForTable%d",	instance_count);

				main_form_widget	=
								XtVaCreateWidget(	charbuf,
																										xmFormWidgetClass,	dialog_shell_widget,
																										NULL);

				XtVaGetValues	(main_form_widget,	XmNforeground,	&fg,	XmNbackground,	&bg,	NULL);

				//	-------------------------------------------------------
				//	Ctrls	form
				//	-------------------------------------------------------

				ctrls_form_widget	=	XtVaCreateManagedWidget(	"CtrlsForm",	xmFormWidgetClass,	main_form_widget,
																																												XmNresizePolicy,					XmRESIZE_GROW,
																																												XmNfractionBase,					10,
																																												XmNtopAttachment,				XmATTACH_FORM,
																																												XmNleftAttachment,			XmATTACH_FORM,
																																												XmNrightAttachment,		XmATTACH_FORM,
																																												XmNbottomAttachment,	XmATTACH_NONE,
																																												NULL);

				XtVaGetValues	(ctrls_form_widget,	XmNforeground,	&fg,	XmNbackground,	&bg,	NULL);


				save_button_widget	=	XtVaCreateManagedWidget(	"Save",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																		XmNleftAttachment,			XmATTACH_FORM,
																																																		NULL);

				XtAddCallback(	save_button_widget,	XmNactivateCallback,	save_table_cb,	(XtPointer)this);


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

				//	-------------------------------------------------------
				//	Text	Widget
				//	-------------------------------------------------------
				nargs	=	0;
				XtSetArg(	wargs[nargs],	XmNeditable,	False	);	nargs++;
				XtSetArg(	wargs[nargs],	XmNeditMode,	XmMULTI_LINE_EDIT);	nargs++;
				XtSetArg(	wargs[nargs],	XmNcursorPositionVisible,	False);	nargs++;

				text_widget	=	XmCreateScrolledText(main_form_widget,	(char	*)"scrolledtext",	wargs,	nargs);

				XtVaSetValues(	XtParent(text_widget),
																			XmNwidth,																	500,
																			XmNheight,																400,
																			XmNtopAttachment,												XmATTACH_WIDGET,
																			XmNtopWidget,																ctrls_form_widget,
																			XmNbottomAttachment,									XmATTACH_FORM,
																			XmNrightAttachment,										XmATTACH_FORM,
																			XmNleftAttachment,											XmATTACH_FORM,
																			NULL);

				XtManageChild	(text_widget);
				XtManageChild	(main_form_widget);


				//	-------------------------------------------------------
				//	Set	the	table's	foreground	color.
				//	-------------------------------------------------------
				//XtVaSetValues(	text_widget,	XmNforeground,	BlackPixel(XtDisplay(toplevel),0),	NULL);
				if	((foreground_color	=	table->getAttribute("foreground_color"))	!=	NULL)	{
								XColor	color_def;
								if	(	XParseColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	foreground_color,	&color_def	))	{
												if	(	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																XtVaSetValues(	text_widget,	XmNforeground,	color_def.pixel,	NULL);
												}
								}
				}

				//	-------------------------------------------------------
				//	Set	the	table's	background	color.
				//	-------------------------------------------------------
				//XtVaSetValues(	text_widget,	XmNbackground,	BlackPixel(XtDisplay(toplevel),0),	NULL);
				if	((background_color	=	table->getAttribute("background_color"))	!=	NULL)	{
								XColor	color_def;
								if	(	XParseColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	background_color,	&color_def	))	{
												if	(	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																XtVaSetValues(	text_widget,	XmNbackground,	color_def.pixel,	NULL);
												}
								}
				}

				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																														(char	*)"Title:	%s\n",	table_title);
				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																														(char	*)"Run:	%s\n",	run_dir);

				n_columns	=	table->getNumColumns();

				column_heading_format	=	(char	**)calloc(	(size_t)n_columns,	sizeof(char*));
				column_data_format	=	(char	**)calloc(	(size_t)n_columns,	sizeof(char*));
				column_width		=	(int	*)calloc(	(size_t)n_columns,	sizeof(int));
				total_column_width	=	0;

				for	(colix=0;	colix	<	n_columns	;	colix++)	{
								int	sign,	width,	precision,	conv_char;
								const	char	*user_format	=	table->getColumnAttribute(	colix,"format");

								//	Determine	whether	the	print	format	string	that	the	user	specified
								//	is	valid.	If	it	is,	we	need	the	width	that	was	specified	in	the	format
								//	so	that	we	can	use	it	for	the	column	headings.	If	the	user	specified
								//	format	is	invalid,	then	we	must	use	a	default.

								if	(	parse_format_string(user_format,	&sign,	&width,	&precision,	&conv_char)	<	0	)	{
												//	User	supplied	format	failed	to	parse,	so	use	the	default.
												column_heading_format[colix]	=	(char	*)"%+18s";
												column_data_format[colix]	=	strdup("%18.6e");
												column_width[colix]	=	18;
								}	else	{
												//	Parse	succeeded.
												char	temp_s[32];
												snprintf(temp_s,	sizeof(temp_s),	"%%+%ds",width);
												column_heading_format[colix]	=	strdup(temp_s);
												column_data_format[colix]	=	strdup(user_format);
												column_width[colix]	=	width;
								}
								total_column_width	+=	column_width[colix];
				}

				heading_line_string	=	(char	*)calloc(	(size_t)total_column_width+1,	sizeof(char));
				memset(heading_line_string,'-',total_column_width);

				//	Print	a	line	of	dashes	over	the	column	headings.
				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,	(char	*)"%s\n",	heading_line_string);

				//	----------------------------------------------------------------
				//	Print	out	the	column	heading	labels.
				//	----------------------------------------------------------------
				for	(colix=0;	colix	<	n_columns	;	colix++)	{
								std::string	column_label	=	table->getColumnLabel(colix);
								if	(column_label.empty())	{
												snprintf(	charbuf,	sizeof(charbuf),	"Column_%d",	colix);
												table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																																						column_heading_format[colix],	charbuf);
								}	else	{
												table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																																						column_heading_format[colix],	column_label.c_str());
								}
				}
				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,	(char	*)"\n");

				//	----------------------------------------------------------------
				//	Print	out	the	column	heading	units
				//	----------------------------------------------------------------
				for	(colix=0;	colix	<	n_columns	;	colix++)	{
								const	char	*col_units	=	table->getColumnUnits(	colix);
								table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																																		column_heading_format[colix],	col_units);
				}
				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,	(char	*)"\n");

				//	Print	a	line	of	dashes	under	the	column	headings.
				table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,	(char	*)"%s\n",	heading_line_string);

				free(heading_line_string);
				//	----------------------------------------------------------------
				//	----------------------------------------------------------------
				row_values	=	new	double[n_columns];

				while	(table->getRow(&time,	row_values))	{

								for	(colix=0;	colix	<	n_columns	;	colix++)	{

									var_scale_val	=	1.0;
									var_bias_val	=	0.0;
									if	((var_scale	=	table->getVarAttribute(colix,	"scale"))	!=	NULL)	{
													var_scale_val	=	strtod(var_scale,	NULL);
									}
									if	((var_bias	=	table->getVarAttribute(colix,	"bias"))	!=	NULL)	{
													var_bias_val	=	strtod(var_bias,	NULL);
									}

												table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,
																																						column_data_format[colix],	(row_values[colix]*var_scale_val	+	var_bias_val));
								}
								table_text_buf	=	twprint(	table_text_buf,	&table_buf_size,	&table_insertion_pos,	(char	*)"\n");
				}

				//	Free	all	the	column	building	arrays.
				free(	column_heading_format);
				free(	column_data_format);
				free(	column_width);

				XtVaSetValues(	text_widget,	XmNvalue,	table_text_buf,	NULL);

				XtVaSetValues(dialog_shell_widget,
																		XmNx,							Xpos,
																		XmNy,							Ypos,
																		XmNwidth,			Width,
																		XmNheight,		Height,
																		NULL);

				instance_count	++;
}

//DESTRUCTOR
TableViewNode::~TableViewNode()	{

				#ifdef	DEBUG
				std::cout	<<	"TableViewNode	DESTRUCTOR."	<<	std::endl;
				#endif

				if	(table_text_buf	!=	NULL)	{
								free(table_text_buf);
				}
				//	FIXME:	figure	out	how	to	destroy	text_widget	without
				//								getting	warning	messages.
				//	XtDestroyWidget(text_widget);

				XtDestroyWidget(trick_label);
				XtDestroyWidget(save_button_widget);
				XtDestroyWidget(ctrls_form_widget);
				XtDestroyWidget(main_form_widget);
				XtDestroyWidget(dialog_shell_widget);
}

void	TableViewNode::finalize()	{
				//	Do	Nothing.
}
