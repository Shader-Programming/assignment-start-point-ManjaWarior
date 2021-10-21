#include "Renderer.h"

Renderer::Renderer(unsigned int sW, unsigned int sH)
{
	cube1.createCube();
	floor1.createFloor();
	screenW = sW;
	screenH = sH;
}

void Renderer::renderScene(Shader& shader, Camera camera)
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenW / (float)screenH, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setVec3("viewPos", camera.Position);

	renderCubes(shader);
	renderFloor(shader);
}

void Renderer::renderCubes(Shader& shader)
{
	shader.use();
	shader.setVec3("objectCol", cube1.cubeColor);
	glm::mat4 model = glm::mat4(1.0f);
	glBindVertexArray(cube1.cubeVAO);  // bind and draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0.0, 0.0, 5.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0));
	//model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(2.0, 2.0, 2.0));
	//model = glm::scale(model, glm::vec3(2.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::renderFloor(Shader& shader)
{
	shader.setVec3("objectCol", floor1.floorColor);
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);

	shader.setVec3("objectCol", floor1.floorColor);
	glBindVertexArray(floor1.floorVAO);  // bind and draw floor
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


