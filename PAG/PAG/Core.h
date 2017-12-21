#pragma once

#include "GraphNode.h"
#include "Camera.h"
#include <map>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

struct ModelStorage
{
private:
	std::vector<Model*> allModels_;
	std::map<int, Model*> modelRefs_;
	void insertAllIDs(Model* model)
	{
		modelRefs_.insert(std::make_pair(model->getID(), model));
		for(auto& child : model->getChildren())
		{
			insertAllIDs(&child);
		}
	}
public:
	int addModel(Model* model)
	{
		allModels_.push_back(model);
		insertAllIDs(model);
		return model->getID();
	}
	Model* getModel(int const i) const
	{
		try
		{
			return modelRefs_.at(i);
		}
		catch (std::out_of_range)
		{
			return nullptr;
		}
	}
	std::vector<Model*>& getAllModels()
	{
		return allModels_;
	}
	void clear()
	{
		for (auto it = allModels_.begin(); it != allModels_.end(); ++it)
		{
			delete (*it);
		}
		allModels_.clear();
	}
};

struct NodeStorage
{
private:
	std::vector<GraphNode*> allNodes_;
public:
	void addNode(GraphNode* node)
	{
		allNodes_.push_back(node);
	}
	GraphNode* getNode(int const i) const
	{
		try
		{
			return allNodes_.at(i);
		}
		catch (std::out_of_range)
		{
			return nullptr;
		}
	}
	std::vector<GraphNode*>& getAllNodes()
	{
		return allNodes_;
	}
	void clear()
	{
		for (auto it = allNodes_.begin(); it != allNodes_.end(); ++it)
		{
			delete (*it);
		}
		allNodes_.clear();
	}
};

class Core
{
private:
	GraphNode root_;
	Transform wvp_;
	Camera cam_;
	GLfloat oldTime_;
	GLfloat newTime_;
	GLfloat gameTime_;
	double lastX_, lastY_;
	bool firstMouse_, showGui_, drawColor_;
	float intersection_distance;
	std::map<int, bool> pressedKeys_;
	ModelStorage models_;
	Model* selectedModel_;
	NodeStorage nodes_;
	DirectionalLight dir_light_;
	PointLight point_light_;
	SpotLight spot_light_;
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	void render(float tpf, GLFWwindow* window, Shader* shader);
	void update(GLFWwindow* window);
	void loadContent(Shader* shader);
	int init(int width, int height);
	void processInput(GLFWwindow* window);
	static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
	void mainloop(GLFWwindow* window, Shader* shader);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	static Core* ref_;
	Core();
public:
	Core(Core const&) = delete;
	void operator=(Core const&) = delete;
	~Core();

	static int run(int width, int height);
};

