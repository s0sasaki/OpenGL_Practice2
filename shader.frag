#version 330 core

struct PointLight{
    vec3 color;
    vec3 position;
};

struct SpotLight{
    vec3 color;
    vec3 position;
    vec3 direction;
    float spotCutoff;
    float spotExponent;
};


// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 Normal;
in vec3 FragPos;

uniform mat4 modelview;
uniform vec3 cameraPos;
uniform vec3 ambientLightColor;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform vec3 directionalLightPos;
uniform vec3 directionalLightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform int flagNormColor;
uniform int toggleOnPoint;
uniform int toggleOnSpot;
uniform int toggleOnDir;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;


vec3 directionalLighting();
vec3 pointLighting();
vec3 spotLighting();

void main()
{
    vec3 ambient = ambientStrength * ambientLightColor;
    
    vec3 result = ambient;
    if(toggleOnPoint == 1) result += pointLighting()*3000;
    if(toggleOnSpot  == 1) result += spotLighting()*10;
    if(toggleOnDir   == 1) result += directionalLighting();
    if(flagNormColor == 0){
        result *= objectColor;
    }else{
        vec3 norm = normalize(Normal);
        result *= (norm+1.0f)/2.0f;
    }
    
    color = vec4(result, 1.0f);
}

vec3 directionalLighting(){
    vec3 norm = normalize(Normal);
    
    vec3 lightDir = normalize(directionalLightPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * directionalLightColor;
    
    vec3 viewPos = cameraPos;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * directionalLightColor;
    
    return (diffuse + specular);
}

vec3 pointLighting(){
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    
    float diff = max(dot(norm, lightDir), 0.0f);

    vec3 viewPos = cameraPos;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    
    float d = length(pointLight.position - FragPos);
    float attenuation = 1.0f / (1.0f + 0.001f * d + 1.0f * d * d);
    
    vec3 diffuse = diff * diffuseStrength * pointLight.color * attenuation;
    vec3 specular = spec * specularStrength * pointLight.color * attenuation;
    
    return (diffuse + specular);
}

vec3 spotLighting(){
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);
    vec3 spotDir = normalize(spotLight.direction);
    float theta = dot(lightDir, -spotDir);
    float intensity = clamp(spotLight.spotExponent * (theta - spotLight.spotCutoff)/(1.0f - spotLight.spotCutoff), 0.0f, 1.0f);
    if(theta > spotLight.spotCutoff){
        float diff = max(dot(norm, lightDir), 0.0f);
        
        vec3 viewPos = cameraPos;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
        
        float d = length(spotLight.position - FragPos);
        float attenuation = 1.0f / (1.0f + 0.001f * d + 0.001f * d * d);
        
        vec3 diffuse = diff * diffuseStrength * spotLight.color * attenuation * intensity;
        vec3 specular = spec * specularStrength * spotLight.color * attenuation * intensity;
        
        return (diffuse + specular);
    }else{
        return vec3(0.0f, 0.0f, 0.0f);
    }
    
}
