#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBiTan;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 posWS;
out vec2 uv;
out mat3 TBN;

void main()
{  
    gl_Position = projection * view * model*vec4(aPos,1.0);
    posWS = (model*vec4(aPos,1.0)).xyz;
    normal = (model*vec4(aNormals, 0.0)).xyz;

    vec3 T = (model*vec4(aTan,0.0)).xyz;
    vec3 B = (model*vec4(aBiTan,0.0)).xyz;
    TBN = mat3(T, B, normal);

    uv = texCoords*3.0;
}