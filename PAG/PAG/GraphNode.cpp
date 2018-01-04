#include "GraphNode.h"


GraphNode::GraphNode(Drawable * drw)
	: dirty_(true),
	local_(Transform::origin()),
	drawable_(drw),
	shader_(nullptr)
{
}

GraphNode::~GraphNode()
{
}

void GraphNode::setTransform(Transform const local)
{
	local_ = local;
	dirty_ = true;
}

void GraphNode::render(const Transform wvp, const Transform model_mat, bool dirty, const bool picking_color, const bool gui, Shader* sha, Shader* line_shader)
{
	if (!sha) sha = shader_;
	if (!line_shader) line_shader = line_shader_;
	dirty |= dirty_;
	if (dirty)
	{
		world_ = local_.combine(model_mat);
		dirty_ = false;
	}

	if (drawable_) renderDrawable(drawable_, wvp, world_, sha, line_shader, picking_color, gui);

	for (auto child : children)
	{
		child->render(wvp, world_, dirty, picking_color, gui, sha, line_shader);
	}
}

void GraphNode::addChild(GraphNode * node)
{
	children.push_back(node);
}

Drawable* GraphNode::getDrawable() const
{
	return drawable_;
}

Transform GraphNode::getTransform() const
{
	return local_;
}

void GraphNode::setShader(Shader* shader)
{
	this->shader_ = shader;
}

void GraphNode::setLineShader(Shader* shader)
{
	line_shader_ = shader;
}

void GraphNode::renderDrawable(Drawable * drawable, const Transform wvp, const Transform model_mat, Shader* sha, Shader* line_shader, const bool picking_color, const bool gui) const
{
	
	if (picking_color)
	{
		line_shader->use();
		drawable->drawColor(line_shader, wvp);
		sha->use();
	}
	else
	{
		if (gui && drawable->isLight())
		{
			line_shader->use();
			drawable->draw(line_shader, wvp, model_mat, gui);
			sha->use();
		}
		else
			drawable->draw(sha, wvp, model_mat, gui);
	}
}
