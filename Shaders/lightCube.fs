#version 410 core

out vec4 FragColor;
uniform vec3 objectCol;


void main()
{
	FragColor = vec4(objectCol, 1.0);
}