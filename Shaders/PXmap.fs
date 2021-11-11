#version 410 core

struct material
{
    vec3 ambient;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    sampler2D normalMap;
    smapler2D dispMap;
    float shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{   
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float kC;
    float kL;
    float kQ;
};

struct spotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float Kc; // constant
    float Kl; 
    float Ke; 

    float innerRad;//inner and outer radius
    float outerRad;
};

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light);
vec3 getDirectionalLight(vec3 norm, vec3 viewDir, vec2 texCoords);
vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir);

out vec4 FragColor;

#define numPointLights 3
uniform pointLight pLight[numPointLights];

#define numSpotLights 2
uniform spotLight sLight[numSpotLights];

uniform int isNM;
uniform float PXscale;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform Material mat;
uniform Light light;

in vec3 normal; 
in vec3 posWS;
in vec2 uv;
in mat3 TBN;


void main()
{
    vec3 norm = normalize(normal);
    vec2 texCoords = uv;
    vec3 result = vec3(0.0);
    vec3 lightDir = normalize(-lightPos);
    vec3 viewDir = normalize(eyePos - posWS);

    if(isNM == 1)
    {
        //PX
        texCoords = ParallaxMapping(uv, viewDir);
        //texCoords = SteepParallaxMapping(uv, viewDir);
    }
    norm = texture(mat.normalMap, texCoords).rgb;
    norm = norm*2.0 - 1.0;
    norm = normalize(TBN*norm;)

    result = result + getDirectionalLight(norm, viewDir, texCoords);
    FragColor = vec4(result, 1.0f);
}