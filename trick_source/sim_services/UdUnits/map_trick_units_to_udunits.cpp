
#include	<iostream>
#include	<string>
#include	<map>
#include	<vector>
#include	<numeric>

static	std::map<	std::string,	std::string>	init_map_old_to_ud()	{
				std::map<	std::string,	std::string>	init_map	;
				init_map["r"]	=	"rad"	;
				init_map["d"]	=	"degree"	;
				init_map["as"]	=	"arcsecond"	;
				init_map["am"]	=	"arcminute"	;
				init_map["rev"]	=	"revolution"	;
				init_map["M"]	=	"m"	;		//	really?		people	still	using	this?	(yes)
				init_map["n.m."]	=	"nmile"	;
				init_map["sl"]	=	"slug"	;
				init_map["mt"]	=	"t"	;
				init_map["lbm"]	=	"lb"	;
				init_map["oz"]	=	"ozf"	;
				init_map["v"]	=	"V"	;
				init_map["F"]	=	"degF"	;
				init_map["C"]	=	"degC"	;
				init_map["R"]	=	"degR"	;
				init_map["TNT"]	=	"ton_TNT"	;
				init_map["Wh"]	=	"(W*h)"	;
				init_map["kWh"]	=	"(kW*h)"	;
				init_map["gal"]	=	"gallon"	;
				init_map["dB"]	=	"1"	;
				init_map["one"]	=	"1"	;
				init_map["cnt"]	=	"count"	;
				init_map["--"]	=	"1"	;

				return	init_map	;
}

static	std::map<	std::string,	std::string>	old_to_ud	=	init_map_old_to_ud()	;

static	void	tokenize_units(const	std::string&	str,
																						std::vector<std::string>&	tokens,
																						const	std::string&	delimiters	=	"	")
{
				//	Skip	delimiters	at	beginning.
				std::string::size_type	lastPos	=	str.find_first_not_of(delimiters,	0);

				if	(	lastPos	!=	0	)	{
								tokens.push_back(str.substr(0,	lastPos));
				}

				//	Find	first	"non-delimiter".
				std::string::size_type	pos	=	str.find_first_of(delimiters,	lastPos);

				while	(std::string::npos	!=	pos	||	std::string::npos	!=	lastPos)	{
								//	Found	a	token,	add	it	to	the	vector.
								tokens.push_back(str.substr(lastPos,	pos	-	lastPos));
								//	Find	end	of	delimiters.		Note	the	"not_of"
								lastPos	=	str.find_first_not_of(delimiters,	pos);
								//	Push	the	delimiters
								if	(	lastPos	!=	std::string::npos	)	{
												tokens.push_back(str.substr(pos,	lastPos	-	pos));
								}	else	if	(	pos	!=	std::string::npos	)	{
												tokens.push_back(str.substr(pos));
								}
								//	Find	next	"non-delimiter"
								pos	=	str.find_first_of(delimiters,	lastPos);
				}

}

//	This	routine	(partially)	tokenizes	a	units	string.		It	substitutes
//	units	that	need	their	names	changed	to	conform	to	the	new	units	system.
std::string	map_trick_units_to_udunits(	std::string	orig_units	)	{
				std::vector<std::string>	tokens	;
				std::vector<std::string>::iterator	it	;
				tokenize_units(orig_units,	tokens,	"*/^()")	;
				for(	it	=	tokens.begin()	;	it	!=	tokens.end()	;	++it	)	{
								char	last_char	=	*((*it).rbegin())	;
								//	tokens	can	end	in	powers	of	2	or	3	in	the	old	system.
								//	save	the	power	if	it	is	the	last	character
								if(last_char	==	'2'	||	last_char	==	'3')	{
												(*it).erase((*it).length()	-1)	;
								}
								//	look	up	the	unit	to	see	if	it	needs	to	be	converted.
								std::map<	std::string,	std::string>::iterator	mit	;
								mit	=	old_to_ud.find(*it)	;
								if	(	mit	!=	old_to_ud.end()	)	{
												*it	=	mit->second	;
								}
								//	put	back	the	power	after	the	base	unit	has	been	tested
								if	((last_char	==	'2'	||	last_char	==	'3'))	{
												*it	+=	last_char	;
								}
								//std::cout	<<	"	"	<<	*it	<<	std::endl	;
				}
				std::string	new_units	=	std::accumulate(	tokens.begin(),tokens.end(),	std::string("")	);
				return	new_units	;
}
