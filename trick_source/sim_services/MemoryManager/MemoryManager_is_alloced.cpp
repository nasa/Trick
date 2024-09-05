#include	"trick/MemoryManager.hh"

int	Trick::MemoryManager::is_alloced(void	*addr)	{

				ALLOC_INFO	*info;

				pthread_mutex_lock(&mm_mutex);
				if	(	(info	=	get_alloc_info_at(	addr))	!=	NULL)	{
								pthread_mutex_unlock(&mm_mutex);
								return	(1);
				}
				pthread_mutex_unlock(&mm_mutex);
				return	(0);
}

//	MEMBER	FUNCTION:
int	Trick::MemoryManager::var_exists(	std::string	var_name)	{
				VARIABLE_MAP::iterator	variable_pos;

				pthread_mutex_lock(&mm_mutex);
				variable_pos	=	variable_map.find(	var_name);

				if	(variable_pos	!=	variable_map.end())	{
								pthread_mutex_unlock(&mm_mutex);
								return	(1);
				}
				pthread_mutex_unlock(&mm_mutex);
				return	(0);
}
