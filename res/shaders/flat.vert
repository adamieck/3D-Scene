#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
flat out vec4 FragColor;

struct Light {
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform sampler2D texture_diffuse1;
uniform float fogIntensity;
uniform vec3 fogColor;


uniform Light light;

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
	vec3 FragPos = vec3(view * model * vec4(aPos, 1.0));
	vec3 Normal = mat3(transpose(inverse(view * model))) * aNormal;
	gl_Position = proj * view * model * vec4(aPos, 1.0);


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
    
    //vec2 texCoords = aTexCoords;
    vec4 textureColor = texture(texture_diffuse1, vec2(aTexCoords));
    vec3 result = (ambient + diffuse + specular) * textureColor.xyz;

    float fog_factor = CalcFogFactor(FragPos);
    result = mix(fogColor, result, fog_factor);
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 1.0);
}