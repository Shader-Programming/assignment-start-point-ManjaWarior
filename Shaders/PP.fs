#version 410 core

out vec4 FragColor;

uniform sampler2D image;

in vec2 uv;

void main()
{
    vec4 result = texture(image, uv);
    float avg = (result.x + result.y + result.z) / 3;
    FragColor = vec4(vec3(avg), 1.0);
}