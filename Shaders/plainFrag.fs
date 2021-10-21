#version 410 core

vec3 getDirectionalLight();
out vec4 FragColor;

in vec3 normal ;
in vec3 posWS;

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
    vec3 result = vec3(0.0);
   

    result = getDirectionalLight(norm, viewDir);
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
//13:36 into the video