#include	<stdio.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	"DPM/DPM_product.hh"


int	main(int	argc,	char	*	argv[]	)	{
					xmlParserCtxtPtr	parser_context;
					xmlDocPtr	doc;
					xmlNode	*root_node;
					DPM_product	*product;

					if	(argc	!=	2)	{
										fprintf(stderr,"I	need	a	filename.\n");
										fflush(stderr);
										return	(1);
					}

					LIBXML_TEST_VERSION
					if	((parser_context	=	xmlNewParserCtxt())	==	NULL)	{
										fprintf(stderr,"Out	of	Memory\n");
					}
					doc	=	xmlCtxtReadFile(	parser_context,	argv[1],	NULL,	XML_PARSE_DTDVALID	);
					if	(doc	==	NULL)	{
										fprintf(stderr,"Failed	to	parse	\"%s\"\n",argv[1]);
					}	else	if	(parser_context->valid	==	0)	{
										fprintf(stderr,"Failed	to	validate	\"%s\"\n",argv[1]);
										xmlFreeDoc(doc);
					}
					xmlFreeParserCtxt(parser_context);

					root_node	=	xmlDocGetRootElement(doc);

					if	((product	=	new	DPM_product(	NULL,	root_node))	==	NULL	)	{
										fprintf(stderr,"Failed	to	create	a	DPM_product.\n");
					}

					std::cout	<<	product;

					return	0;
}
