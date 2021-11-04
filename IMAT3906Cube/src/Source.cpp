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
void setUniforms(Shader& shader, Shader& shader2);

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

	glEnable(GL_DEPTH_TEST);

	//Renderer
	Renderer renderer(SCR_WIDTH, SCR_HEIGHT);

	// simple vertex and fragment shader 
	Shader cubeShader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs");
	Shader floorShader("..\\shaders\\plainVert.vs", "..\\shaders\\floorFrag.fs");

	setUniforms(cubeShader, floorShader);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		cubeShader.use();
		cubeShader.setVec3("sLight[0].position", camera.Position);
		cubeShader.setVec3("sLight[0].direction", (camera.Front));
		floorShader.use();
		floorShader.setVec3("sLight[0].position", camera.Position);
		floorShader.setVec3("sLight[0].direction", (camera.Front));

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   // what happens if we change to GL_LINE?

		renderer.renderScene(cubeShader, floorShader, camera);
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

void setUniforms(Shader& cubeShader, Shader& floorShader)
{
	cubeShader.use();
	//directional light
	glm::vec3 lightDirection = glm::vec3(0, -1, 0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

	cubeShader.setVec3("lightCol", lightColor);
	cubeShader.setVec3("lightDir", lightDirection);

	//cube textures
	cubeShader.setInt("diffuseTexture", 0);
	cubeShader.setInt("specularTexture", 1);

	//point light
	glm::vec3 pLightPos = glm::vec3(0.0, -1.0, 1.0);
	glm::vec3 pLightCol = glm::vec3(5.0, 0.0, 0.0);
	glm::vec3 pLightPos2 = glm::vec3(-4.0, 0.0, 1.0);
	glm::vec3 pLightPos3 = glm::vec3(4.0, 0.0, 1.0);
	float Kc = 1.0f;
	float Kl = 0.22f;
	float Ke = 0.2f;

	cubeShader.setVec3("pLight[0].position", pLightPos);
	cubeShader.setVec3("pLight[0].color", pLightCol);
	cubeShader.setFloat("pLight[0].Kc", Kc);
	cubeShader.setFloat("pLight[0].Kl", Kl);
	cubeShader.setFloat("pLight[0].Ke", Ke);
	cubeShader.setVec3("pLight[1].position", pLightPos2);
	cubeShader.setVec3("pLight[1].color", pLightCol);
	cubeShader.setFloat("pLight[1].Kc", Kc);
	cubeShader.setFloat("pLight[1].Kl", Kl);
	cubeShader.setFloat("pLight[1].Ke", Ke);
	cubeShader.setVec3("pLight[2].position", pLightPos3);
	cubeShader.setVec3("pLight[2].color", pLightCol);
	cubeShader.setFloat("pLight[2].Kc", Kc);
	cubeShader.setFloat("pLight[2].Kl", Kl);
	cubeShader.setFloat("pLight[2].Ke", Ke);

	//spot light
	cubeShader.setVec3("sLight[0].color", glm::vec3(0.1f, 0.1f, 0.1f));
	cubeShader.setFloat("sLight[0].kC", 1.0f);
	cubeShader.setFloat("sLight[0].Kl", 0.027f);
	cubeShader.setFloat("sLight[0].Ke", 0.0028f);
	cubeShader.setFloat("sLight[0].innerRad", glm::cos(glm::radians(12.5f)));
	cubeShader.setFloat("sLight[0].outerRad", glm::cos(glm::radians(17.5f)));

	cubeShader.setVec3("sLight[1].position", glm::vec3(2.5, 3.0, -3.5));
	cubeShader.setVec3("sLight[1].direction", glm::vec3(0.0, -1.0, 0.0));
	cubeShader.setVec3("sLight[1].color", glm::vec3(0.1f, 0.1f, 0.1f));
	cubeShader.setFloat("sLight[1].kC", 1.0f);
	cubeShader.setFloat("sLight[1].Kl", 0.027f);
	cubeShader.setFloat("sLight[1].Ke", 0.0028f);
	cubeShader.setFloat("sLight[1].innerRad", glm::cos(glm::radians(12.5f)));
	cubeShader.setFloat("sLight[1].outerRad", glm::cos(glm::radians(17.5f)));


	//Floor Shader
	floorShader.use();
	//directional light
	floorShader.setVec3("lightCol", lightColor);
	floorShader.setVec3("lightDir", lightDirection);

	//floor textures
	floorShader.setInt("diffuseTexture", 0);
	floorShader.setInt("specularTexture", 1);

	//point light
	floorShader.setVec3("pLight[0].position", pLightPos);
	floorShader.setVec3("pLight[0].color", pLightCol);
	floorShader.setFloat("pLight[0].Kc", Kc);
	floorShader.setFloat("pLight[0].Kl", Kl);
	floorShader.setFloat("pLight[0].Ke", Ke);
	floorShader.setVec3("pLight[1].position", pLightPos2);
	floorShader.setVec3("pLight[1].color", pLightCol);
	floorShader.setFloat("pLight[1].Kc", Kc);
	floorShader.setFloat("pLight[1].Kl", Kl);
	floorShader.setFloat("pLight[1].Ke", Ke);
	floorShader.setVec3("pLight[2].position", pLightPos3);
	floorShader.setVec3("pLight[2].color", pLightCol);
	floorShader.setFloat("pLight[2].Kc", Kc);
	floorShader.setFloat("pLight[2].Kl", Kl);
	floorShader.setFloat("pLight[2].Ke", Ke);

	//spot light
	floorShader.setVec3("sLight[0].color", glm::vec3(0.1f, 0.1f, 0.1f));
	floorShader.setFloat("sLight[0].kC", 1.0f);
	floorShader.setFloat("sLight[0].Kl", 0.027f);
	floorShader.setFloat("sLight[0].Ke", 0.0028f);
	floorShader.setFloat("sLight[0].innerRad", glm::cos(glm::radians(12.5f)));
	floorShader.setFloat("sLight[0].outerRad", glm::cos(glm::radians(17.5f)));

	floorShader.setVec3("sLight[1].position", glm::vec3(2.5, 3.0, -3.5));
	floorShader.setVec3("sLight[1].direction", glm::vec3(0.0, -1.0, 0.0));
	floorShader.setVec3("sLight[1].color", glm::vec3(0.1f, 0.1f, 0.1f));
	floorShader.setFloat("sLight[1].kC", 1.0f);
	floorShader.setFloat("sLight[1].Kl", 0.027f);
	floorShader.setFloat("sLight[1].Ke", 0.0028f);
	floorShader.setFloat("sLight[1].innerRad", glm::cos(glm::radians(12.5f)));
	floorShader.setFloat("sLight[1].outerRad", glm::cos(glm::radians(17.5f)));
}




