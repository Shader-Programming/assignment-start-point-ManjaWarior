#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Floor.h"
#include "Quad.h"

class Renderer
{
public:
	Renderer(unsigned int sW, unsigned int sH);
	void renderScene(Shader& shader, Shader& floorShader, Camera& cam);

	void drawQuad(Shader& shader, unsigned int& textureObj);


private:
	void renderCubes(Shader& shader);
	void renderFloor(Shader& shader);

	void loadTextureFiles();
	unsigned int loadTexture(char const* path);

	Cube cube1;
	Floor floor1;
	Quad quad1;

	unsigned int screenW, screenH;

	unsigned int cubeDiffuse, cubeSpec, cubeNormal, floorDiffuse, floorSpec, floorNormal, floorDisplacement;
};
#endif