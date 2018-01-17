#pragma once
#include <vector>
#include <map>
#include "Model.h"
#include "GraphNode.h"
#include "BaseLight.h"
#include "Skybox.h"

struct ModelStorage
{
private:
	std::vector<Model*> allModels_;
	std::map<int, Model*> modelRefs_;
	void insertAllIDs(Model* model)
	{
		modelRefs_.insert(std::make_pair(model->getID(), model));
		for (auto& child : model->getChildren())
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

struct LightStorage
{
private:
	std::vector<BaseLight*> allLights_;
public:
	void addLight(BaseLight* node)
	{
		allLights_.push_back(node);
	}
	BaseLight* getLight(int const i) const
	{
		try
		{
			return allLights_.at(i);
		}
		catch (std::out_of_range)
		{
			return nullptr;
		}
	}
	std::vector<BaseLight*>& getAllLights()
	{
		return allLights_;
	}
	void clear()
	{
		for (auto it = allLights_.begin(); it != allLights_.end(); ++it)
		{
			delete (*it);
		}
		allLights_.clear();
	}
};

struct Options
{
	bool reflections = true;
	bool refractions = false;
	bool ssao = true;
	float refractive_index = 1.52f;
};

struct Storage
{
	bool loadContent(Shader *shader, GraphNode *root);
	void clear();
	ModelStorage models;
	NodeStorage nodes;
	LightStorage lights;
	Options options;
	Skybox skybox;
};

