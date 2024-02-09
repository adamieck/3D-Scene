#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 0) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 worldNormal;
out vec3 fragPos;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	texCoords = aTexCoords;
	fragPos = vec3(model * vec4(aPos, 1.0));
	worldNormal = mat3(transpose(inverse(model))) * aNormal; 
	gl_Position = proj * view * model * vec4(aPos, 1.0);
}