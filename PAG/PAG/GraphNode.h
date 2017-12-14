#pragma once
#include "Transform.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"

class GraphNode
{
public:
	GraphNode(Model* model);
	~GraphNode();
	
	void render(Transform parentWorld, bool dirty, bool aabb, Shader* sha = NULL, Shader* line_shader = NULL);
	void addChild(GraphNode* node);
	Model* getModel() const;
	Transform getTransform() const;
	void setShader(Shader* shader);
	void setLineShader(Shader* shader);
	void setTransform(Transform local);
	std::vector<GraphNode*> children;
private:
	Transform world_;
	bool dirty_;
	Transform local_;
	Model* model_;
	Shader* shader_;
	Shader* line_shader_;
	void renderModel(Model* model, Transform transform, Shader* sha, Shader* line_shader, bool aabb) const;
};

