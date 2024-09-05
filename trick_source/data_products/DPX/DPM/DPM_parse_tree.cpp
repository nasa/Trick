
#include	<DPM/DPM_parse_tree.hh>
#include	<stdlib.h>

int	DPM_parse_tree::Initialize(const	char	*XMLFileName)	{

			int	rc;
			xmlParserCtxtPtr	parser_context;

			valid	=	0;

			if	(!	XMLFileName)	{
							std::cerr	<<	"ERROR:	XML	file	name	is	NULL."	<<	std::endl;
							std::cerr.flush();
							rc	=	-1;
			}	else	{

							LIBXML_TEST_VERSION

							//	Create	a	parser	context.
							if	((parser_context	=	xmlNewParserCtxt())	==	NULL)	{
											std::cerr	<<	"ERROR:	Out	of	Memory"	<<	std::endl;
											std::cerr.flush();
											exit(0);
							}

							const	char*	xml_catalog_files	=	getenv("XML_CATALOG_FILES");
							if	(xml_catalog_files	!=	NULL)	{
											doc	=	xmlCtxtReadFile(	parser_context,	XMLFileName,	NULL,	XML_PARSE_DTDVALID	);
							}	else	{
											std::cerr	<<	std::endl
																					<<	"The	XML_CATALOG_FILES	environment	variable	is	not	set."	<<	std::endl
																					<<	"So,	\""	<<	XMLFileName	<<	"\"	cannot	be	validated	against	it's	DTD."	<<	std::endl
																					<<	"It	will	therefore	be	parsed	without	validation."	<<	std::endl	<<	std::endl;
											doc	=	xmlCtxtReadFile(	parser_context,	XMLFileName,	NULL,	0	);
							}	

							if	(doc	==	NULL)	{
											std::cerr	<<	"ERROR:	Parse	of	XML	file	\""	<<	XMLFileName	<<	"\""	<<	"	failed."	<<	std::endl;
											std::cerr.flush();
											rc	=	-1;
							}	else	if	(parser_context->valid	==	0)	{
											std::cerr	<<	"WARNING:	Validation	of	XML	file	\""	<<	XMLFileName	<<	"\""	<<	"	failed."	<<	std::endl;
											std::cerr.flush();
											rc	=	0;
							}	else	{
											valid	=	1;
											rc	=	0;
							}
							xmlFreeParserCtxt(parser_context);
			}

			return	rc;
}


//	CONSTRUCTOR
DPM_parse_tree::DPM_parse_tree(const	char	*XMLFileName)		{

				if	(	Initialize(	XMLFileName)	<	0)	{
								throw	std::invalid_argument("DPM_parse_tree	is	un-usable.");
				}
}

//	DESTRUCTOR
DPM_parse_tree::~DPM_parse_tree()	{

			if	(doc)	{
							xmlFreeDoc(doc);
			}
}

//	MEMBER	FUNCTION
xmlNode	*DPM_parse_tree::getRootNode()	{

			return	(	xmlDocGetRootElement(doc));
}

//	MEMBER	FUNCTION
int	DPM_parse_tree::is_valid()	{

			return	(	valid);
}







