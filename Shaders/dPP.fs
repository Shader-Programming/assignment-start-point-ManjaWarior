#version 330 core

out vec4 FragColor;

float LinearizeDepth(float depth) ;

in vec2 uv;

uniform sampler2D depthMap;

const float near_plane = 1;
const float far_plane = 1000;

void main()
{             
 
    float depth = texture(depthMap,uv).r ;
    FragColor = vec4(vec3(LinearizeDepth(depth) / far_plane), 1.0); // perspective
    
}

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}