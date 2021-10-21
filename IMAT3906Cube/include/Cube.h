#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

class Cube
{
public:
	void createCube();
	const glm::vec3 cubeColor = glm::vec3(1.0, 0.4, 0.4);
	unsigned int cubeVAO, cubeVBO, cubeEBO;
};