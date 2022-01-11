#version 410 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D image;
uniform sampler2D bloomBlur;

void main()
{
	const float gamma = 1.0f;
	vec3 hdrColor = texture(image, uv).rgb;
	vec3 bloomColor = texture(bloomBlur, uv).rgb;
	hdrColor += bloomColor; //additive blending
	//tone mapping
	//vec3 reinhard = hdrColor/(hdrColor + vec3(1.0)); //2.0 looks better worst case scenario

	vec3 result = vec3(1.0) - exp(-hdrColor * 0.5f);
	result = pow(result, vec3(1.0 / gamma));
	
	FragColor = vec4(result, 1.0);
}