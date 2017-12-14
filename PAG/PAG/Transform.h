#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform
{
private:
	glm::mat4 transform_;
public:
	Transform();
	Transform(glm::mat4 matrix);
	static Transform origin();
	Transform combine(Transform other) const;
	Transform translate(glm::vec3 pos) const;
	Transform rotate(glm::vec3 rot) const;
	Transform scale(glm::vec3 scale) const;
	glm::mat4 getMatrix() const;
	void setMatrix(glm::mat4 matrix);
	~Transform();
};

