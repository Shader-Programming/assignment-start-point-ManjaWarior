#version 410 core

struct material
{
    float ambient;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    sampler2D normalMap;
    sampler2D dispMap;
    float shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct pointLight
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

uniform float PXscale;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform vec3 eyePos;
uniform material mat;
uniform Light light;

uniform int map;
uniform bool DL;
uniform bool PL;
uniform bool SL;
uniform bool NM;

in vec3 normal; 
in vec3 posWS;
in vec2 uv;
in mat3 TBN;


void main()
{
    vec3 norm = normalize(normal);
    vec2 texCoords = uv;
    vec3 result = vec3(0.0);
    //vec3 lightDir = normalize(-lightPos);
    vec3 viewDir = normalize(eyePos - posWS);

    if(NM == true)
    {
        //PX
        texCoords = ParallaxMapping(uv, viewDir);
        //texCoords = SteepParallaxMapping(uv, viewDir);
    }
    if(map == 1)
    {
        norm = texture(mat.normalMap, uv).xyz;
        norm = norm*2.0 - 1.0;
        norm = normalize(TBN*norm);
    }
    else
    {
        norm = normalize(normal);    
    }

    if(DL == true)
    {
        result = result + getDirectionalLight(norm, viewDir, texCoords);
    }

    if(PL == true)
    {
        for(int i = 0; i < numPointLights; i++)
        {
            result = result + getPointLight(norm, viewDir, pLight[i], texCoords);
        }
    }

    /*if(SL == true)
    {
        for(int i = 0; i < numSpotLights; i++)
        {
            result = result + getSpotLight(norm, viewDir, sLight[i], texCoords);
        }
    }*/


    FragColor = vec4(result, 1.0f);
}

vec3 getDirectionalLight(vec3 normal, vec3 viewDir, vec2 texCoords)
{
    vec3 ambient = texture(mat.diffuseTexture, texCoords).rgb*mat.ambient*light.ambient;
    float dif = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = dif * (texture(mat.diffuseTexture, texCoords)).rgb*light.diffuse;
    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWay), 0.0), mat.shininess);
    vec3 specular = light.specular * spec * vec3(texture(mat.specularTexture, texCoords).r);
    return ambient + diffuse + specular;
}

vec3 getPointLight(vec3 normal, vec3 viewDir, pointLight pLight, vec2 texCoords)
{
    vec3 lightDir = normalize(pLight.pos - posWS);
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading
    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfWay), 0.0), mat.shininess);
    //attenutation
    float distance = length(pLight.pos - posWS);
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

vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light, vec2 texCoords)
{

    vec3 diffMapColor = texture(mat.diffuseTexture, texCoords).xyz;
    float specMapColor = texture(mat.specularTexture, texCoords).x;

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
    vec3 halfwayDir = normalize(viewDir - lightDir);
    float specularFactor = dot(viewDir, halfwayDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, mat.shininess);
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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(mat.dispMap, texCoords).r;
    return texCoords = (viewDir.xy) * (height * PXscale);
}

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float numLayers = 10;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * PXscale;
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(mat.dispMap, currentTexCoords).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(mat.dispMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}