#version 410 core

out vec4 FragColor;

in vec3 normal;
in vec3 posWS;
in vec2 uv;
in mat3 TBN;

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

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light, vec2 texCoords);
vec3 getDirectionalLight(vec3 norm, vec3 viewDir, vec2 texCoords);
vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light, vec2 texCoords);
vec2 ParallaxMapping(vec2 uv, vec3 viewDir);
vec2 SteepParallaxMapping(vec2 uv, vec3 viewDir);


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
uniform sampler2D normalMap;
uniform sampler2D dispMap;

uniform int map;
uniform bool DL;
uniform bool PL;
uniform bool SL;
uniform bool NM;

uniform float PXscale;

float ambientFactor = 0.5f;
float shine = 128;

void main()
{    	
    vec3 viewDir = normalize(viewPos - posWS);
    vec2 texCoords = uv;
    vec3 result = vec3(0.0f);
    vec3 norm = vec3(0.0);

    if(NM == true)
    {
        //texCoords = ParallaxMapping(uv, -viewDir); //working?
        texCoords = SteepParallaxMapping(uv, -viewDir);//working? but shouldn't need a texture map?
    }

    if(map == 1)
    {
        norm = texture(normalMap, texCoords).rgb;
        norm = normalize(TBN*(norm*2.0 - 1.0));
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

    if(SL == true)
    {
        for(int i = 0; i < numSpotLights; i++)
        {
            result = result + getSpotLight(norm, viewDir, sLight[i], texCoords);
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 getDirectionalLight(vec3 norm, vec3 viewDir, vec2 texCoords)
{
    vec3 diffMapColor = texture(diffuseTexture, texCoords).rgb;
    float specMapColor = texture(specularTexture, texCoords).r;
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
    vec3 specularColor = lightCol * specularFactor * specMapColor * 0.5f ;

    vec3 result = ambientColor + diffuseColor + specularColor;
    return result;
}

vec3 getPointLight(vec3 norm, vec3 viewDir, pointLight light, vec2 texCoords)
{
    vec3 diffMapColor = texture(diffuseTexture, texCoords).xyz;//should these be .rgb and .r 
    float specMapColor = texture(specularTexture, texCoords).x;

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
    vec3 halfwayDir = normalize(viewDir - lightDir);
    float specularFactor = dot(viewDir, halfwayDir);
    specularFactor = max(specularFactor, 0.0);
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = light.color * specularFactor * specMapColor * 0.5f ;
    specularColor = specularColor * attn;
    vec3 pointLightResult = (ambientColor + diffuseColor + specularColor) * 0.4f;

    return pointLightResult;
}

vec3 getSpotLight(vec3 norm, vec3 viewDir, spotLight light, vec2 texCoords)
{

    vec3 diffMapColor = texture(diffuseTexture, texCoords).xyz;
    float specMapColor = texture(specularTexture, texCoords).x;

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
    specularFactor = pow(specularFactor, shine);
    vec3 specularColor = light.color * specularFactor * specMapColor * 0.5f ;
    specularColor = specularColor * attn;

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
    float height = texture(dispMap, texCoords).r;
    return texCoords - (viewDir.xy) * (height * PXscale);
}

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 10.f;
    const float maxLayers = 35.f;
    //float numLayers = 25;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));//adjusts the number of layers on the view direction
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * PXscale;
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(dispMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(dispMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    //parallax occlusion mapping below here
    //get tex coords before collision
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    //get depth after and before collision for interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(dispMap, prevTexCoords).r - currentLayerDepth + layerDepth;
    //interpolation of texture coords
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords *weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
    //return currentTexCoords;
}