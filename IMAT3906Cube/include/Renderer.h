#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer(unsigned int sW, unsigned int sH);
	void renderScene(Shader& shader, Camera cam);

private:
	void renderCubes(Shader& shader);
	void renderFloor(Shader& shader);

	void createCube();
	void createFloor();

	unsigned int cubeVAO, floorVAO, cubeVBO, cubeEBO, floorEBO, floorVBO;

	unsigned int screenW, screenH;

	const float floorSize = 5.0f;
	const float floorLevel = -2.0f;
	const glm::vec3 cubeColor = glm::vec3(1.0, 0.4, 0.4);
	const glm::vec3 floorColor = glm::vec3(0.1, 0.3, 0.3);
};
#endif