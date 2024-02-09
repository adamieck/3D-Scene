#version 410 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D texture_diffuse1;

void main()
{
	fragColor = vec4(texture(texture_diffuse1, texCoords).rgb, 1.0f);

}