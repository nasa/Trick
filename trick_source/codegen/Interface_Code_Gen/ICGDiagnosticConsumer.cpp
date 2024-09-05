#include	<iostream>
#include	"clang/Basic/SourceManager.h"
#include	"clang/Basic/SourceLocation.h"

#include	"ICGDiagnosticConsumer.hh"
#include	"HeaderSearchDirs.hh"
#include	"Utilities.hh"


ICGDiagnosticConsumer::ICGDiagnosticConsumer(llvm::raw_ostream	&os,	clang::DiagnosticOptions	*diags,	clang::CompilerInstance	&in_ci,	HeaderSearchDirs	&in_hsd)	
																																																:	clang::TextDiagnosticPrinter(os,	diags),	ci(in_ci),	hsd(in_hsd)	{
				error_in_user_code	=	false;
};
ICGDiagnosticConsumer::~ICGDiagnosticConsumer()	{

};


/**
	*	@details
	*	-#	Check	the	diagnostic	level	to	see	if	an	error	is	from	user	code.	
	*	-#	Terminate	the	build	if	yes,	continue	otherwise.
*/
void	ICGDiagnosticConsumer::HandleDiagnostic(clang::DiagnosticsEngine::Level	DiagLevel,	const	clang::Diagnostic	&Info)	{
				//	Use	TextDiagnosticPrinter	to	handle	diagnostic	if	the	code	is	user	code.
				//	Otherwise	use	base	DiagnosticConsumer	to	handle	diagnostic	for	system	code.
				if	(isInUserCode(ci	,	Info.getLocation(),	hsd))	{
								//	Parent	class	implementation	for	handling	diagnostic
								clang::TextDiagnosticPrinter::HandleDiagnostic(DiagLevel,	Info);

								//	Flag	it	if	an	error	is	from	user	code
								if	(DiagLevel	==	clang::DiagnosticsEngine::Level::Fatal	||	DiagLevel	==	clang::DiagnosticsEngine::Level::Error)	{
												error_in_user_code	=	true;
								}
				}	else	{
								//	Base	class	implementation	for	handling	diagnostic
								clang::DiagnosticConsumer::HandleDiagnostic(DiagLevel,	Info);
				}
}
