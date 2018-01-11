#pragma once
#include "Transform.h"
#include "Shader.h"
#include "Model.h"

class GraphNode
{
public:
	GraphNode(Drawable* model);
	~GraphNode();
	
	void render(Transform wvp, Transform model, bool dirty, bool picking_color, bool gui, Shader* sha = NULL, Shader* line_shader = NULL);
	void addChild(GraphNode* node);
	Drawable* getDrawable() const;
	Transform getTransform() const;
	void setShader(Shader* shader);
	void setLineShader(Shader* shader);
	void setTransform(Transform local);
	std::vector<GraphNode*> children;
	
private:
	Transform world_;
	bool dirty_;
	Transform local_;
	Drawable* drawable_;
	Shader* shader_;
	Shader* line_shader_;
	void renderDrawable(Drawable* drawable, Transform wvp, Transform model_mat, Shader* sha, Shader* line_shader, bool picking_color, const bool gui) const;
};

