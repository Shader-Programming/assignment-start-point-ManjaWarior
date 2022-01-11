#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	texCoords = aPos;
	vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;//this makes it so the skybox will not be rendered where there are no objects visible
	//makes it slightly more efficient instead of rendering everything 
}