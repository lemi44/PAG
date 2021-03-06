#include "Camera.h"
#include <glm/gtc/matrix_transform.inl>


Camera::Camera() : changed_(true), fov_(glm::radians(45.f))
{
	cameraPos_ = glm::vec3(0.0f, 2.f, 2.0f);
	cameraUp_ = glm::vec3(0.f, 1.f, 0.f);
	cameraFront_ = glm::vec3(0.f, 0.f, -1.f);
}

glm::mat4 Camera::getWVPMatrix(GLFWwindow* window)
{
	if (changed_)
	{
		updateVP(window);
		changed_ = false;
	}
	return wvp_;
}

glm::mat4 Camera::getSkyboxMatrix(GLFWwindow* window)
{
	if (changed_)
	{
		/* Set view matrix */
		updateVP(window);
		changed_ = false;
	}
	return projection * glm::mat4(glm::mat3(view));
}

glm::mat4 Camera::getProjection(GLFWwindow* window)
{
	if (changed_)
	{
		/* Set view matrix */
		updateVP(window);
		changed_ = false;
	}
	return projection;
}

glm::mat4 Camera::getView(GLFWwindow* window)
{
	if (changed_)
	{
		/* Set view matrix */
		updateVP(window);
		changed_ = false;
	}
	return view;
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

void Camera::updateVP(GLFWwindow* window)
{
	/* Set view matrix */
	view = glm::lookAt(cameraPos_, cameraPos_ + cameraFront_, cameraUp_);
	int w;
	int h;
	glfwGetWindowSize(window, &w, &h);
	projection = glm::perspective(fov_, float(w) / float(h), 0.001f, 100.0f);
	/* Set MVP matrix */
	wvp_ = projection * view;
}

glm::vec3 Camera::getRight() {
	return glm::vec3(view[0][0], view[1][0], view[2][0]);
}

glm::vec3 Camera::getUp()
{
	return glm::vec3(view[0][1], view[1][1], view[2][1]);
}
