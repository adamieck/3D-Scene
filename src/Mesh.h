#pragma once
#include <string>
#include <vector>

#include "Shader.h"
#include "VBLayout.h"
#include "glm/glm.hpp"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 texCoords;
	glm::vec3 normal;	
};

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures);
	void Draw(Shader& shader);

private:
	// render
	unsigned int VAO, VBO, EBO;
	VBLayout layout;

	void setupMesh();
};
