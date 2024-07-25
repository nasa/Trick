
#include "Entity.hh"
#include "Shape.hh"

/*
An organism
*/
class Organism: public Entity
{
	private:

	BodyPart* body;			// Tree of body parts 

	public:

	//Organism(BodyPart* parts);

	~Organism();

	Shape* get_shapes();		// Return root of shape tree
	int move(int x, int y);		// Translates entire entity
	int rotate(int d);		// Rotates entire entity around core shape origin
}
