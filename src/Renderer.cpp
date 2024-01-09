#include "Renderer.h"

void Renderer::Clear()
{
	glClearColor(0.19f, 0.196f, 0.267f, 1.0f); // bg Surface0 catppucin palette
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetPolygonMode(int mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, mode);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	va.Bind();
	ib.Bind();
	shader.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);

}

void Renderer::Draw(const VertexArray& va, const Shader& shader, unsigned int count ) const
{
	va.Bind();
	shader.Bind();

	glDrawArrays(GL_PATCHES, 0, count);

}
