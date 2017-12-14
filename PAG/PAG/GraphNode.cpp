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

void GraphNode::render(Transform parent_world, bool dirty, bool aabb, Shader* sha, Shader* line_shader)
{
	if (!sha) sha = shader_;
	if (!line_shader) line_shader = line_shader_;
	dirty |= dirty_;
	if (dirty)
	{
		world_ = local_.combine(parent_world);
		dirty_ = false;
	}

	if (model_) renderModel(model_, world_, sha, line_shader, aabb);

	for (auto child : children)
	{
		child->render(world_, dirty, aabb, sha, line_shader);
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

void GraphNode::renderModel(Model * model, Transform transform, Shader* sha, Shader* line_shader, bool aabb) const
{
	
	if (aabb)
	{
		glUseProgram(line_shader->get());
		model->drawColor(line_shader);
		glUseProgram(sha->get());
	}
	else
	{
		model->draw(sha, transform);
	}
}
