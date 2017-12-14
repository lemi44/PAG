#pragma once

#include "GraphNode.h"
#include "Camera.h"
#include <map>

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
	std::vector<Model> allModels_;
	Model* selectedModel_;
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	void render(float tpf, GLFWwindow* window);
	void update(GLFWwindow* window);
	int init(int width, int height);
	void processInput(GLFWwindow* window);
	static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
	void mainloop(GLFWwindow* window);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	void screenPosToWorldRay(int mouse_x, int mouse_y, int screen_width, int screen_height,
		glm::mat4 view_matrix, glm::mat4 projection_matrix, glm::vec3& out_origin,
		glm::vec3& out_direction) const;
	bool testRayObbIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction,
		glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 model_matrix, float& intersection_distance) const;
	void checkAllChildren(Model* mdl, glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance);
	Model* findModelByID(int i, Model* mdl);
	GraphNode* findGraphNode(Model* mdl, GraphNode* root) const;
	Model* findModelByModel(Model* root, Model* mdl) const;
	glm::mat4 getModelMatrix(GraphNode* node, Model* mdl);
	bool isAncestor(GraphNode* parent, GraphNode* child) const;
	bool isAncestor(Model* parent, Model* child) const;
	static Core* ref_;
	Core();
public:
	Core(Core const&) = delete;
	void operator=(Core const&) = delete;
	~Core();

	static int run(int width, int height);
};

