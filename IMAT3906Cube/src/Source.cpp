#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"

#include<string>
#include <iostream>
#include <numeric>



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//own functions
void setUniforms(Shader& shader);
unsigned int loadTexture(char const* path);

// camera
Camera camera(glm::vec3(0, 0, 9));


float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int floorVBO, cubeVBO, floorEBO, cubeEBO, cubeVAO, floorVAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int diffTexture = loadTexture("..\\resources\\SampleTextures\\metalPlate\\diffuse.jpg");
	unsigned int specTexture = loadTexture("..\\resources\\SampleTextures\\metalPlate\\specular.jpg");


	// simple vertex and fragment shader 
	Shader shader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs");
	shader.use();
	glEnable(GL_DEPTH_TEST);
	//textures
	shader.setInt("diffuseTexture", 0);
	shader.setInt("specularTexture", 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specTexture);


	//Renderer
	Renderer renderer(SCR_WIDTH, SCR_HEIGHT);

	setUniforms(shader);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		shader.setVec3("sLight[0].position", camera.Position);
		shader.setVec3("sLight[0].direction", (camera.Front));

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // what happens if we change to GL_LINE?

		renderer.renderScene(shader, camera);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void setUniforms(Shader& shader)
{
	//directional light
	glm::vec3 lightDirection = glm::vec3(0, -1, 0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

	shader.setVec3("lightCol", lightColor);
	shader.setVec3("lightDir", lightDirection);

	//point light
	glm::vec3 pLightPos = glm::vec3(0.0, -1.0, 1.0);
	glm::vec3 pLightCol = glm::vec3(5.0, 0.0, 0.0);
	glm::vec3 pLightPos2 = glm::vec3(-4.0, 0.0, 1.0);
	glm::vec3 pLightPos3 = glm::vec3(4.0, 0.0, 1.0);
	float Kc = 1.0f;
	float Kl = 0.22f;
	float Ke = 0.2f;

	shader.setVec3("pLight[0].position", pLightPos);
	shader.setVec3("pLight[0].color", pLightCol);
	shader.setFloat("pLight[0].Kc", Kc);
	shader.setFloat("pLight[0].Kl", Kl);
	shader.setFloat("pLight[0].Ke", Ke);
	shader.setVec3("pLight[1].position", pLightPos2);
	shader.setVec3("pLight[1].color", pLightCol);
	shader.setFloat("pLight[1].Kc", Kc);
	shader.setFloat("pLight[1].Kl", Kl);
	shader.setFloat("pLight[1].Ke", Ke);
	shader.setVec3("pLight[2].position", pLightPos3);
	shader.setVec3("pLight[2].color", pLightCol);
	shader.setFloat("pLight[2].Kc", Kc);
	shader.setFloat("pLight[2].Kl", Kl);
	shader.setFloat("pLight[2].Ke", Ke);


	//spot light

	shader.setVec3("sLight[0].color", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setFloat("sLight[0].kC", 1.0f);
	shader.setFloat("sLight[0].Kl", 0.027f);
	shader.setFloat("sLight[0].Ke", 0.0028f);
	shader.setFloat("sLight[0].innerRad", glm::cos(glm::radians(12.5f)));
	shader.setFloat("sLight[0].outerRad", glm::cos(glm::radians(17.5f)));

	shader.setVec3("sLight[1].position", glm::vec3(2.5, 3.0, -3.5));
	shader.setVec3("sLight[1].direction", glm::vec3(0.0, -1.0, 0.0));
	shader.setVec3("sLight[1].color", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setFloat("sLight[1].kC", 1.0f);
	shader.setFloat("sLight[1].Kl", 0.027f);
	shader.setFloat("sLight[1].Ke", 0.0028f);
	shader.setFloat("sLight[1].innerRad", glm::cos(glm::radians(12.5f)));
	shader.setFloat("sLight[1].outerRad", glm::cos(glm::radians(17.5f)));

}

unsigned int loadTexture(char const* path)
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


