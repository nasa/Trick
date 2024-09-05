#include	"trick/MemoryManager.hh"

Trick::CheckPointAgent	*	Trick::MemoryManager::get_CheckPointAgent()	{
				return	currentCheckPointAgent	;
}

void	Trick::MemoryManager::set_CheckPointAgent(CheckPointAgent*	agent)	{

				if	(agent	!=	NULL)	{
								currentCheckPointAgent	=	agent;
				}	else	{
								emitError("Attempt	to	set	CheckPointAgent	to	NULL.\n")	;
				}
				return;
}

void	Trick::MemoryManager::reset_CheckPointAgent()	{

				currentCheckPointAgent	=	defaultCheckPointAgent;
				return;
}

