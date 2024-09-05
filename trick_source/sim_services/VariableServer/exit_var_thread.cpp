
#include	"trick/VariableServer.hh"

//	This	should	only	be	called	from	the	VST	itself
void	exit_var_thread(void	*in_vst)	{
				Trick::VariableServerSessionThread	*	vst	=	(Trick::VariableServerSessionThread	*)	in_vst	;

				Trick::VariableServer	*	vs	=	vst->get_vs()	;
				
				vs->delete_session(vst->get_pthread_id());

				//	Tell	the	variable	server	that	this	thread	is	exiting.
				vs->delete_vst(vst->get_pthread_id())	;

				vst->cleanup();
}

