#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

class Floor
{
public:
	void createFloor();
	const glm::vec3 floorColor = glm::vec3(0.1, 0.3, 0.3);
	unsigned int floorVAO, floorEBO, floorVBO;
private:
	const float floorSize = 5.0f;
	const float floorLevel = -2.0f;

};