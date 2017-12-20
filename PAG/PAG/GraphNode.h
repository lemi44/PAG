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
	
	void render(Transform wvp, Transform model, bool dirty, bool picking_color, Shader* sha = NULL, Shader* line_shader = NULL);
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
	void renderModel(Model* model, Transform wvp, Transform model_mat, Shader* sha, Shader* line_shader, bool aabb) const;
};

