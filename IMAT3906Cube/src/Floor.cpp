#include "..\include\Floor.h"

void Floor::createFloor()
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
