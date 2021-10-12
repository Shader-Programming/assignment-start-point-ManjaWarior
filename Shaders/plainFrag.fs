#version 410 core

out vec4 FragColor;

in vec3 normal ;

vec3 colour = vec3(0.2f,0.5f,0.6f) ;
void main()
{    	
    FragColor = vec4(colour, 1.0f);
}

