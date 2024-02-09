#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 texCoords;

struct Light {
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform sampler2D texture_diffuse1;
uniform mat4 view;
uniform float fogIntensity;
uniform vec3 fogColor;

float CalcFogFactor(vec3 fragPos)
{
    if (fogIntensity == 0) return 1;
    float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60);
    float distance = length(-fragPos);
    float fog = exp(-pow((distance / gradient), 4));
    fog = clamp(fog, 0.0, 1.0);
    return fog;
}

void main()
{

    vec3 LightPos = vec3(view * vec4(light.position, 1.0));
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * light.ambient;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse;
    
    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(-FragPos); // (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.specular; 
    
    vec4 textureColor = texture(texture_diffuse1, texCoords);
    vec3 result = (ambient + diffuse + specular);

    float fog_factor = CalcFogFactor(FragPos);
    result = mix(fogColor, result, fog_factor);
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 1.0) * textureColor;
}