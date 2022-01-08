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

const unsigned int SH_WIDTH = 1024;
const unsigned int SH_HEIGHT = 1024;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

//light position
glm::vec3 lightDir = glm::vec3(1.0f, -3.f, -1.f);

float orthoSize = 10;

glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, -orthoSize, 2 * orthoSize);
glm::mat4 lightView = glm::lookAt(lightDir * glm::vec3(-1.f), glm::vec3(0.f), glm::vec3(0.0, 1.0, 0.0));
glm::mat4 lightSpaceMatrix = lightProjection * lightView;

//own functions
void setUniforms(Shader& shader, Shader& shader2, Shader& shader3, Shader& shader4, Shader& shader5, Shader& shader6, Shader& shader7);//updates all shaders before running
void updatePerFrameUniforms(Shader& cubeShader, Shader& floorShader, Camera camera, bool DL, bool  PL, bool SL, int map, bool NM);
void setFBOColour();
void setFBODepth();
void setFBOColourAndDepth();
void setFBOBlur();

// camera
Camera camera(glm::vec3(0, 0, 9));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//own variables
bool PL, SL;
bool DL = true;
bool NM = true;
unsigned int myFBO, myFBODepth, depthAttachment;
unsigned int FBOBlur, blurredTexture;
unsigned int colourAttachment[3];
unsigned int depthMap;
unsigned int map;

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

	// simple vertex and fragment shader and post processing shaders
	Shader cubeShader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs");
	Shader floorShader("..\\shaders\\floorVert.vs", "..\\shaders\\floorFrag.fs");

	Shader postProcess("..\\shaders\\PP.vs", "..\\shaders\\PP.fs");
	Shader depthPostProcess("..\\shaders\\PP.vs", "..\\shaders\\dPP.fs");
	Shader blurShader("..\\shaders\\PP.vs", "..\\shaders\\blur.fs");
	Shader bloomShader("..\\shaders\\PP.vs", "..\\shaders\\BloomShader.fs");

	Shader lightCubeShader("..\\shaders\\lightCube.vs", "..\\shaders\\lightCube.fs");

	Shader shadowMapShader("..\\shaders\\SM.vs", "..\\shaders\\SM.fs");
	Shader skyBoxShader("..\\shaders\\SB.vs", "..\\shaders\\SB.fs");

	setUniforms(cubeShader, floorShader, postProcess, depthPostProcess, blurShader, bloomShader, skyBoxShader);

	setFBOColourAndDepth();
	setFBOBlur();
	setFBODepth();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		updatePerFrameUniforms(cubeShader, floorShader, camera, DL, PL, SL, map, NM);

		processInput(window);
		//1st pass to FBO for shadow map
		glBindFramebuffer(GL_FRAMEBUFFER, myFBODepth);
		glViewport(0, 0, SH_WIDTH, SH_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glCullFace(GL_FRONT);
		renderer.renderCubes(shadowMapShader);
		renderer.renderFloor(shadowMapShader);
		glCullFace(GL_BACK);//CullFace method solves peter panning issues

		//2nd pass with normal shader and perspective projection, also need to add depth map
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthAttachment);

		renderer.renderScene(cubeShader, floorShader, lightCubeShader, skyBoxShader, camera);

		//bind colour to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, myFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		renderer.renderScene(cubeShader, floorShader, lightCubeShader, skyBoxShader, camera);

		//blur colour attachment
		glBindFramebuffer(GL_FRAMEBUFFER, FBOBlur);
		glDisable(GL_DEPTH_TEST);
		blurShader.use();
		blurShader.setInt("horz", 1);
		renderer.drawQuad(blurShader, colourAttachment[1]);
		blurShader.setInt("horz", 0);
		renderer.drawQuad(blurShader, blurredTexture);

		//3rd pass to render screen - QUAD VAO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);//default FBO
		glDisable(GL_DEPTH_TEST);
		//renderer.drawQuad(postProcess, colourAttachment[0]);
		renderer.drawQuad(bloomShader, colourAttachment[0], blurredTexture);//should apply bloom to the normal window
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
			renderer.drawQuad(depthPostProcess, depthAttachment);//shows the depth map from light perspective


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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//normal map
	{
		if (map == 1) map = 0;
		else map = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)//directional light
	{
		if (DL == true) DL = false;
		else DL = true;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)//point light
	{
		if (PL == true) PL = false;
		else PL = true;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)//spot light
	{
		if (SL == true) SL = false;
		else SL = true;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)//parallax mapping
	{
		if (NM == true) NM = false;
		else NM = true;
	}
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

void updatePerFrameUniforms(Shader& cubeShader, Shader& floorShader, Camera camera, bool DL, bool  PL, bool SL, int map, bool NM)
{
	cubeShader.use();
	cubeShader.setVec3("sLight[0].position", camera.Position);
	cubeShader.setVec3("sLight[0].direction", (camera.Front));
	cubeShader.setInt("map", map);
	cubeShader.setBool("DL", DL);
	cubeShader.setBool("PL", PL);
	cubeShader.setBool("SL", SL);
	floorShader.use();
	floorShader.setVec3("sLight[0].position", camera.Position);
	floorShader.setVec3("sLight[0].direction", (camera.Front));
	floorShader.setInt("map", map);
	floorShader.setBool("DL", DL);
	floorShader.setBool("PL", PL);
	floorShader.setBool("SL", SL);
	floorShader.setBool("NM", NM);
}

void setUniforms(Shader& cubeShader, Shader& floorShader, Shader& postProcess, Shader& depthPostProcess, Shader& blurShader, Shader& bloomShader, Shader& skyBoxShader)
{
	float bloomMinBrightness = 0.95f;

	cubeShader.use();
	//directional light
	glm::vec3 lightDirection = glm::vec3(0, -1, 0);
	glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

	cubeShader.setVec3("lightCol", lightColor);
	cubeShader.setVec3("lightDir", lightDirection);
	cubeShader.setFloat("bloomBrightness", bloomMinBrightness);

	//cube textures
	cubeShader.setInt("diffuseTexture", 0);
	cubeShader.setInt("normalMap", 1);
	cubeShader.setInt("specularTexture", 2);
	cubeShader.setInt("depthMap", 4);
	cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	//point light
	glm::vec3 pLightPos = glm::vec3(2.0, 3.0, -10.0);
	glm::vec3 pLightCol = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 pLightCol2 = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 pLightCol3 = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 pLightPos2 = glm::vec3(-4.0, 0.0, 1.0);
	glm::vec3 pLightPos3 = glm::vec3(4.0, 0.0, 1.0);
	float Kc = 1.0f;
	float Kl = 0.002f;
	float Ke = 0.002f;

	cubeShader.setVec3("pLight[0].position", pLightPos);
	cubeShader.setVec3("pLight[0].color", pLightCol);
	cubeShader.setFloat("pLight[0].Kc", Kc);
	cubeShader.setFloat("pLight[0].Kl", Kl);
	cubeShader.setFloat("pLight[0].Ke", Ke);
	cubeShader.setVec3("pLight[1].position", pLightPos2);
	cubeShader.setVec3("pLight[1].color", pLightCol2);
	cubeShader.setFloat("pLight[1].Kc", Kc);
	cubeShader.setFloat("pLight[1].Kl", Kl);
	cubeShader.setFloat("pLight[1].Ke", Ke);
	cubeShader.setVec3("pLight[2].position", pLightPos3);
	cubeShader.setVec3("pLight[2].color", pLightCol3);
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

	cubeShader.setVec3("sLight[1].position", glm::vec3(2.5, 2.0, -3.5));
	cubeShader.setVec3("sLight[1].direction", glm::vec3(0.0, -1.0, 0.0));
	cubeShader.setVec3("sLight[1].color", glm::vec3(0.1f, 0.1f, 0.1f));
	cubeShader.setFloat("sLight[1].kC", 1.0f);
	cubeShader.setFloat("sLight[1].Kl", 0.027f);
	cubeShader.setFloat("sLight[1].Ke", 0.0028f);
	cubeShader.setFloat("sLight[1].innerRad", glm::cos(glm::radians(12.5f)));
	cubeShader.setFloat("sLight[1].outerRad", glm::cos(glm::radians(17.5f)));


	//Floor Shader
	floorShader.use();
	float ambientFactor = 0.5f;
	//directional light
	floorShader.setFloat("bloomBrightness", bloomMinBrightness);
	floorShader.setVec3("lightCol", lightColor);
	floorShader.setVec3("lightDir", lightDirection);

	//floor textures
	floorShader.setInt("diffuseTexture", 0);
	floorShader.setInt("normalMap", 1);
	floorShader.setInt("specularTexture", 2); 
	floorShader.setInt("dispMap", 3);
	floorShader.setFloat("PXscale", 0.0175);
	floorShader.setInt("depthMap", 4);
	floorShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	//point light
	floorShader.setVec3("pLight[0].position", pLightPos);
	floorShader.setVec3("pLight[0].color", pLightCol);
	floorShader.setFloat("pLight[0].Kc", Kc);
	floorShader.setFloat("pLight[0].Kl", Kl);
	floorShader.setFloat("pLight[0].Ke", Ke);
	floorShader.setVec3("pLight[1].position", pLightPos2);
	floorShader.setVec3("pLight[1].color", pLightCol2);
	floorShader.setFloat("pLight[1].Kc", Kc);
	floorShader.setFloat("pLight[1].Kl", Kl);
	floorShader.setFloat("pLight[1].Ke", Ke);
	floorShader.setVec3("pLight[2].position", pLightPos3);
	floorShader.setVec3("pLight[2].color", pLightCol3);
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

	floorShader.setVec3("sLight[1].position", glm::vec3(2.5, 2.0, -3.5));
	floorShader.setVec3("sLight[1].direction", glm::vec3(0.0, -1.0, 0.0));
	floorShader.setVec3("sLight[1].color", glm::vec3(0.1f, 0.1f, 0.1f));
	floorShader.setFloat("sLight[1].kC", 1.0f);
	floorShader.setFloat("sLight[1].Kl", 0.027f);
	floorShader.setFloat("sLight[1].Ke", 0.0028f);
	floorShader.setFloat("sLight[1].innerRad", glm::cos(glm::radians(12.5f)));
	floorShader.setFloat("sLight[1].outerRad", glm::cos(glm::radians(17.5f)));


	postProcess.use();
	postProcess.setInt("image", 0);
	depthPostProcess.use();
	depthPostProcess.setInt("image", 0);
	blurShader.use();
	blurShader.setInt("image", 0);
	bloomShader.use();
	bloomShader.setInt("image", 0);
	bloomShader.setInt("bloomBlur", 1);//bloom is working
	skyBoxShader.use();
	skyBoxShader.setInt("skybox", 0);
}

void setFBOColour()
{
	glGenFramebuffers(1, &myFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, myFBO);
	glGenTextures(1, &colourAttachment[0]);
	glBindTexture(GL_TEXTURE_2D, colourAttachment[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourAttachment[0], 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//check if it is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void setFBODepth()
{
	glGenFramebuffers(1, &myFBODepth);
	glBindFramebuffer(GL_FRAMEBUFFER, myFBODepth);
	glGenTextures(1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SH_WIDTH, SH_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);//prevents shadows repeating and clamps them to the custom border
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setFBOColourAndDepth()
{
	glGenFramebuffers(1, &myFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, myFBO);
	glGenTextures(2, colourAttachment);
	//colour
	for (int i = 0; i < 3; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colourAttachment[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourAttachment[i], 0);
	}
	//depth
	glGenTextures(1, &depthAttachment);
	glBindTexture(GL_TEXTURE_2D, depthAttachment);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setFBOBlur()
{
	glGenFramebuffers(1, &FBOBlur);
	glBindFramebuffer(GL_FRAMEBUFFER, FBOBlur);
	glGenTextures(1, &blurredTexture);
	glBindTexture(GL_TEXTURE_2D, blurredTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurredTexture, 0);
}