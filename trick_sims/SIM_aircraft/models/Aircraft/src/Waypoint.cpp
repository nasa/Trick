/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(	Handles	the	waypoints.	)
*******************************************************************************/
#include	"Aircraft/include/Waypoint.hh"

#include	<stdlib.h>
#include	<iostream>
#include	<fstream>
#include	"Waypoint.hh"

WaypointList::WaypointList()	{
				length	=	0;
}

WaypointList::WaypointList(std::string	path)	{
				length	=	0;

				load(path);
}

int	WaypointList::add(double	n,	double	w,	std::string	i="images/defaultIcon.png")
{
				if	(length	<	ARRAY_SIZE)
				{
								north[length]	=	n;
								west[length]	=	w;
								img[length]	=	i;
								length++;
								return	length-1;
				}
				
				return	-1;
}

void	WaypointList::remove(int	index)
{
				if	(index	<	0	||	index	>=	length)	{
								std::cerr	<<	"Invalid	Index"	<<	std::endl;
								return;
				}

				length--;
				for(int	i	=	index;	i	<	length;	i++)	{
								north[i]	=	north[i+1];
								west[i]	=	west[i+1];
								img[i]	=	img[i+1];
				}
}

void	WaypointList::clear()	{
				length	=	0;
}

void	WaypointList::load(std::string	path)	{
				clear();
				append(path);
}

void	WaypointList::append(std::string	path)	{
				std::ifstream	in(path);

				if(in.is_open())	{
								std::string	str;
								//	Expected	input	formatting:	<North>,<West>	OR	<North>,<West>,<Image	Path>
								while(getline(in,str))	{
												int	k	=	str.find(','),	j	=	str.find(',',k+1);
												double	n,w;
												if	(j>=0)	{
																n	=	std::stod(str.substr(0,k));
																w	=	std::stod(str.substr(k+1,(j-k-1)));
																std::string	i	=	str.substr(j+1);
																add(n,w,i);
												}	else	{
																n	=	std::stod(str.substr(0,k));
																w	=	std::stod(str.substr(k+1));
																add(n,w);
												}
												
								}
								in.close();
				}	else	{
								std::cerr	<<	"FILE	DID	NOT	OPEN"	<<	std::endl;
				}
}

void	WaypointList::save(std::string	path)	{
				std::ofstream	out(path);

				if(out.is_open())	{
								for(int	i	=	0;	i	<	length;	i++)	{
												out	<<	std::to_string(north[i])	+	","	+	std::to_string(west[i])	+	","	+	img[i]	<<	std::endl;
								}
				}

				out.close();
}

void	WaypointList::getPosition(int	index,	double	(&pos)[2])	{
				pos[0]	=	north[index];
				pos[1]	=	west[index];
}

std::string	WaypointList::getImage(int	index)
{
				return	img[index];
}

int	WaypointList::size()
{
				return	length;
}
