#include "..\include\normalMapper.h"

normalMapper::normalMapper()
{

}

void normalMapper::calculateTanAndBitan(float* vertexData, int length, unsigned int* indicesData, int indLength)
{
	extractVertices(vertexData, length);
	computeTanAndBitan(indicesData, indLength);
	unPackVertices();
}

std::vector<float> normalMapper::getUpdatedVertexData()
{
	return updatedVertexData;
}

void normalMapper::unPackVertices()
{
	for (vertex v : vertices)
	{
		int stride = 14;
		float vertexData[] = {
			v.pos.x, v.pos.y, v.pos.z,
			v.normal.x, v.normal.y, v.normal.z,
			v.uv.x, v.uv.y,
			v.tan.x, v.tan.y, v.tan.z,
			v.biTan.x, v.biTan.y, v.biTan.z
		};
		for (int i = 0; i < stride; i++)
		{
			updatedVertexData.push_back(vertexData[i]);
		}
	}
}

void normalMapper::extractVertices(float* vertexData, int length)
{
	vertex V;
	for (int i = 0; i < length; i = i + 8)
	{
		V.pos = glm::vec3(vertexData[i], vertexData[i + 1], vertexData[i + 2]);
		V.normal = glm::vec3(vertexData[i + 3], vertexData[i + 4], vertexData[i + 5]);
		V.uv = glm::vec2(vertexData[i + 6], vertexData[i + 7]);
		V.tan = glm::vec3(0.0f, 0.0f, 0.0f);
		V.biTan = glm::vec3(0.0f, 0.0f, 0.0f);
		vertices.push_back(V);
	}
}

void normalMapper::computeTanAndBitan(unsigned int* indicesData, int indLength)
{
	for (int i = 0; i < indLength; i = i + 3)
	{
		vertex& a = vertices.at(indicesData[i]);
		vertex& b = vertices.at(indicesData[i + 1]);
		vertex& c = vertices.at(indicesData[i + 2]);

		//edges of traingle (delta p)
		glm::vec3 deltaPos1 = b.pos - a.pos;
		glm::vec3 deltaPos2 = c.pos - a.pos;
		//differences in uv
		glm::vec2 deltaUV1 = b.uv - a.uv;
		glm::vec2 deltaUV2 = c.uv - a.uv;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 thisTangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 thisBitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		a.tan = a.tan + thisTangent;
		a.biTan = a.biTan + thisBitangent;

		b.tan = b.tan + thisTangent;
		b.biTan = b.biTan + thisBitangent;

		c.tan = c.tan + thisTangent;
		c.biTan = c.biTan + thisBitangent;

		for (vertex& v : vertices)
		{
			v.tan = glm::normalize(v.tan);
			v.biTan = glm::normalize(v.biTan);
		}
	}
}
