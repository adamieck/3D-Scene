#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec3 worldNormal;
out vec3 worldPos;
out vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	texCoords = aTexCoords;
	worldPos = vec3(modelMatrix * vec4(aPos, 1.0));
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}