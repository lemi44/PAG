#include "Camera.h"
#include <glm/gtc/matrix_transform.inl>


Camera::Camera() : changed_(true), fov_(glm::radians(45.f)), world(1.0f)
{
	cameraPos_ = glm::vec3(1.5f, 1.f, 1.5f);
	cameraUp_ = glm::vec3(0.f, 1.f, 0.f);
	cameraFront_ = glm::vec3(0.f, 0.f, -1.f);
}

glm::mat4 Camera::getWVPMatrix(GLFWwindow* window)
{
	if (changed_)
	{
		/* Set view matrix */
		view = glm::lookAt(cameraPos_, cameraPos_ + cameraFront_, cameraUp_);
		int w;
		int h;
		glfwGetWindowSize(window, &w, &h);
		projection = glm::perspective(fov_, float(w) / float(h), 0.001f, 100.0f);
		/* Set MVP matrix */
		wvp_ = projection * view * world;
		changed_ = false;
	}
	return wvp_;
}

glm::mat4 Camera::getSkyboxMatrix() const
{
	return projection * glm::mat4(glm::mat3(view));
}

glm::vec3 Camera::getCameraPos() const
{
	return cameraPos_;
}

glm::vec3 Camera::getCameraFront() const
{
	return cameraFront_;
}

glm::vec3 Camera::getCameraUp() const
{
	return cameraUp_;
}

void Camera::setCameraPos(glm::vec3 const pos)
{
	cameraPos_ = pos;
	changed_ = true;
}

void Camera::setCameraFront(glm::vec3 const front)
{
	cameraFront_ = front;
	changed_ = true;
}
void Camera::setPitch(double const pitch)
{
	pitch_ = pitch;
	changed_ = true;
}
void Camera::setYaw(double const yaw)
{
	yaw_ = yaw;
	changed_ = true;
}
void Camera::setRoll(double const roll)
{
	roll_ = roll;
	changed_ = true;
}
void Camera::setFoV(const GLfloat fov)
{
	fov_ = fov;
	changed_ = true;
}
bool Camera::getCameraChanged() const
{
	return changed_;
}

double Camera::getPitch() const
{
	return pitch_;
}

double Camera::getYaw() const
{
	return yaw_;
}

double Camera::getRoll() const
{
	return roll_;
}

GLfloat Camera::getFoV() const
{
	return fov_;
}

Camera::~Camera()
{
}

glm::vec3 Camera::getRight() {
	return glm::vec3(wvp_[0][0], wvp_[1][0], wvp_[2][0]);
}