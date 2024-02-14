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

struct Pointlight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform Pointlight carpetLight;
uniform sampler2D texture_diffuse1;
uniform mat4 view;
uniform float fogIntensity;
uniform vec3 fogColor;
uniform int levels = 4;
uniform float scaleFactor = 1.0;

float CalcFogFactor(vec3 fragPos)
{
    vec3 LightPos = vec3(view * vec4(light.position, 1.0));
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    if (fogIntensity == 0) return 1;
    float gradient = (fogIntensity * fogIntensity - 50 * fogIntensity + 60);
    float distance = length(-fragPos);
    float fog = exp(-pow((distance / gradient), 4));
    fog = clamp(fog, 0.0, 1.0);
    return fog;
}

vec3 CalcPointLight(Pointlight spotlight)
{
    vec3 LightPos = vec3(view * vec4(spotlight.position, 1.0));
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * spotlight.ambient;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.3 * floor( diff * levels ) * scaleFactor * spotlight.diffuse;

    
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

    vec3 LightPos = vec3(view * vec4(light.position, 1.0));
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * light.ambient;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = (diff * light.diffuse);


    vec3 diffuse = 0.3* floor( diff * levels ) * scaleFactor * light.diffuse;
    
    // specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(-FragPos); // (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.specular; 
    
    vec4 textureColor = texture(texture_diffuse1, texCoords);
    vec3 result = vec3(0.0, 0.0, 0.0);
    result += (diffuse + ambient); //+ specular);
    result += CalcPointLight(carpetLight);
    result *= textureColor.xyz;

    float fog_factor = CalcFogFactor(FragPos);
    result = mix(fogColor, result, fog_factor);
    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, 1.0);
}