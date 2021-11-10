#ifndef NORMALMAPPER_H
#define NORMALMAPPER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>

struct vertex
{
	glm::vec3 pos;
	glm::vec3 normal; 
	glm::vec2 uv;
	glm::vec3 tan;
	glm::vec3 biTan;
};

class normalMapper
{
public:
	normalMapper();
	void calculateTanAndBitan(float* vertexData, int length, unsigned int* indicesData, int indLength);
	std::vector<float> getUpdatedVertexData();

private:
	void unPackVertices();
	void extractVertices(float* vertexData, int length);
	void computeTanAndBitan(unsigned int* indicesData, int indLength);
	std::vector<vertex> vertices;
	std::vector<float> updatedVertexData;
};

#endif