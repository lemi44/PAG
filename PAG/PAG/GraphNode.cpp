#include "GraphNode.h"


GraphNode::GraphNode(Model * model)
	: dirty_(true),
	local_(Transform::origin()),
	model_(model),
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

void GraphNode::render(const Transform wvp, const Transform model_mat, bool dirty, const bool picking_color, Shader* sha, Shader* line_shader)
{
	if (!sha) sha = shader_;
	if (!line_shader) line_shader = line_shader_;
	dirty |= dirty_;
	if (dirty)
	{
		world_ = local_.combine(model_mat);
		dirty_ = false;
	}

	if (model_) renderModel(model_, wvp, world_, sha, line_shader, picking_color);

	for (auto child : children)
	{
		child->render(world_, model_mat, dirty, picking_color, sha, line_shader);
	}
}

void GraphNode::addChild(GraphNode * node)
{
	children.push_back(node);
}

Model* GraphNode::getModel() const
{
	return model_;
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

void GraphNode::renderModel(Model * model, const Transform wvp, const Transform model_mat, Shader* sha, Shader* line_shader, const bool picking_color) const
{
	
	if (picking_color)
	{
		line_shader->use();
		model->drawColor(line_shader, wvp);
		sha->use();
	}
	else
	{
		model->draw(sha, wvp, model_mat);
	}
}
