#version 410 core

vec3 getDirectionalLight(vec3 norm, vec3 viewDir);

out vec4 FragColor;

in vec3 normal;
in vec3 posWS;

struct pointLight
{
    vec3 position;
    vec3 color;
    float Kc; // constant
    float Kl; // linear
    float Ke; //exponential 
};

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light);

#define numPointLights 3
uniform pointLight pLight[numPointLights];
uniform vec3 lightCol;
uniform vec3 lightDir;
uniform vec3 objectCol;
uniform vec3 viewPos;

float ambientFactor = 0.5f;
float shine = 128;
float specularStrength = 0.4f;

void main()
{    	
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - posWS);
    vec3 result = vec3(0.0f);

    for(int i = 0; i < numPointLights; i++)
    {
        result = result + getPointLight(norm, viewDir, pLight[i]);
    }

    FragColor = vec4(result, 1.0);
}

vec3 getDirectionalLight(vec3 norm, vec3 viewDir)
{
    //ambient light
    vec3 ambientColor = lightCol*objectCol*ambientFactor;

    //diffuse light
    float diffuseFactor = dot(norm, -lightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = lightCol*objectCol*diffuseFactor;

    //specular light 
    vec3 halfwayDir = normalize(viewDir - lightDir);
    float specularFactor = dot(norm, halfwayDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = lightCol * specularFactor * specularStrength;

    vec3 result = ambientColor + diffuseColor + specularColor;
    return result;
}

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light)
{
    //point light
    float dist = length(light.position - posWS);
    float attn = 1.0/(light.Kc + (light.Kl*dist) + (light.Ke*(dist*dist)));
    vec3 pLightDir = normalize(light.position - posWS);
    //ambient
    vec3 ambientColor = lightCol*objectCol*ambientFactor;
    ambientColor = ambientColor * attn; //was optional
    //diffuse
    float diffuseFactor = dot(norm, pLightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = lightCol*objectCol*diffuseFactor;
    diffuseColor = diffuseColor*attn;
    //specular
    vec3 reflectDir = reflect(pLightDir, norm);//maybe change to blinn-phong later?
    float specularFactor = dot(viewDir, reflectDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = lightCol * specularFactor * specularStrength;
    specularColor = specularColor*attn;
    vec3 pointLightResult = ambientColor + diffuseColor + specularColor;

    return pointLightResult;
}

