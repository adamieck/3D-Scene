#version 410 core

out vec4 fragColor;

in vec3 worldNormal;
in vec3 fragPos;
in vec2 texCoords;

struct Light {
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Light light;
uniform vec3 fogColor;
uniform float fogIntensity;

float CalcFogFactor(vec3 fragPos)
{
    if (fogIntensity == 0) return 1;
    float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60);
    float distance = length(viewPos - fragPos);
    float fog = exp(-pow((distance / gradient), 4));
    fog = clamp(fog, 0.0, 1.0);
    return fog;
}

void main()
{
    // ambient
    vec3 ambient = light.ambient;
      
    // diffuse 
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse  * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos -fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 140.0f);
    vec3 specular = light.specular * spec;

    // Attenuation
    //float distance = length(light.position - fragPos);
    //float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);

      
    vec3 result = ambient + diffuse + specular; 
    //float fogFactor = CalcFogFactor(fragPos);
    //result = mix(fogColor, result, fogFactor);

    //result = clamp(result, 0.0, 1.0);
    vec4 textureColor = texture(texture_diffuse1, texCoords);
    
    fragColor = vec4(result, 1.0) * textureColor;

}