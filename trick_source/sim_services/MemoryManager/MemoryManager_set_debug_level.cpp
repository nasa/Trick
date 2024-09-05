#include	"trick/MemoryManager.hh"

void	Trick::MemoryManager::set_debug_level(int	level)	{
				debug_level	=	level;
				currentCheckPointAgent->set_debug_level(level);
				defaultCheckPointAgent->set_debug_level(level);
				return;
}

void	Trick::MemoryManager::set_reduced_checkpoint(bool	flag)	{
				reduced_checkpoint	=	flag;
				currentCheckPointAgent->set_reduced_checkpoint(flag);
				defaultCheckPointAgent->set_reduced_checkpoint(flag);
}

void	Trick::MemoryManager::set_hexfloat_checkpoint(bool	flag)	{
				hexfloat_checkpoint	=	flag;
				currentCheckPointAgent->set_hexfloat_checkpoint(flag);
				defaultCheckPointAgent->set_hexfloat_checkpoint(flag);
}

void	Trick::MemoryManager::set_expanded_arrays(bool	flag)	{
				expanded_arrays	=	flag;
}
