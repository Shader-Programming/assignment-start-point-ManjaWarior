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
#include "Cube.h"
#include "Floor.h"

class Renderer
{
public:
	Renderer(unsigned int sW, unsigned int sH);
	void renderScene(Shader& shader, Camera cam);

private:
	void renderCubes(Shader& shader);
	void renderFloor(Shader& shader);

	Cube cube1;
	Floor floor1;

	unsigned int screenW, screenH;

};
#endif