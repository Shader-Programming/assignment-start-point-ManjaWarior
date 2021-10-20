#include "Renderer.h"

Renderer::Renderer(unsigned int sW, unsigned int sH)
{
	createCube();
	createFloor();
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

	renderCubes(shader);
	renderFloor(shader);
}

void Renderer::renderCubes(Shader& shader)
{
	shader.setVec3("objectCol", cubeColor);
	glBindVertexArray(cubeVAO);  // bind and draw cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);//cube isn't drawing?

	glm::mat4 model = glm::translate(model, glm::vec3(0.0, 0.0, 5.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 0.0, -5.0));
	//model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(2.0, 2.0, 2.0));
	model = glm::scale(model, glm::vec3(2.0));
	shader.setMat4("model", model);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Renderer::renderFloor(Shader& shader)
{
	shader.setVec3("objectCol", floorColor);
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);

	shader.setVec3("objectCol", floorColor);
	glBindVertexArray(floorVAO);  // bind and draw floor
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::createCube()
{
	float cubeVertices[] = {
		//back
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // 0
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   //1
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			//front
				 -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   //4
				  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				 -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				 //left
					 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  //8
					 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
					 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
					 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
					 //right
						   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   //12
						   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
						   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
						   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
						   //bottom
								-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  //16
								 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
								 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
								-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
								//top	
									-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, //20
									 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
									 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
									-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,

	};

	unsigned int cubeIndices[] = {
		1,2,3,
		1,3,0,

		5,6,7,
		5,7,4,

		11,8,9,
		11,9,10,

		15,12,13,
		15,13,14,

		23,22,21,
		23,21,20

	};
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	// fill VBO with vertex data
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// fill EBO with index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Renderer::createFloor()
{
	float floorVertices[] = {
		 -floorSize, floorLevel,  -floorSize, 0.0, 1.0, 0.0,
		 floorSize, floorLevel,   -floorSize, 0.0, 1.0, 0.0,
		 floorSize, floorLevel,  floorSize, 0.0, 1.0, 0.0,
		-floorSize, floorLevel,  floorSize, 0.0, 1.0, 0.0,
	};

	unsigned int floorIndices[] = {
		3,2,1,
		3,1,0
	};

	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glGenBuffers(1, &floorEBO);

	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
