
#include	"DPV_textbuffer.hh"

#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<iostream>

#define	MAX_MSG_SIZE	0xFFFF

DPV_TextBuffer::DPV_TextBuffer()	{

				text_buf	=	NULL;
				insertion_pos	=	0;
				buf_size	=	0;
}

DPV_TextBuffer::~DPV_TextBuffer()	{
}

void	DPV_TextBuffer::print(const	char*	format,	...)	{

				char	message[MAX_MSG_SIZE];
				va_list	vargs;
				size_t	new_insertion_pos;
				size_t	message_len;

				va_start(vargs,	format);
				vsnprintf(message,	(size_t)MAX_MSG_SIZE,	format,	vargs);
				va_end(vargs);

				message_len	=	strlen(message);
				new_insertion_pos	=	insertion_pos	+	message_len;

				if	(text_buf	==	NULL)	{
								text_buf	=	(char*)calloc(1,	size_t(MAX_MSG_SIZE));
				}

				while	(	insertion_pos	>	buf_size)	{
								buf_size	+=	(size_t)MAX_MSG_SIZE;
								if	((text_buf	=	(char	*)realloc(	text_buf,	buf_size	))	==	NULL)	{
												fprintf(stderr,"OUT_OF_MEMORY	in	DPV_TextBuffer.\n");
												fflush(stderr);
												exit(1);
								}
				}

				strcpy(&text_buf[insertion_pos],	message);
				insertion_pos	=	new_insertion_pos;

}


int	DPV_TextBuffer::subst(	const	char*	search_s,	const	char*	replace_s	)	{

				std::string	haystack	=	text_buf;
				std::string	needle	=	search_s;

				//	If	we	did'nt	find	the	search	string.
				if	(	haystack.find(needle)	==	std::string::npos	)	{
								//	There	is	no	substitution.
								return	(0);
				}

				//	Perform	a	single	substitution.
				haystack.replace(	haystack.find(needle),	needle.length(),	replace_s	);

				if	(	sizeof(haystack)	>	buf_size)	{
								buf_size	+=	(size_t)MAX_MSG_SIZE;
								if	((text_buf	=	(char	*)realloc(	text_buf,	buf_size	))	==	NULL)	{
												fprintf(stderr,"OUT_OF_MEMORY	in	DPV_TextBuffer.\n");
												fflush(stderr);
												exit(1);
								}
				}

				strcpy(text_buf,	haystack.c_str());

				//!	Important!	Needed	by	future	print()	calls.
				insertion_pos	=	haystack.length();

				//	There	was	a	substitution.
				return	(1);
}

void	DPV_TextBuffer::subst_g(	const	char*	search_s,	const	char*	replace_s	)	{

				while	(	subst(	search_s,	replace_s))	;

}

const	char*	DPV_TextBuffer::getText()	{
				return	((const	char*)text_buf);
}

int	DPV_TextBuffer::readFile(	const	char*	fileName	)	{

				FILE	*fp;

				char	*buf;
				int	success	=	0;
				int	failure	=	-1;
				size_t	numRead;
				long	fileSize;

				//!	See	if	file	name	exists,	and	if	writable
				if	(access(fileName,	F_OK)	==	success)	{
								if	(access(fileName,	R_OK)	==	failure)	{
												std::cerr	<<	"Unable	to	access	file	\""	<<	fileName	<<	"\"	for	reading.\n";
												return	(-1);
								}
				}
				//!	Open	it
				fp	=	fopen(fileName,	"r");
				if	(fp	==	NULL)	{
								return	(-1);
				}
				//!	Get	size	of	file
				fseek(fp,	0,	SEEK_END);
				fileSize	=	ftell(fp);
				rewind(fp);

				//!	Read	file	into	buffer
				buf	=	new	char[fileSize	+	1];
				numRead	=	fread((char	*)	buf,	fileSize,	1,	fp);
				if	(numRead	!=	1)	{
								delete[]buf;
								fclose(fp);
								return	(-1);
				}
				buf[fileSize]	=	'\0';

				//!	Close	file
				fclose(fp);

				this->print(buf);
				return	(0);
}

int	DPV_TextBuffer::writeFile(	const	char*	filename	)	{

				FILE	*fp;

				if	((fp	=	fopen(	filename,	"w"))	==	NULL)	{
								std::cerr	<<	"Unable	to	open	file	\""	<<	filename	<<	"\"	for	writing.\n";
								return	(-1);
				}

				fprintf(fp,"%s",	this->getText());
				fclose(fp);
				return	(0);
}

