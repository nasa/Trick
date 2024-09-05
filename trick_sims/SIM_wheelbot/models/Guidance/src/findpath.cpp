#include	<stdlib.h>
#include	<vector>				//	std::vector
#include	<algorithm>	//	std::find
#include	"findpath.hh"

std::vector<Point>	FindPath(	GridSquare*	origin,	GridSquare*	goal,	Arena*	arena)	{

				std::vector<GridSquare*>	openSet;
				std::vector<GridSquare*>	closedSet;
				std::vector<Point>	failure;

				int	estimated_cost_to_goal;	//	h(n)

				if	(arena	==	(Arena*)0)	{
								return	failure;
				}
				Point	origPt,	goalPt;
				if	(arena->getGridSquareCoordinates(origin,	origPt)	!=	0)	{
								std::cerr	<<	"FindPath:	Bad	Origin.";
								return	failure;
				}
				if	(arena->getGridSquareCoordinates(goal,	goalPt)	!=	0)	{
								std::cerr	<<	"FindPath:	Bad	Goal.";
								return	failure;
				}

				origin->parent	=	(GridSquare*)0;
				origin->g_score	=	0;

				arena->movementCostEstimate(origin,	goal,	estimated_cost_to_goal);
				origin->f_score	=	estimated_cost_to_goal;

				openSet.push_back(origin);

				while	(	!openSet.empty()	)	{

								//	Get	the	item	in	the	openSet	that	has	the	lowest	f_score.
								std::vector<GridSquare*>::iterator	curr,	best;
								for	(best=curr=openSet.begin();	curr	!=	openSet.end();	++curr)	{
													if	(	(*curr)->f_score	<	(*best)->f_score	)	{
																	best	=	curr;
													}
								}

								//	Get	and	remove	the	best	from	the	open	set.	This	is	current.
								GridSquare*	current	=	*best;
								openSet.erase(best);

								//	if	the	current	position	is	the	goal	then	backtrack,	build
								//	and	return	the	path	to	get	here.
								if	(current	==	goal)	{
												std::vector<Point>	path;
												while	(current	!=	(GridSquare*)0)	{
																Point	coordinates;
																if	(	arena->getGridSquareCoordinates(	current,	coordinates	)	==	0)	{
																				path.insert(	path.begin(),	coordinates);
																}
																current	=	current->parent;
												}
												return	path;
								}

								//	Add	current	to	the	closed	set.
								closedSet.push_back(current);

								//	Get	the	neighbors	of	current.
								std::vector<GridSquare*>	neighbors	=	arena->getNeighbors(	current	);

								//	For	each	of	the	neighbors
								std::vector<GridSquare*>::iterator	neighborsIterator;
								for	(	neighborsIterator=neighbors.begin();
														neighborsIterator!=neighbors.end();
														++neighborsIterator)	{

												GridSquare*	neighbor	=	*neighborsIterator;

												//	Search	for	the	neighbor	in	the	closed	set.
												std::vector<GridSquare*>::iterator	closedSetIterator	=
																find	(	closedSet.begin(),	closedSet.end(),	neighbor	);

												//	if	neighbor	is	not	in	the	closed	set.
												if	(closedSetIterator	==	closedSet.end())	{

																//	Calculate	a	g_score	for	this	neighbor.
																int	cost_to_move_to_neighbor;
																arena->distanceBetween(	current,	neighbor,	cost_to_move_to_neighbor	);
																int	neighbor_g_score	=	current->g_score	+	cost_to_move_to_neighbor;

																//	Search	for	the	neighbor	in	the	openset.
																std::vector<GridSquare*>::iterator	openSetIterator	=
																				find	(	openSet.begin(),	openSet.end(),	neighbor	);

																//	if	neighbor	is	in	the	openset	and	the	tentative	g	score	is	better	than	the	current	score
																if	((openSetIterator	==	openSet.end())	||	(neighbor_g_score	<	neighbor->g_score))	{

																				neighbor->parent	=	current;
																				neighbor->g_score	=	neighbor_g_score;
																				arena->movementCostEstimate(neighbor,	goal,	estimated_cost_to_goal	);
																				neighbor->f_score	=	neighbor->g_score	+	estimated_cost_to_goal;

																				if	(openSetIterator	==	openSet.end())	{
																								openSet.push_back(neighbor);
																				}
																}
												}
								}
				}
				std::cerr	<<	"Failed	to	find	a	path	to	the	goal.";
				return	failure;
}
