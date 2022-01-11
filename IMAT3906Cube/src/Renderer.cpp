#include "Renderer.h"

Renderer::Renderer(unsigned int sW, unsigned int sH)
{
	skyBox.createSkyBox();

	cube1.createCube();
	floor1.createFloor();
	quad1.createQuad();
	loadTextureFiles();
	screenW = sW;
	screenH = sH;
}

void Renderer::renderScene(Shader& shader, Shader& floorShader, Shader& lightCubeShader, Shader& skyBoxShader, Camera& camera)
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

	lightCubeShader.use();
	lightCubeShader.setMat4("projection", projection);
	lightCubeShader.setMat4("view", view);
	lightCubeShader.setMat4("model", model);
	renderLightCubes(lightCubeShader);

	skyBoxShader.use();
	skyBoxShader.setMat4("projection", projection);
	skyBoxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	skyBox.renderSkyBox(skyBoxShader);
}

void Renderer::renderCubes(Shader& shader)
{
	shader.use();

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

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1., 5., 1.5));
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

	glBindVertexArray(floor1.floorVAO);  // bind and draw floor
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::renderLightCubes(Shader& shader)
{
	shader.use();
	shader.setVec3("objectCol", glm::vec3(1.0, 1.0, 1.0));
	glBindVertexArray(cube1.cubeVAO);  // bind and draw cube
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 model2 = glm::mat4(1.0f);
	glm::mat4 model3 = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.25f));
	model = glm::translate(model, glm::vec3(2.0, 3.0, -10.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	shader.setVec3("objectCol", glm::vec3(0.0, 1.0, 0.0));
	model2 = glm::translate(model2, glm::vec3(-4.0, 0.0, 1.0));
	model2 = glm::scale(model2, glm::vec3(0.25f));
	shader.setMat4("model", model2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	shader.setVec3("objectCol", glm::vec3(1.0, 0.0, 0.0));
	model3 = glm::translate(model3, glm::vec3(4.0, 0.0, 1.0));
	model3 = glm::scale(model3, glm::vec3(0.25f));
	shader.setMat4("model", model3);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}


void Renderer::drawQuad(Shader& shader, unsigned int& textureObj)
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glBindVertexArray(quad1.quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::drawQuad(Shader& shader, unsigned int& textureObj, unsigned int textureObj2)
{
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureObj2);
	glBindVertexArray(quad1.quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::loadTextureFiles()
{
	cubeDiffuse = loadTexture("..\\resources\\SampleTextures\\metalPlate\\diffuse.jpg");
	cubeSpec = loadTexture("..\\resources\\SampleTextures\\metalPlate\\specular.jpg");
	cubeNormal = loadTexture("..\\resources\\SampleTextures\\metalPlate\\normal.jpg");
	floorDiffuse = loadTexture("..\\resources\\SampleTextures\\Brick\\Brick_Wall_017_SD\\brickWallDiffuse.jpg");
	floorSpec = loadTexture("..\\resources\\SampleTextures\\Brick\\Brick_Wall_017_SD\\brickWallSpecular.jpg");
	floorNormal = loadTexture("..\\resources\\SampleTextures\\Brick\\Brick_Wall_017_SD\\brickWallNormal.jpg");
	floorDisplacement = loadTexture("..\\resources\\SampleTextures\\Brick\\Brick_Wall_017_SD\\brickWallDisplacement.png");
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
