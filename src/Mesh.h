#pragma once
#include <string>
#include <vector>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VBLayout.h"
#include "VertexArray.h"
#include "glm/glm.hpp"


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;	
	glm::vec2 texCoords;
	glm::vec3 colors;
};

struct Tex
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	unsigned int VAO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Tex> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Tex> textures);
	void Draw(Shader& shader);

private:
	// render
	unsigned int VBO, EBO;
	//VertexArray VAO;
	//VertexBuffer VBO;
	//IndexBuffer Ib;
	//VBLayout layout;

	void setupMesh();
};
