#include "Renderer.h"

Renderer::Renderer(unsigned int sW, unsigned int sH)
{
	cube1.createCube();
	floor1.createFloor();
	loadTextureFiles();
	screenW = sW;
	screenH = sH;
}

void Renderer::renderScene(Shader& shader, Shader& floorShader, Camera& camera)
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenW / (float)screenH, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setVec3("viewPos", camera.Position);
	renderCubes(shader);

	floorShader.use();
	floorShader.setMat4("projection", projection);
	floorShader.setMat4("view", view);
	floorShader.setMat4("model", model);
	floorShader.setVec3("viewPos", camera.Position);
	renderFloor(floorShader);
}

void Renderer::renderCubes(Shader& shader)
{
	shader.use();
	//shader.setVec3("objectCol", cube1.cubeColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, cubeNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, cubeSpec);

	glBindVertexArray(cube1.cubeVAO);  // bind and draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(0.0, 0.0, 5.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0));
	model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(2.0, 2.0, 2.0));
	model = glm::scale(model, glm::vec3(2.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::renderFloor(Shader& shader)
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, floorNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, floorSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, floorDisplacement);

	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);

	//shader.setVec3("objectCol", floor1.floorColor);
	glBindVertexArray(floor1.floorVAO);  // bind and draw floor
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::loadTextureFiles()
{
	cubeDiffuse = loadTexture("..\\resources\\SampleTextures\\metalPlate\\diffuse.jpg");
	cubeSpec = loadTexture("..\\resources\\SampleTextures\\metalPlate\\specular.jpg");
	cubeNormal = loadTexture("..\\resources\\SampleTextures\\metalPlate\\normal.jpg");
	floorDiffuse = loadTexture("..\\resources\\SampleTextures\\Brick\\brickDiffuse.jpg");
	floorSpec = loadTexture("..\\resources\\SampleTextures\\Brick\\brickNormal.jpg");
	floorNormal = loadTexture("..\\resources\\SampleTextures\\Brick\\brickNormal.jpg");
	floorDisplacement = loadTexture("..\\resources\\SampleTextures\\Brick\\brickDisplacement.jpg");
}

unsigned int Renderer::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //S == x axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //T == y axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
