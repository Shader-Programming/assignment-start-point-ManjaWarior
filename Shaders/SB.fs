#version 330 core
//out vec4 FragColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 brightColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, texCoords);
	brightColor = vec4(vec3(0.0), 1.0);
}