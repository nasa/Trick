
#include "Structs/Coordinate.hh"
#include "Structs/Color.hh"

enum Shape_t
{
	S_CIRCLE = 0,
	S_TRIANGLE = 1,
	S_RECTANGLE = 2
};

/*
Abstract base representation of a single geometric unit
*/
class Shape
{
	private:

	Coord pos;			// Center of the shape
	Shape_t shape;			// Type of shape
	Shape* parent;			// Parent shape. Null if core shape
	Shape** attached_shapes;	// Shapes attached to this shape
	uint8_t attached_shapes_count;	// How many shapes are attached
	Color color;			// Base color of this shape
	//Some way to indicate attach point to parent

	public:
	
	Shape();

	virtual ~Shape() = 0;
	
	// Simple move function that translates position directly, no checks
	virtual void move(int x, int y);

	// Rotate by d degrees around this origin
	virtual void rotate(int d);

	// Rotate by d degrees around point c
	virtual void rotate(int d, Coord c);

	Coord get_coord();
	Shape_t get_shape_type();
	Shape* get_shapes();
	uint8_t get_shape_count();
	Color get_color;
	
	
};
