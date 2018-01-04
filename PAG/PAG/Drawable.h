#pragma once
#include "Shader.h"
#include "Transform.h"
#include "Mesh.h"

class Drawable
{
protected:
	int id;
public:
	Drawable();
	virtual void draw(Shader* shader, const Transform wvp, const Transform model, const bool gui) = 0;
	virtual void drawColor(Shader * shader, const Transform wvp) = 0;
	virtual bool isLight() const;
	virtual std::vector<Mesh>& getMeshes() = 0;
	int getID() const;
	virtual ~Drawable();
};

