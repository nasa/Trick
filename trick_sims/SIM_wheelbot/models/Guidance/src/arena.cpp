#include	"arena.hh"
#include	<math.h>						//	sqrt()
#include	<stdlib.h>				//	abs()
#include	<iostream>

Arena::Arena(unsigned	int	width,	unsigned	int	height)
				:	height(height),	width(width)	{

				unsigned	int	area	=	height	*	width;
				grid	=	new	GridSquare[area];
				for	(int	i=0;	i	<	area;	i++)	{
								grid[i].isBlocked	=	false;
								grid[i].mark	=	'	'	;
				}
}

Arena::Arena(unsigned	int	width,	unsigned	int	height,	unsigned	char	bits[])
						:	height(height),	width(width)	{

			unsigned	int	area	=	height	*	width;
			grid	=	new	GridSquare[area];

			unsigned	int	cx	=0;
			for	(int	row=0;	row<height;	row++)	{
							unsigned	char	octet	=	bits[cx];
							unsigned	int	bx=0;
							while	(	bx	<	width	)	{
											unsigned	int	ii	=	row*width+bx;
											if	((bx	!=	0)	&&	((bx	%	8)==0))	{
															cx	++;
															octet=bits[cx];
											}
											grid[ii].isBlocked	=	(0x01	&	octet);
											octet	=	octet	>>	1;
											grid[ii].mark	=	'	'	;
											bx++;
							}
							cx	++;
							octet=bits[cx];
			}
}

Arena::~Arena()	{
				delete	grid;
}

//straightDistance
int	Arena::distanceBetween(	GridSquare*	orig,	GridSquare*	dest,	int&	distance	)	{
				Point	origPt;
				Point	destPt;
				if	(((	getGridSquareCoordinates(orig,	origPt)	)	==	0)	&&
								((	getGridSquareCoordinates(dest,	destPt)	)	==	0))	{
								distance	=	10	*	sqrt((destPt.x	-	origPt.x)	*	(destPt.x	-	origPt.x)	+
																													(destPt.y	-	origPt.y)	*	(destPt.y	-	origPt.y));
								return	0;
				}
				std::cerr	<<	"Arena::distanceBetween:	bad	pointer	parameter(s)."	<<	std::endl;
				return	1;
}

//manhattenDistance
int	Arena::movementCostEstimate(	GridSquare*	orig,	GridSquare*	dest,	int&	costEstimate	)	{
				Point	origPt;
				Point	destPt;
				if	(((	getGridSquareCoordinates(orig,	origPt)	)	==	0)	&&
								((	getGridSquareCoordinates(dest,	destPt)	)	==	0))	{
								costEstimate	=	10	*	(abs(destPt.x	-	origPt.x)	+	abs(destPt.y	-	origPt.y));
								return	0;
				}
				std::cerr	<<	"Arena::movementCostEstimate:	bad	pointer	parameter(s)."	<<	std::endl;
				return	1;
}

void	Arena::block(unsigned	int	x,	unsigned	int	y)	{
				GridSquare*	gridSquare;
				if	(	(gridSquare	=	getGridSquare(x,	y))	!=	(GridSquare*)0)	{
								gridSquare->isBlocked	=	true;
				}
}

void	Arena::unblock(unsigned	int	x,	unsigned	int	y)	{
				GridSquare*	gridSquare;
				if	(	(gridSquare	=	getGridSquare(x,	y))	!=	(GridSquare*)0)	{
								gridSquare->isBlocked	=	false;
				}
}

void	Arena::mark(unsigned	int	x,	unsigned	int	y,	char	c)	{
				GridSquare*	gridSquare;
				if	(	(gridSquare	=	getGridSquare(x,	y))	!=	(GridSquare*)0)	{
								gridSquare->mark	=	c;
				}
}

int	Arena::calcOffset(unsigned	int	x,	unsigned	int	y,	size_t&	offset)	{
				if	((x	<	width)	&&	(y	<	height))	{
								offset	=	x+width*y;
								return	0;
				}
				return	1;
}

int	Arena::calcOffset(GridSquare*	gridSquare,	size_t&	offset)	{

				if	(gridSquare	>=	grid)	{
								size_t	toffset	=	(gridSquare	-	grid);
								if	(toffset	<	(width	*	height))	{
												offset	=	toffset;
												return	0;
								}
				}
				return	1;
}

GridSquare*	Arena::getGridSquare(unsigned	int	x,	unsigned	int	y)	{
				size_t	offset;
				if	(	calcOffset(x,y,offset)	==	0	)	{
								return	(	grid	+	offset	);
				}
				return	((GridSquare*)0);
}

int	Arena::getGridSquareCoordinates(	GridSquare*	gridSquarePointer,	Point&	coords	)	{
				size_t	offset;
				if	(	calcOffset(	gridSquarePointer,	offset)	==	0)	{
								coords.x	=	offset	%	width;
								coords.y	=	offset	/	width;
								return	0;
				}	else	{
								std::cerr	<<	"Arena::getGridSquareCoordinates:	problem."	<<	std::endl;
								return	1;
				}
}

std::vector<GridSquare*>	Arena::getNeighbors(	GridSquare*	gridSquarePointer	)	{

				std::vector<GridSquare*>	neighbors;
				GridSquare*	neighbor;
				Point	loc;

				if	(	getGridSquareCoordinates(	gridSquarePointer,	loc	)	==	0)	{

#ifdef	DIAGONAL_NEIGHBORS
								if	((neighbor=getGridSquare(loc.x+1,	loc.y+1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x+1,	loc.y-1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x-1,	loc.y-1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x-1,	loc.y+1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
#endif
								if	((neighbor=getGridSquare(loc.x,	loc.y+1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x,	loc.y-1))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x-1,	loc.y))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}
								if	((neighbor=getGridSquare(loc.x+1,	loc.y))	!=	(GridSquare*)0)	{
												if	(!neighbor->isBlocked)
																neighbors.push_back(neighbor);
								}

				}	else	{
								std::cerr	<<	"Arena::getNeighbors:	invalid	gridSquarePointer.";
				}
				return	neighbors;
}

std::ostream&	operator<<	(std::ostream&	s,	const	Arena&	arena)	{
				s	<<	"Arena	height="	<<	arena.height	<<	"	width="	<<	arena.width	<<	std::endl;

				for	(int	y=0;	y	<	arena.height;	y++)	{
								s	<<	"|";
								for	(int	x=0;	x	<	arena.width;	x++)	{
												if	(	arena.grid[x	+	arena.width	*	y].isBlocked	)	{
																s	<<	"\x1b[41m"	<<	arena.grid[x	+	arena.width	*	y].mark	<<	"\x1b[47m"	;
												}	else	{
																s	<<	arena.grid[x	+	arena.width	*	y].mark	;
												}
												s	<<	"|"	;
								}
								s	<<	std::endl;
				}

				return	s;
}
