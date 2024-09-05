#include	"trick/MemoryManager.hh"

//	MEMBER	FUNCTION:
void	Trick::MemoryManager::add_checkpoint_alloc_dependency(const	char	*name)	{
				VARIABLE_MAP_ITER	it	=	variable_map.find(name)	;
				if	(	it	!=	variable_map.end()	)	{
								dependencies.push_back(it->second)	;
								stl_dependencies.push_back(it->second)	;
				}
}

