#version 410 core

out vec4 FragColor;

in vec3 normal;
in vec3 posWS;
in vec2 uv;

struct pointLight
{
    vec3 position;
    vec3 color;
    float Kc; // constant
    float Kl; // linear
    float Ke; //exponential 
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
vec3 getDirectionalLight(vec3 norm, vec3 viewDir);
vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light);

#define numPointLights 3
uniform pointLight pLight[numPointLights];

#define numSpotLights 2
uniform spotLight sLight[numSpotLights];

uniform vec3 lightCol;
uniform vec3 lightDir;
uniform vec3 objectCol;
uniform vec3 viewPos;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;


float ambientFactor = 0.5f;
float shine = 128;
float specularStrength = 0.4f;

void main()
{    	
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - posWS);
    vec3 result = vec3(0.0f);

    result = result + getDirectionalLight(norm, viewDir);

    for(int i = 0; i < numPointLights; i++)
    {
        result = result + getPointLight(norm, viewDir, pLight[i]);
    }

    for(int i = 0; i < numSpotLights; i++)
    {
        result = result + getSpotLight(norm, viewDir, sLight[i]);
    }

    float rimLight = 0.075*(1.0-dot(norm, viewDir));//apply to cubes but not floor, big Flan said so
    rimLight = pow(rimLight, 0.5);
    result = result + rimLight;

    FragColor = vec4(result, 1.0);
}

vec3 getDirectionalLight(vec3 norm, vec3 viewDir)
{
    vec3 diffMapColor = texture(diffuseTexture, uv).xyz;
    float specMapColor = texture(specularTexture, uv).x;
    //ambient light
    vec3 ambientColor = lightCol*diffMapColor*ambientFactor;

    //diffuse light
    float diffuseFactor = dot(norm, -lightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = lightCol * diffMapColor * diffuseFactor;

    //specular light 
    vec3 halfwayDir = normalize(viewDir - lightDir);
    float specularFactor = dot(norm, halfwayDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = lightCol * specularFactor * /*specularStrength*/ specMapColor ;

    vec3 result = ambientColor + diffuseColor + specularColor;
    return result;
}

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light)
{
    vec3 diffMapColor = texture(diffuseTexture, uv).xyz;
    float specMapColor = texture(specularTexture, uv).x;

    //point light
    float dist = length(light.position - posWS);
    float attn = 1.0/(light.Kc + (light.Kl*dist) + (light.Ke*(dist*dist)));
    vec3 pLightDir = normalize(light.position - posWS);
    //ambient
    vec3 ambientColor = lightCol*diffMapColor*ambientFactor;
    ambientColor = ambientColor * attn; //was optional
    //diffuse
    float diffuseFactor = dot(norm, pLightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = light.color*diffMapColor*diffuseFactor;
    diffuseColor = diffuseColor*attn;
    //specular
    vec3 reflectDir = reflect(pLightDir, norm);//maybe change to blinn-phong later?
    float specularFactor = dot(viewDir, reflectDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = light.color * specularFactor * specMapColor;
    specularColor = specularColor * attn;
    vec3 pointLightResult = ambientColor + diffuseColor + specularColor;

    return pointLightResult;
}

vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light)
{

    vec3 diffMapColor = texture(diffuseTexture, uv).xyz;
    float specMapColor = texture(specularTexture, uv).x;

    //spot light
    float dist = length(light.position - posWS);
    float attn = 1.0/(light.Kc + (light.Kl*dist) + (light.Ke*(dist*dist)));
    vec3 sLightDir = normalize(light.position - posWS);
    //diffuse
    float diffuseFactor = dot(norm, sLightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = light.color*diffMapColor*diffuseFactor;
    diffuseColor = diffuseColor * attn;
    //specular
    vec3 reflectDir = reflect(sLightDir, norm);//maybe change to blinn-phong later?
    float specularFactor = dot(viewDir, reflectDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = light.color * specularFactor * specMapColor;
    specularColor = specularColor;

    float theta = dot(-sLightDir, normalize(light.direction));
    float denom = (light.innerRad - light.outerRad);
    float illum = (theta - light.outerRad) / denom;
    illum = clamp(illum, 0.0,1.0);
    diffuseColor = diffuseColor * illum;
    specularColor = specularColor * illum;

    vec3 spotLightResult = diffuseColor + specularColor;
    return spotLightResult;
}
