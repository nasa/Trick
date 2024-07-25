
/*
Abstract base class for anything that exists on the map and can interact/be interacted with
*/
class Entity
{
	private:

	public:

	virtual Shape* get_shapes() = 0;	// Return root of shape tree
	virtual int move(int x, int y) = 0;	// Translates entire entity
	virtual int rotate(int d) = 0;		// Rotates entire entity around core shape origin
}
