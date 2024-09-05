
#include	<unistd.h>
#include	<stdio.h>
#include	<stdarg.h>
#include	<time.h>

//	message_publish	when	you	don't	have	a	message	publisher	class
#define	MAX_MSG_HEADER_SIZE	256
#define	MAX_MSG_SIZE	20480
extern	"C"	int	message_publish_standalone(int	level,	const	char	*	format_msg,	...)	{

				char	msg_buf[MAX_MSG_SIZE];
				char	date_buf[MAX_MSG_HEADER_SIZE];
				char	hostname[64];
				time_t	date	;
				va_list	args;

				va_start(args,	format_msg);
				(void)	vsnprintf(msg_buf,	MAX_MSG_SIZE,	format_msg,	args);
				va_end(args);

				date	=	time(NULL)	;
				strftime(date_buf,	(size_t)	20,	"%Y/%m/%d,%H:%M:%S",	localtime(&date));
				(void)	gethostname(hostname,	(size_t)	48);
				fprintf(stdout,	"|L	%d|%s|		|%s|T	%d|%.2f|	%s"	,	level,
												//	so	that	we	don't	call	any	exec	routines,	use	process	id	0	and	sim	time	0.0
												date_buf,	hostname	,	0,	0.0,	msg_buf)	;
				fflush(stdout)	;

				return	(0);
}
