#include	"trick/MemoryManager.hh"

/**
	*
	*/
int	Trick::MemoryManager::add_template_name_trans(	std::string	template_name	,	std::string	attr_name	)	{

				template_name_map[template_name]	=	attr_name	;
				return(0)	;
}

