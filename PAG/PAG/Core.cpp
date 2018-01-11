#include "Core.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <fstream>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "imgui_impl_glfw_gl3.h"
#include "GUI.h"
#include "Logger.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <string>
#include <vector>
#include <iterator>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}


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
	win.init(width, height, "Zadanie 6");
	glfwSwapInterval(1); // Enable vsync


	/* Initialize GLEW */
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		Logger::logError("Failed to initialize GLAD");
		return -1;
	}
	ImGui_ImplGlfwGL3_Init(win.get(), true);
	Shader sha;
	Shader line_sha("Shaders/line.vert", "Shaders/line.frag");
	Shader quad_sha("Shaders/quad.vert", "Shaders/quad.frag");
	Shader skybox_sha("Shaders/skybox.vert", "Shaders/skybox.frag");
	postprocess_.init(width, height, &quad_sha);
	skybox_.init(&skybox_sha);
	/* Apply shader */
	sha.use();
	loadContent(&sha);

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



	mainloop(win.get(), &sha);

	return 0;
}

void Core::mainloop(GLFWwindow* window, Shader* shader)
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
				if (selectedDrawable_ != nullptr)
				{
					if(selectedDrawable_->isLight())
					{
						// TODO: do light manipulation here
						ImGui::Text("Selected light with id:%d", selectedDrawable_->getID());
						BaseLight* light = static_cast<BaseLight*>(selectedDrawable_);
						ImGui::ColorEdit3("Ambient", reinterpret_cast<float*>(&light->ambient));
						ImGui::ColorEdit3("Diffuse", reinterpret_cast<float*>(&light->diffuse));
						ImGui::ColorEdit3("Specular", reinterpret_cast<float*>(&light->specular));
						switch(light->getType())
						{
						case directional:
						{DirectionalLight * dir = static_cast<DirectionalLight*>(light);
						ImGui::SliderFloat3("Direction", reinterpret_cast<float*>(&dir->local_direction), -1.f, 1.f);
						}
							break;
						case point:
						{PointLight * pt = static_cast<PointLight*>(light);
						ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&pt->local_position), -50.f, 50.f);
						ImGui::SliderFloat("Constant", &pt->constant, 0.0f, 1.0f);
						ImGui::SliderFloat("Linear", &pt->linear, 0.0f, 1.0f);
						ImGui::SliderFloat("Quadratic", &pt->quadratic, 0.0f, 1.0f); }
							break;
						case spot:
						{SpotLight * sp = static_cast<SpotLight*>(light);
						ImGui::SliderFloat3("Direction", reinterpret_cast<float*>(&sp->local_direction), -1.0f, 1.0f);
						ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&sp->local_position), -50.f, 50.f);
						ImGui::SliderFloat("Constant", &sp->constant, 0.0f, 1.0f);
						ImGui::SliderFloat("Linear", &sp->linear, 0.0f, 1.0f);
						ImGui::SliderFloat("Quadratic", &sp->quadratic, 0.0f, 1.0f); }
							break;
						}
					}
					else
					{
						Model* mdl = static_cast<Model*>(selectedDrawable_);
						ImGui::Text("Selected model with id:%d", mdl->getID());
						glm::mat4 matrix = mdl->world.getMatrix();
						glm::vec3 world(matrix[3][0], matrix[3][1], matrix[3][2]);
						world -= pos;
						ImGui::SliderFloat3("World Translation", reinterpret_cast<float*>(&world), -50.f, 50.f);
						ImGui::SliderFloat3("Translate", reinterpret_cast<float*>(&mdl->pos), -50.f, 50.f);
						ImGui::SliderFloat3("Rotate", reinterpret_cast<float*>(&mdl->rot), -glm::pi<float>(), glm::pi<float>());
						ImGui::SliderFloat3("Scale", reinterpret_cast<float*>(&mdl->scale), 0.f, 2.f);
					}
				}
				else
				{
					ImGui::Text("No model selected");
				}
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			GUI::ShowPropertyEditor(&showGui_, models_.getAllModels());
		}
		//if imgui glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);

		/* Update game time value */
		oldTime_ = newTime_;
		newTime_ = float(glfwGetTime());
		gameTime_ = newTime_ - oldTime_;
		/* Update game here */
		update(window);
		/* Render here */
		render(gameTime_, window, shader);
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
		if (selectedDrawable_ != nullptr)
			for (auto &m : selectedDrawable_->getMeshes())
				m.outline = false;
		selectedDrawable_ = nullptr;

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


		std::vector<GLubyte> data = { 0xFF, 0xFF, 0xFF, 0x0 };
		Logger::logDebug(string_format("Reading pixel for color-picking at (%d,%d)", int(mouseX), height - int(mouseY)));
		glReadPixels(int(mouseX), height - int(mouseY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		bool error = false;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::ostringstream sstr;
			sstr << "Got error while trying to read pixel for color-picking at (" << mouseX << "," << height - int(mouseY) << "): ";
			switch (err)
			{
			case GL_INVALID_ENUM:
				sstr << "GL_INVALID_ENUM";
				break;
			default:
				sstr << err;
			}
			Logger::logError(sstr.str());
			error = true;
		}
		if (!error)
		{
			const auto picked_id =
				data[0] +
				data[1] * 256 +
				data[2] * 256 * 256;
			if (picked_id == 0x00ffffff || picked_id == 0) { // Full white, must be the background !
				Logger::logDebug("Picked BG!");
				if (selectedDrawable_ != nullptr)
					for (auto &m : selectedDrawable_->getMeshes())
						m.outline = false;
				selectedDrawable_ = nullptr;
			}
			else {
				Logger::logDebug(string_format("Picked id=%d,a=%d,b=%d,g=%d,r=%d", picked_id, int(data[3]), int(data[2]), int(data[1]), int(data[0])));
				selectedDrawable_ = models_.getModel(picked_id);
				if(selectedDrawable_==nullptr)
					for(auto light : lights_.getAllLights())
						if (light->getID() == picked_id)
						{
							selectedDrawable_ = light;
							break;
						}
				if (selectedDrawable_ != nullptr)
				{
					for (auto &mes : selectedDrawable_->getMeshes())
						mes.outline = true;
				}
			}
		}


	}
	nodes_.getNode(3)->setTransform(nodes_.getNode(3)->getTransform().rotate(glm::vec3(gameTime_, 0.0f, 0.0f)));
	lights_.getLight(1)->diffuse = glm::vec3(glm::clamp(glm::sin(float(glfwGetTime())), 0.0f, 1.0f), 0.8f, 0.8f);
	lights_.getLight(1)->specular = glm::vec3(1.0f, glm::clamp(glm::sin(float(glfwGetTime())), 0.0f, 1.0f), 1.0f);
	//nodes_.getNode(1)->setTransform(nodes_.getNode(1)->getTransform().rotate(glm::vec3(0.0f, 0.0f, gameTime_)));
}

void Core::loadContent(Shader* shader)
{
	std::ifstream content_manifest_file("level.map");
	std::string line;
	auto count = 0;
	if (!content_manifest_file)
	{
		Logger::logError("Could not open file level.map!");
		content_manifest_file.close();
		return;
	}
	//Assert version
	getline(content_manifest_file, line);
	count++;
	if (line != "PAG6_1")
	{
		Logger::logError(string_format("File level.map is not supported! Line:%d", count));
		content_manifest_file.close();
		return;
	}
	while (content_manifest_file.good())
	{
		getline(content_manifest_file, line);
		count++;
		auto x = split(line, ':');
		if (x[0] == "MDL")
		{
			// Assert size 5
			if (x.size() != 5)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			Model* mdl = new Model(x[1]);
			models_.addModel(mdl);
			nodes_.addNode(new GraphNode(mdl));
			glm::vec3 pos, rot, scale;
			auto pos_str = split(x[2], ',');
			if (pos_str.size() == 0)
				pos = glm::vec3(0.0f);
			else if (pos_str.size() == 1)
				pos = glm::vec3(atof(pos_str[0].c_str()));
			else if (pos_str.size() == 3)
				pos = glm::vec3(atof(pos_str[0].c_str()), atof(pos_str[1].c_str()), atof(pos_str[2].c_str()));
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			auto rot_str = split(x[3], ',');
			if (rot_str.size() == 0)
				rot = glm::vec3(0.0f);
			else if (rot_str.size() == 1)
				rot = glm::vec3(glm::radians(atof(rot_str[0].c_str())));
			else if (rot_str.size() == 3)
			{
				float f1 = atof(rot_str[0].c_str());
				float f2 = atof(rot_str[1].c_str());
				float f3 = atof(rot_str[2].c_str());
				rot = glm::vec3(glm::radians(f1), glm::radians(f2), glm::radians(f3));
			}
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			auto scale_str = split(x[4], ',');
			if (scale_str.size() == 0)
				scale = glm::vec3(1.0f);
			else if (scale_str.size() == 1)
				scale = glm::vec3(atof(scale_str[0].c_str()));
			else if (scale_str.size() == 3)
			{
				float f1 = atof(scale_str[0].c_str());
				float f2 = atof(scale_str[1].c_str());
				float f3 = atof(scale_str[2].c_str());
				scale = glm::vec3(f1, f2, f3);
			}
			else
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			nodes_.getAllNodes().back()->setTransform(nodes_.getAllNodes().back()->getTransform().translate(pos).rotate(rot).scale(scale));
			root_.addChild(nodes_.getAllNodes().back());
		}
		else if (x[0] == "DIL")
		{
			// Assert size 5
			if (x.size() != 5)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			glm::vec3 val[4];
			for (auto i = 0; i < 4; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return;
				}
			}
			lights_.addLight(new DirectionalLight(val[0], val[1], val[2], val[3]));
			nodes_.addNode(new GraphNode(lights_.getAllLights().back()));
			root_.addChild(nodes_.getAllNodes().back());
			lights_.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "POL")
		{
			// Assert size 8
			if (x.size() != 8)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			glm::vec3 val[4];
			for (auto i = 0; i < 4; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return;
				}
			}
			float vf[3];
			for (auto i = 0; i < 3; i++)
			{
				auto str = split(x[i + 5], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						vf[i] = 0.0f;
					else
					{
						vf[i] = atof(str[0].c_str());
					}
				}
				else
				{
					Logger::logError(string_format("File level.map is not supported! Line:%d", count));
					content_manifest_file.close();
					return;
				}
			}
			lights_.addLight(new PointLight(val[0], val[1], val[2], val[3], vf[0], vf[1], vf[2]));
			nodes_.addNode(new GraphNode(lights_.getAllLights().back()));
			root_.addChild(nodes_.getAllNodes().back());
			lights_.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "SPL")
		{
			// Assert size 11
			if (x.size() != 11)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}
			glm::vec3 val[5];
			for (auto i = 0; i < 5; i++)
			{
				auto str = split(x[i + 1], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						val[i] = glm::vec3(0.0f);
					else
					{
						val[i] = glm::vec3(atof(str[0].c_str()));
					}
				}
				else if (str.size() == 3)
				{
					val[i] = glm::vec3(atof(str[0].c_str()), atof(str[1].c_str()), atof(str[2].c_str()));
				}
				else
				{
					Logger::logError("File level.map is not supported!");
					content_manifest_file.close();
					return;
				}
			}
			float vf[5];
			for (auto i = 0; i < 5; i++)
			{
				auto str = split(x[i + 6], ',');
				if (str.size() == 1)
				{
					if (str[0].empty())
						vf[i] = 0.0f;
					else
					{
						vf[i] = atof(str[0].c_str());
					}
				}
				else
				{
					Logger::logError("File level.map is not supported!");
					content_manifest_file.close();
					return;
				}
			}
			lights_.addLight(new SpotLight(val[0], val[1], val[2], val[3], val[4], glm::cos(glm::radians(vf[0])), glm::cos(glm::radians(vf[1])), vf[2], vf[3], vf[4]));
			nodes_.addNode(new GraphNode(lights_.getAllLights().back()));
			root_.addChild(nodes_.getAllNodes().back());
			lights_.getAllLights().back()->setupShader(shader);
		}
		else if (x[0] == "SKY")
		{
			// Assert size 7
			if (x.size() != 7)
			{
				Logger::logError(string_format("File level.map is not supported! Line:%d", count));
				content_manifest_file.close();
				return;
			}

			vector<std::string> faces;
			for (auto i = 1; i < 7; i++)
			{
				faces.push_back(x[i]);
			}
			skybox_.loadCubemap(faces);
		}
		else if (x[0][0] == '#')continue;
		else
		{
			Logger::logError(string_format("File level.map is not supported! Line:%d", count));
			content_manifest_file.close();
			return;
		}
	}
	content_manifest_file.close();
}

void Core::render(float tpf, GLFWwindow* window, Shader* shader)
{
	// first pass
	//glBindFramebuffer(GL_FRAMEBUFFER, postprocess_.getFramebuffer());
	/* Clear the color buffer & depth buffer*/
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto const wvp_changed = cam_.getCameraChanged();
	if (wvp_changed)
		wvp_.setMatrix(cam_.getWVPMatrix(window));
	shader->use();
	shader->setVec3("viewPos", cam_.getCameraPos());
	root_.render(wvp_, Transform::origin(), wvp_changed, drawColor_, showGui_);
	if (!drawColor_) skybox_.drawSkybox(cam_.getSkyboxMatrix());
	// second pass
	//glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//postprocess_.render();

	if (showGui_)
		ImGui::Render();
}

void Core::windowSizeCallback(GLFWwindow * window, int const width, int const height)
{
	glViewport(0, 0, width, height);
	ref_->postprocess_.update(width, height);
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

Core::~Core()
{
	nodes_.clear();
	models_.clear();
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