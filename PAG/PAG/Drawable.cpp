#include "Drawable.h"



Drawable::Drawable()
{
	static int sid = 1;
	id = sid++;
}


Drawable::~Drawable()
{
}

bool Drawable::isLight() const
{
	return false;
}

int Drawable::getID() const
{
	return id;
}
