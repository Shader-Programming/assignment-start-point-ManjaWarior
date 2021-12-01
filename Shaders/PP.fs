#version 410 core

out vec4 FragColor;

uniform sampler2D image;

in vec2 uv;

void main()
{
    //vec4 result = texture(image, uv);
    //float avg = (result.x + result.y + result.z) / 3;
    //FragColor = vec4(vec3(avg), 1.0);//this is for greyscale

    vec3 result = texture(image, uv).rgb;
    //result = result/ (result + vec3(1.0f));//tone mapping
    FragColor = vec4(result, 1.0);

    //FragColor = texture(image, uv); //normal screen no post processing
}