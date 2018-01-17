#pragma once

#include "GraphNode.h"
#include "Camera.h"
#include <map>
#include "Storage.h"
#include "Postprocess.h"
#include "Skybox.h"
#include "Deferred.h"
#include "SSAO.h"

class Core
{
private:
	/* PRIVATE DATA */
	GraphNode root_;
	Postprocess postprocess_;
	Deferred deferred_;
	SSAO ssao_;
	ViewProjection wvp_;
	Camera cam_;
	GLfloat oldTime_, newTime_, gameTime_, exposure_, intersection_distance;
	double lastX_, lastY_;
	bool firstMouse_, showGui_, drawColor_, firstFrame_;
	std::map<int, bool> pressedKeys_;
	Storage storage_;
	Drawable* selectedDrawable_;

	/* PRIVATE STATIC FIELDS */
	static Core* ref_;

	/* PRIVATE STATIC METHODS */
	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	/* PRIVATE METHODS */
	Core();
	int init(int width, int height);
	void mainloop(GLFWwindow* window, Shader* shader);
	void render(float tpf, GLFWwindow* window, Shader* shader);
	void update(GLFWwindow* window);
	void processInput(GLFWwindow* window);

public:
	/* PUBLIC STATIC METHODS */
	static int run(int width, int height);

	/* PUBLIC METHODS */
	Core(Core const&) = delete;
	void operator=(Core const&) = delete;
	~Core();
};

