#pragma once
#include <glm/detail/type_vec3.hpp>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>

class Camera
{
public:
	Camera();
	glm::mat4 getWVPMatrix(GLFWwindow* window);
	glm::vec3 getRight();
	glm::vec3 getCameraPos() const;
	glm::vec3 getCameraFront() const;
	glm::vec3 getCameraUp() const;
	bool getCameraChanged() const;
	double getPitch() const;
	double getYaw() const;
	double getRoll() const;
	GLfloat getFoV() const;
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	void setCameraPos(glm::vec3 pos);
	void setCameraFront(glm::vec3 front);
	void setPitch(double pitch);
	void setYaw(double yaw);
	void setRoll(double roll);
	void setFoV(GLfloat fov);
	
	~Camera();
private:
	glm::vec3 cameraPos_;
	glm::vec3 cameraUp_;
	glm::vec3 cameraFront_;
	glm::mat4 wvp_;
	glm::mat4 view_;
	glm::mat4 projection_;
	double pitch_, yaw_, roll_;
	GLfloat fov_;
	bool changed_;
};

