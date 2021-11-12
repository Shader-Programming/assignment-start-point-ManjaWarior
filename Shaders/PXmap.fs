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

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light, vec2 texCoords);
vec3 getDirectionalLight(vec3 norm, vec3 viewDir, vec2 texCoords);
vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light, vec2 texCoords);
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

vec3 getDirectionalLight(vec3 normal, vec3 lightDir, vec3 viewDir, vec2 texCoords)
{
    vec3 ambient = texture(mat.diffuseTexture, texCoords).rgb*mat.ambient*light.ambient;
    float dif = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dif * (texture(mat.diffuseTexture, texCoords)).rgb*light.diffuse;
    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow(max(dor(normal, halfWay), 0.0), mat.shininess);
    vec3 specular = light.specular * spec * vec3(texture(mat.specularTexture, texCoords).r);
    return ambient + diffuse + specular;
}

vec3 getPointLight(vec3 normal, vec3 viewDir, pointLight pLight, vec2 texCoords)
{
    vec3 lightDir = normalize(pLight.pos - posWS);
    //diffuse shading
    float diff = max(dor(normal, lightDir), 0.0);
    //specular shading
    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow(max(dor(normal, halfWay), 0.0), mat.shininess);
    //attenutation
    float distance = length(-lightDir, normal);
    float attenuation = 1.0 / (pLight.kC + pLight.kL * distance + pLight.kQ * (distance*distance));
    //combine results
    vec3 ambient = pLight.ambient * vec3(texture(mat.diffuseTexture, texCoords));
    vec3 diffuse = pLight.diffuse * diff * vec3(texture(mat.diffuseTexture, texCoords));
    vec3 specular = pLight.specular * spec * vec3(texture(mat.specularTexture, texCoords).r);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}