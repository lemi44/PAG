#include "Core.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "imgui_impl_glfw_gl3.h"
#include "GUI.h"

Core* Core::ref_ = nullptr;

// ReSharper disable once CppPossiblyUninitializedMember
Core::Core() : root_(GraphNode(nullptr)), firstMouse_(true), showGui_(false), drawColor_(false)
{
	pressedKeys_[GLFW_KEY_F1] = false;
	ref_ = this;
}

int Core::run(int const width, int const height)
{
	Core core;
	return core.init(width, height);
}

int Core::init(int const width, int const height)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	Window win;
	win.init(width, height, "Zadanie 4");
	glfwSwapInterval(1); // Enable vsync


	/* Initialize GLEW */
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	ImGui_ImplGlfwGL3_Init(win.get(), true);
	Shader sha;
	Shader line_sha("Shaders/line.vert", "Shaders/line.frag");
	/* Apply shader */
	glUseProgram(sha.get());
	//glUniform1i(glGetUniformLocation(sha.get(), "diffuse"), 0);
	allModels_.push_back(Model("Assets/treeplan1.fbx"));
	allModels_.push_back(Model("Assets/hierarchy.fbx"));

	glfwSetInputMode(win.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(win.get(), mouseCallback);
	//glfwGetCursorPos(win.get(), &lastX_, &lastY_);
	lastX_ = width / 2, lastY_ = height / 2;
	glfwSetScrollCallback(win.get(), scrollCallback);
	glfwSetWindowSizeCallback(win.get(), windowSizeCallback);

	/* Enable depth test */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 0);
	glLineWidth(2.f);

	wvp_ = Transform(cam_.getWVPMatrix(win.get()));
	root_.setShader(&sha);
	root_.setLineShader(&line_sha);

	GraphNode tree(&allModels_[0]);
	GraphNode pion(&allModels_[1]);

	tree.setTransform(tree.getTransform().rotate(glm::vec3(glm::radians(-90.f), 0.f, 0.f)).scale(glm::vec3(.1f)));
	pion.setTransform(pion.getTransform().translate(glm::vec3(40.0f, -20.0f, 0.0f)));
	root_.addChild(&tree);
	root_.addChild(&pion);
	mainloop(win.get());

	return 0;
}

void Core::mainloop(GLFWwindow* window)
{
	oldTime_ = 0.0f;
	newTime_ = 0.0f;
	gameTime_ = 0.0f;

	auto show_test_window = true;
	auto show_another_window = false;
	auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		if (showGui_)
		{
			ImGui_ImplGlfwGL3_NewFrame();

			{
				glm::vec3 pos = cam_.getCameraPos();
				ImGui::Text("X:%f, Y:%f, Z:%f", pos.x, pos.y, pos.z);
				if (selectedModel_ != nullptr)
				{
					ImGui::Text("Selected model with id:%d", selectedModel_->getID());
					glm::mat4 matrix = selectedModel_->world.getMatrix();
					glm::vec3 world(matrix[3][0], matrix[3][1], matrix[3][2]);
					world -= pos;
					ImGui::SliderFloat3("World Translation", reinterpret_cast<float*>(&world), -50.f, 50.f);
					ImGui::SliderFloat3("Translate", reinterpret_cast<float*>(&selectedModel_->pos), -50.f, 50.f);
					ImGui::SliderFloat3("Rotate", reinterpret_cast<float*>(&selectedModel_->rot), -glm::pi<float>(), glm::pi<float>());
					ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&selectedModel_->scale), 0.f, 2.f);
				}
				else
				{
					ImGui::Text("No model selected");
				}
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			GUI::ShowPropertyEditor(&showGui_, allModels_);
		}
		//if imgui glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);

		/* Update game time value */
		oldTime_ = newTime_;
		newTime_ = float(glfwGetTime());
		gameTime_ = newTime_ - oldTime_;
		/* Update game here */
		update(window);
		/* Render here */
		render(gameTime_, window);
		/* Swap front and back buffers */
		if (!drawColor_)
			glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();
	}
}

void Core::processInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		drawColor_ = true;
	}
	else
		drawColor_ = false;
	auto sprint_speed = 2.f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sprint_speed = 4.f;

	auto const camera_speed = 2.5f * gameTime_ * sprint_speed;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam_.setCameraPos(cam_.getCameraPos() + camera_speed * cam_.getCameraFront());
	//{
	//	glm::vec3 vec = cam_.getCameraPos() + camera_speed * cam_.getCameraFront();
	//	vec.y = 1.5f;
	//	cam_.setCameraPos(vec);
	//}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam_.setCameraPos(cam_.getCameraPos() - camera_speed * cam_.getCameraFront());
	//{
	//	glm::vec3 vec = cam_.getCameraPos() - camera_speed * cam_.getCameraFront();
	//	vec.y = 1.5f;
	//	cam_.setCameraPos(vec);
	//}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam_.setCameraPos(cam_.getCameraPos() - glm::normalize(glm::cross(cam_.getCameraFront(), cam_.getCameraUp())) * camera_speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam_.setCameraPos(cam_.getCameraPos() + glm::normalize(glm::cross(cam_.getCameraFront(), cam_.getCameraUp())) * camera_speed);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam_.setCameraPos(cam_.getCameraPos() + camera_speed * cam_.getCameraUp());
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && !ImGui::GetIO().WantCaptureMouse)
		cam_.setCameraPos(cam_.getCameraPos() - camera_speed * cam_.getCameraUp());
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !pressedKeys_[GLFW_KEY_F1])
	{
		pressedKeys_[GLFW_KEY_F1] = true;
		showGui_ = !showGui_;
		if (showGui_)
		{
			glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
		}
		else
		{
			glfwSetScrollCallback(window, scrollCallback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
		pressedKeys_[GLFW_KEY_F1] = false;
}

void Core::update(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) && showGui_ && !ImGui::GetIO().WantCaptureMouse) {
		//glm::vec3 ray_origin;
		//glm::vec3 ray_direction;
		//int screen_width, screen_height;
		//glfwGetWindowSize(window, &screen_width, &screen_height);
		//double mouseX = 0, mouseY = 0;
		//glfwGetCursorPos(window, &mouseX, &mouseY);
		//screenPosToWorldRay(
		//	int(mouseX), screen_height - int(mouseY),
		//	screen_width, screen_height,
		//	cam_.getViewMatrix(),
		//	cam_.getProjectionMatrix(),
		//	ray_origin,
		//	ray_direction
		//);

		////ray_direction = ray_direction*20.0f;
		//if (selectedModel_ != nullptr)
		//	for (auto &m : selectedModel_->getMeshes())
		//		m.outline = false;
		//selectedModel_ = nullptr;

		//// Test each each Oriented Bounding Box (OBB).
		//// A physics engine can be much smarter than this, 
		//// because it already has some spatial partitionning structure, 
		//// like Binary Space Partitionning Tree (BSP-Tree),
		//// Bounding Volume Hierarchy (BVH) or other.
		//intersection_distance = std::numeric_limits<float>::max(); // Output of TestRayOBBIntersection()
		//for (auto &m : allModels_)
		//{
		//	checkAllChildren(&m, ray_origin, ray_direction, intersection_distance);
		//	float distance;
		//	
		//	if (testRayObbIntersection(ray_origin, ray_direction, m.aabb_min, m.aabb_max, getModelMatrix(findGraphNode(&m, &root_), &m), distance))
		//	{
		//		if (distance < intersection_distance)
		//		{
		//			intersection_distance = distance;
		//			selectedModel_ = &m;
		//			for (auto &mes : selectedModel_->getMeshes())
		//				mes.outline = true;
		//		}
		//	}
		//}

		if (selectedModel_ != nullptr)
			for (auto &m : selectedModel_->getMeshes())
				m.outline = false;
		selectedModel_ = nullptr;

		// Wait until all the pending drawing commands are really done.
		// Ultra-mega-over slow ! 
		// There are usually a long time between glDrawElements() and
		// all the fragments completely rasterized.
		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		// Read the pixel at the center of the screen.
		// You can also use glfwGetMousePos().
		// Ultra-mega-over slow too, even for 1 pixel, 
		// because the framebuffer is on the GPU.
		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		

		std::vector<GLubyte> data = {0xFF, 0xFF, 0xFF, 0x0};
		//std::cout << "Trying to read pixel for color-picking at (" << mouseX << "," << height - int(mouseY) << ")" << std::endl;
		glReadPixels(int(mouseX), height - int(mouseY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		bool error = false;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "Got error while trying to read pixel for color-picking at (" << mouseX << "," << height - int(mouseY) << "): ";
			switch(err)
			{
			case GL_INVALID_ENUM:
				std::cout << "GL_INVALID_ENUM";
				break;
			default:
				std::cout << err;
			}
			std::cout << std::endl;
			error = true;
		}
		if (!error)
		{
			int pickedID =
				data[0];
			if (pickedID == 0x00ffffff || pickedID == 0) { // Full white, must be the background !
				std::cout << "Picked BG!" << std::endl;
				if (selectedModel_ != nullptr)
					for (auto &m : selectedModel_->getMeshes())
						m.outline = false;
				selectedModel_ = nullptr;
			}
			else {
				std::cout << "Picked id=" << pickedID << ",r=" << int(data[3]) << ",g=" << int(data[2]) << ",b=" << int(data[1]) << ",a=" << int(data[0]) << std::endl;
				for (auto &m : allModels_)
				{
					selectedModel_ = findModelByID(pickedID, &m);
					if (selectedModel_ != nullptr)
					{
						for (auto &mes : selectedModel_->getMeshes())
							mes.outline = true;
						break;
					}
				}
			}
		}


	}

}

void Core::render(float tpf, GLFWwindow* window)
{
	/* Clear the color buffer & depth buffer*/
	if (drawColor_)
		glClearColor(0.f, 0.f, 0.f, 0.f);
	else
		glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto const wvp_changed = cam_.getCameraChanged();
	if (wvp_changed)
		wvp_.setMatrix(cam_.getWVPMatrix(window));

	root_.render(wvp_, wvp_changed, drawColor_);
	if (showGui_)
		ImGui::Render();
}

void Core::windowSizeCallback(GLFWwindow * window, int const width, int const height)
{
	glViewport(0, 0, width, height);
}

void Core::mouseCallback(GLFWwindow * window, double const xpos, double const ypos)
{
	if (!ref_->showGui_)
	{
		if (ref_->firstMouse_)
		{
			ref_->lastX_ = xpos;
			ref_->lastY_ = ypos;
			ref_->firstMouse_ = false;
		}

		auto xoffset = xpos - ref_->lastX_;
		auto yoffset = ref_->lastY_ - ypos;
		ref_->lastX_ = xpos;
		ref_->lastY_ = ypos;
		auto const sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		ref_->cam_.setYaw(ref_->cam_.getYaw() + xoffset);
		ref_->cam_.setPitch(ref_->cam_.getPitch() + yoffset);
		if (ref_->cam_.getPitch() > 89.0f)
			ref_->cam_.setPitch(89.0f);
		if (ref_->cam_.getPitch() < -89.0f)
			ref_->cam_.setPitch(-89.0f);
		glm::vec3 front;
		front.x = cos(glm::radians(ref_->cam_.getYaw())) * cos(glm::radians(ref_->cam_.getPitch()));
		front.y = sin(glm::radians(ref_->cam_.getPitch()));
		front.z = sin(glm::radians(ref_->cam_.getYaw())) * cos(glm::radians(ref_->cam_.getPitch()));
		ref_->cam_.setCameraFront(glm::normalize(front));
	}
	else
		ref_->firstMouse_ = true;
}

Model* Core::findModelByID(int i, Model* mdl)
{
	for (auto &m : mdl->getChildren())
	{
		Model* ret = findModelByID(i, &m);
		if (ret != nullptr)
			return ret;
		if (m.getID() == i)
			return &m;
	}
	return nullptr;
}

Core::~Core()
{
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
}

void Core::scrollCallback(GLFWwindow* window, double const xoffset, double const yoffset)
{
	if (ref_->cam_.getFoV() >= glm::radians(1.0f) && ref_->cam_.getFoV() <= glm::radians(45.0f))
		ref_->cam_.setFoV(ref_->cam_.getFoV() - glm::radians(yoffset));
	if (ref_->cam_.getFoV() <= glm::radians(1.0f))
		ref_->cam_.setFoV(glm::radians(1.0f));
	if (ref_->cam_.getFoV() >= glm::radians(45.0f))
		ref_->cam_.setFoV(glm::radians(45.0f));
}

void Core::screenPosToWorldRay(
	const int mouse_x, const int mouse_y,             // Mouse position, in pixels, from bottom-left corner of the window
	const int screen_width, const int screen_height,  // Window size, in pixels
	const glm::mat4 view_matrix,               // Camera position and orientation
	const glm::mat4 projection_matrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
) const
{
	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	const glm::vec4 l_ray_start_ndc(
		(float(mouse_x) / float(screen_width) - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		(float(mouse_y) / float(screen_height) - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	const glm::vec4 l_ray_end_ndc(
		(float(mouse_x) / float(screen_width) - 0.5f) * 2.0f,
		(float(mouse_y) / float(screen_height) - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	const auto inverse_projection_matrix = glm::inverse(projection_matrix);

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	const auto inverse_view_matrix = glm::inverse(view_matrix);

	auto l_ray_start_camera = inverse_projection_matrix * l_ray_start_ndc;    l_ray_start_camera /= l_ray_start_camera.w;
	auto l_ray_start_world = inverse_view_matrix       * l_ray_start_camera; l_ray_start_world /= l_ray_start_world.w;
	auto l_ray_end_camera = inverse_projection_matrix * l_ray_end_ndc;      l_ray_end_camera /= l_ray_end_camera.w;
	auto l_ray_end_world = inverse_view_matrix       * l_ray_end_camera;   l_ray_end_world /= l_ray_end_world.w;

	// Faster way (just one inverse)
	//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

	glm::vec3 l_ray_dir_world(l_ray_end_world - l_ray_start_world);
	l_ray_dir_world = glm::normalize(l_ray_dir_world);

	out_origin = glm::vec3(l_ray_start_world);
	out_direction = glm::normalize(l_ray_dir_world);
}


bool Core::testRayObbIntersection(
	const glm::vec3 ray_origin,        // Ray origin, in world space
	const glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	const glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	const glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4 model_matrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
) const
{

	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	auto t_min = -100000.0f;
	auto t_max = 100000.0f;

	const glm::vec3 ob_bposition_worldspace(model_matrix[3].x, model_matrix[3].y, model_matrix[3].z);

	const auto delta = ob_bposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		const glm::vec3 xaxis(model_matrix[0].x, model_matrix[0].y, model_matrix[0].z);
		const auto e = glm::dot(xaxis, delta);
		const auto f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			auto t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			auto t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
											 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

											 // We want t1 to represent the nearest intersection, 
											 // so if it's not the case, invert t1 and t2
			if (t1 > t2) {
				const auto w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < t_max)
				t_max = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > t_min)
				t_min = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (t_max < t_min)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		const glm::vec3 yaxis(model_matrix[1].x, model_matrix[1].y, model_matrix[1].z);
		const auto e = glm::dot(yaxis, delta);
		const auto f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {
			auto t1 = (e + aabb_min.y) / f;
			auto t2 = (e + aabb_max.y) / f;

			if (t1 > t2) { const auto w = t1; t1 = t2; t2 = w; }

			if (t2 < t_max)
				t_max = t2;
			if (t1 > t_min)
				t_min = t1;
			if (t_min > t_max)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		const glm::vec3 zaxis(model_matrix[2].x, model_matrix[2].y, model_matrix[2].z);
		const auto e = glm::dot(zaxis, delta);
		const auto f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {
			auto t1 = (e + aabb_min.z) / f;
			auto t2 = (e + aabb_max.z) / f;

			if (t1 > t2) { const auto w = t1; t1 = t2; t2 = w; }

			if (t2 < t_max)
				t_max = t2;
			if (t1 > t_min)
				t_min = t1;
			if (t_min > t_max)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = t_min;
	return true;

}


void Core::checkAllChildren(Model* mdl, glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance)
{
	for (auto &m : mdl->getChildren())
	{
		checkAllChildren(&m, ray_origin, ray_direction, intersection_distance);
		//if (selectedModel_ != nullptr) break;
		float distance = 0.f;
		if (testRayObbIntersection(
			ray_origin,
			ray_direction,
			m.aabb_min,
			m.aabb_max,
			getModelMatrix(findGraphNode(&m, &root_), &m),
			distance)
			) {
			if (distance < intersection_distance)
			{
				intersection_distance = distance;
				selectedModel_ = &m;
				for (auto &mes : selectedModel_->getMeshes())
					mes.outline = true;
			}
		}
	}
}

GraphNode* Core::findGraphNode(Model* mdl, GraphNode* root) const
{
	if (root == nullptr)
		return root;

	if (findModelByModel(root->getModel(), mdl) != nullptr)
		return root;

	for (auto &node : root->children)
	{
		auto *ret = findGraphNode(mdl, node);
		if (ret != nullptr)
			return ret;
	}
	return nullptr;
}
Model* Core::findModelByModel(Model* root, Model* mdl) const
{
	if (root == nullptr || mdl == nullptr)
		return nullptr;
	if (root == mdl)
		return root;
	for (auto &child : root->getChildren())
	{
		auto *ret = findModelByModel(&child, mdl);
		if (ret != nullptr)
			return ret;
	}
	return nullptr;
}
glm::mat4 Core::getModelMatrix(GraphNode* node, Model* mdl)
{
	if (node == nullptr || mdl == nullptr)
		return glm::mat4(0.f);
	Transform ret = root_.getTransform().getMatrix();
	{
		auto it = &root_;
		while (it != nullptr)
		{
			for (auto child : it->children)
			{
				if (child == node)
				{
					it = nullptr;
					break;
				}
				if (isAncestor(child, node))
				{
					ret.combine(child->getTransform());
					it = child;
					break;
				}
			}
		}
		ret.combine(node->getTransform());
	}
	{
		auto it = node->getModel();
		while (it != nullptr && it!= mdl)
		{
			ret.translate(it->pos).rotate(it->rot).scale(it->scale);
			for (auto& child : it->getChildren())
			{
				if (&child == mdl)
				{
					it = nullptr;
					break;
				}
				if (isAncestor(&child, mdl))
				{
					it = &child;
					break;
				}
			}
		}
		ret.translate(mdl->pos).rotate(mdl->rot).scale(mdl->scale);
	}
	return ret.getMatrix();
}
bool Core::isAncestor(GraphNode* parent, GraphNode* child) const
{
	if (parent == nullptr || child == nullptr)
		return false;
	if (parent == child)
		return false;
	for (auto& node : parent->children)
	{
		if (node == child)
			return true;
		if (isAncestor(node, child))
			return true;
	}
	return false;
}
bool Core::isAncestor(Model* parent, Model* child) const
{
	if (parent == nullptr || child == nullptr)
		return false;
	if (parent == child)
		return false;
	for (auto& node : parent->getChildren())
	{
		if (&node == child)
			return true;
		if (isAncestor(&node, child))
			return true;
	}
	return false;
}
