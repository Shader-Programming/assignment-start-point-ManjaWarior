#version 410 core

out vec4 FragColor;

uniform sampler2D image;
uniform int horz;

in vec2 uv;

float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

int r = 5;
float bias = 1.f;

void main()
{
    vec2 textureOffset = 1.0/textureSize(image, 0);
    vec3 color = texture(image, uv).rgb * (weights[0]*bias);
    if(horz == 1)
    {
        for(int i = 1; i < r; i++)
        {
            color = color + texture(image, uv + vec2(textureOffset.x * i, 0.0)).rgb * (weights[i]*bias);
            color = color + texture(image, uv - vec2(textureOffset.x * i, 0.0)).rgb * (weights[i]*bias);
        }
    }
    else
    {
        for(int i = 1; i < r; i++)
        {
            color = color + texture(image, uv + vec2(0.0, textureOffset.y * i)).rgb * (weights[i]*bias);
            color = color + texture(image, uv - vec2(0.0, textureOffset.y * i)).rgb * (weights[i]*bias);
        }
    }

    FragColor = vec4(color, 1.0);
}