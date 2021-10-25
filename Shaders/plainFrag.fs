#version 410 core

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
vec3 getSpotLight(vec3 norm, vec3 viewDir);

#define numPointLights 3
uniform pointLight pLight[numPointLights];

uniform spotLight sLight;

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

    vec3 spotLight = getSpotLight(norm, viewDir);
    result = result + spotLight;

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
    vec3 diffuseColor = light.color*objectCol*diffuseFactor;
    diffuseColor = diffuseColor*attn;
    //specular
    vec3 reflectDir = reflect(pLightDir, norm);//maybe change to blinn-phong later?
    float specularFactor = dot(viewDir, reflectDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = light.color * specularFactor * specularStrength;
    specularColor = specularColor*attn;
    vec3 pointLightResult = ambientColor + diffuseColor + specularColor;

    return pointLightResult;
}

vec3 getSpotLight(vec3 norm, vec3 viewDir)
{

    //spot light
    float dist = length(sLight.position - posWS);
    float attn = 1.0/(sLight.Kc + (sLight.Kl*dist) + (sLight.Ke*(dist*dist)));
    vec3 sLightDir = normalize(sLight.position - posWS);
    //diffuse
    float diffuseFactor = dot(norm, sLightDir);
    diffuseFactor = max(diffuseFactor,0.0);
    vec3 diffuseColor = sLight.color*objectCol*diffuseFactor;
    diffuseColor = diffuseColor*attn;
    //specular
    vec3 reflectDir = reflect(sLightDir, norm);//maybe change to blinn-phong later?
    float specularFactor = dot(viewDir, reflectDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = sLight.color * specularFactor * specularStrength;
    specularColor = specularColor*attn;

    float theta = dot(-sLightDir, normalize(sLight.direction));
    float denom = (sLight.innerRad - sLight.outerRad);
    float illum = (theta - sLight.outerRad) / denom;
    illum = clamp(illum, 0.0,1.0);
    diffuseColor = diffuseColor * illum;
    specularColor = specularColor * illum;

    vec3 spotLightResult = diffuseColor + specularColor;
    return spotLightResult;
}
