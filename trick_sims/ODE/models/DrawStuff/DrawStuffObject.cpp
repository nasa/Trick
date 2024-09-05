
#include	"DrawStuffObject.hh"
#include	"DrawStuff.hh"

DrawStuffObject::~DrawStuffObject()	{}

void	DrawStuffObject::add_object()	{
				DrawStuff::add_object(this)	;
}

void	DrawStuffObject::command(int)	{
}
