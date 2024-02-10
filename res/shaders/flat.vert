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
struct Pointlight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform sampler2D texture_diffuse1;
uniform float fogIntensity;
uniform vec3 fogColor;


uniform Light light;
uniform Pointlight carpetLight;


float CalcFogFactor(vec3 fragPos)
{
    if (fogIntensity == 0) return 1;
    float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60);
    float distance = length(-fragPos);
    float fog = exp(-pow((distance / gradient), 4));
    fog = clamp(fog, 0.0, 1.0);
    return fog;
}
vec3 CalcPointLight(Pointlight spotlight, vec3 FragPos, vec3 Normal)
{
    vec3 LightPos = vec3(view * vec4(spotlight.position, 1.0));
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * spotlight.ambient;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * spotlight.diffuse;
    
    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(-FragPos); // (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * spotlight.specular; 
    float distance    = length(LightPos - FragPos);
    float attenuation = 1.0 / (spotlight.constant + spotlight.linear * distance + 
    		    spotlight.quadratic * (distance * distance));   

    ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   

    return (ambient + diffuse + specular);
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
    vec3 result = vec3(0.0, 0.0, 0.0);
    result += (ambient + diffuse + specular);
    result += CalcPointLight(carpetLight, FragPos, Normal);
    result *= textureColor.xyz;

    float fog_factor = CalcFogFactor(FragPos);
    result = mix(fogColor, result, fog_factor);
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 1.0);
}