#include "Transform.h"



Transform::Transform() : transform_(glm::mat4(1.0f))
{
}

Transform::Transform(glm::mat4 matrix) : transform_(matrix)
{
}

Transform Transform::origin()
{
	return Transform();
}

Transform Transform::combine(Transform other) const
{
	return Transform(other.getMatrix() * transform_);
}

Transform Transform::translate(const glm::vec3 pos) const
{
	return Transform(glm::translate(transform_, pos));
}

Transform Transform::rotate(const glm::vec3 rot) const
{
	return Transform(glm::rotate(glm::rotate(glm::rotate(transform_, rot.x, glm::vec3(1.0f, 0.0f, 0.0f)), rot.y, glm::vec3(0.0f, 1.0f, 0.0f)), rot.z, glm::vec3(0.0f, 0.0f, 1.0f)));
}

Transform Transform::scale(const glm::vec3 scale) const
{
	return Transform(glm::scale(transform_, scale));
}

glm::mat4 Transform::getMatrix() const
{
	return transform_;
}

void Transform::setMatrix(const glm::mat4 matrix)
{
	transform_ = matrix;
}


Transform::~Transform()
{
}
