#version 410 core

out vec4 FragColor;

uniform sampler2D image;

in vec2 uv;

void main()
{
    FragColor = texture(image, uv);
}