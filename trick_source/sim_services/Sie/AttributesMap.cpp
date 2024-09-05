#include	<iostream>
#include	<fstream>
#include	<algorithm>
#include	"trick/AttributesMap.hh"
#include	"trick/attributes.h"
#include	"trick/command_line_protos.h"

//	Instantiate	static	variables	for	template	types	for	ATTRIBUTES	*	and	ENUM_ATTR	*
namespace	Trick	{
AttributesMap	*	AttributesMap::pInstance	=	NULL	;
}

std::string	&	Trick::AttributesMap::replace_special_chars(	std::string	&	str)	{

				//	escape	&
				size_t	index	=	0;
				while	(index	!=	std::string::npos)	{
								index	=	str.find("&"	,	index)	;
								if	(	index	!=	std::string::npos	)	{
												str.replace(index,	1,	"&amp;")	;
												index	+=	5;
								}
				}

				//	escape	"
				index	=	0;
				while	(index	!=	std::string::npos)	{
								index	=	str.find("\""	,	index)	;
								if	(	index	!=	std::string::npos	)	{
												str.replace(index,	1,	"&quot;")	;
								}
				}

				//	escape	<
				index	=	0;
				while	(index	!=	std::string::npos)	{
								index	=	str.find("<"	,	index)	;
								if	(	index	!=	std::string::npos	)	{
												str.replace(index,	1,	"&lt;")	;
								}
				}

				return	str;
}

std::string	&	Trick::AttributesMap::type_remove_dims(	std::string	&	type	)	{
				size_t	index	=	type.find_first_of("*[");
				if	(index	!=	std::string::npos)	{
								type.erase(index);
				}
				index	=	type.find_last_not_of("	");
				if	(index	!=	std::string::npos)	{
								type.erase(index	+	1);
				}

				return	replace_special_chars(type)	;
}

void	Trick::AttributesMap::print_xml(std::ofstream	&	sie_out	)	{
				std::map<std::string,	ATTRIBUTES	*>::iterator	it	;
				int	jj	;

				for	(	it	=	name_to_attr_map.begin()	;	it	!=	name_to_attr_map.end()	;	++it	)	{
								ATTRIBUTES	*	attr	=	(*it).second	;
								std::string	class_name	=	(*it).first;
								std::replace(class_name.begin(),	class_name.end(),	':',	'_');
								sie_out	<<	"		<class	name=\""	<<		class_name	<<	"\">\n"	;
								while	(	attr->name[0]	!=	'\0'	and	(attr->type_name	!=	NULL))	{
												sie_out	<<	"				<member"	;
												sie_out	<<	"\n						name=\""	<<	attr->name	<<	"\""	;
												std::string	type_name	=	attr->type_name;
												std::replace(type_name.begin(),	type_name.end(),	':',	'_');
												sie_out	<<	"\n						type=\""	<<	type_remove_dims(type_name)	<<	"\""	;
												sie_out	<<	"\n						io_attributes=\""	<<	attr->io	<<	"\""	;
												sie_out	<<	"\n						units=\""	;
												//	If	the	mods	bit	is	set	for	using	--	as	the	units
												if	(	attr->mods	&	TRICK_MODS_UNITSDASHDASH	)	{
																sie_out	<<	"--"	;
												}	else	{
																sie_out	<<	attr->units	;
												}
												sie_out	<<	"\""	;

												std::string	description	=	attr->des;
												if	(	!	description.empty()	)	{
																sie_out	<<	"\n						description=\""	<<	replace_special_chars(description)	<<	"\""	;
												}
												sie_out	<<	">\n"	;
												if	(	attr->num_index	>	0	)	{
																for	(jj	=	0;	jj	<	attr->num_index;	jj++)	{
																				sie_out	<<	"						<dimension>"	<<	attr->index[jj].size	<<	"</dimension>\n"	;
																}
												}
												sie_out	<<	"				</member>\n"	;
												attr++	;
								}
								sie_out	<<	"		</class>\n\n"	;
				}
}

void	Trick::AttributesMap::print_json(std::ofstream	&	sie_out	)	{
				std::map<std::string,	ATTRIBUTES	*>::iterator	it	;
				int	jj	;
				sie_out	<<	"		\"classes\":	[\n"	;
				for	(	it	=	name_to_attr_map.begin()	;	it	!=	name_to_attr_map.end()	;	++it	)	{
								ATTRIBUTES	*	attr	=	(*it).second	;
								std::string	class_name	=	(*it).first;
								std::replace(class_name.begin(),	class_name.end(),	':',	'_');
								sie_out	<<	"				{\n";
								sie_out	<<	"						\"name\":	\""	<<		class_name	<<	"\",\n"	;
								if(attr->name[0]	==	'\0'	||	(attr->type_name	==	NULL))	{
												sie_out	<<	"						\"members\":	[]\n"	;
								}	else	{
												sie_out	<<	"						\"members\":	[\n"	;
												while	(	attr->name[0]	!=	'\0'	and	(attr->type_name	!=	NULL))	{
																sie_out	<<	"								{\n";
																sie_out	<<	"										\"name\":	\""	<<	attr->name	<<	"\",\n"	;
																std::string	type_name	=	attr->type_name;
																std::replace(type_name.begin(),	type_name.end(),	':',	'_');
																sie_out	<<	"										\"type\":	\""	<<	type_remove_dims(type_name)	<<	"\",\n"	;
																sie_out	<<	"										\"io_attributes\":	\""	<<	attr->io	<<	"\",\n"	;
																sie_out	<<	"										\"units\":	\""	;
																//	If	the	mods	bit	is	set	for	using	--	as	the	units
																if	(	attr->mods	&	TRICK_MODS_UNITSDASHDASH	)	{
																				sie_out	<<	"--"	;
																}	else	{
																				sie_out	<<	attr->units	;
																}
																sie_out	<<	"\""	;

																std::string	description	=	attr->des;
																if	(	!	description.empty()	)	{
																				sie_out	<<	",\n										\"description\":	\""	<<	replace_special_chars(description)	<<	"\""	;
																}
																if	(	attr->num_index	>	0	)	{
																				sie_out	<<	",\n										\"dimensions\":	["	;
																				for	(jj	=	0;	jj	<	attr->num_index	-	1;	jj++)	{
																								sie_out	<<	"	\""	<<	attr->index[jj].size	<<	"\","	;
																				}
																				sie_out	<<	"	\""	<<	attr->index[attr->num_index	-	1].size	<<	"\"	"	;
																				sie_out	<<	"]\n"	;
																}	else	{
																				sie_out	<<	'\n'	;
																}
																sie_out	<<	"								}"	;
																if((attr	+	1)->name[0]	!=	'\0')	{
																				sie_out	<<	',';
																}
																sie_out	<<	'\n';
																attr++	;
												}
												sie_out	<<	"						]\n"	;
								}
								sie_out	<<	"				}"	;
								if(std::next(it,	1)	!=	name_to_attr_map.end())	{
												sie_out	<<	','	;
								}
								sie_out	<<	"\n"	;
				}
				sie_out	<<	"		],\n"	;
}
