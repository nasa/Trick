#include	"trick/MemoryManager.hh"
#include	<sstream>

std::string	Trick::MemoryManager::make_decl_string(	TRICK_TYPE	type,
																																		std::string	user_type_name,
																																		int	n_stars,
																																		std::string	var_name,
																																		int	n_cdims,
																																		int	*cdims)	{

				std::stringstream	ss;

				ss	<<	trickTypeCharString(type,	user_type_name.c_str());
				for	(int	ii=0;	ii<n_stars;	ii++)	{
								ss	<<	"*";
				}
				ss	<<	"	";
				ss	<<	var_name	;
				for	(int	ii=0;	ii<n_cdims;	ii++)	{
								ss	<<	"["	<<	cdims[ii]	<<	"]";
				}
				return	ss.str();
}
