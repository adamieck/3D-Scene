#pragma once

#include "VertexBuffer.h"
#include "VBLayout.h"
class VertexArray
{
private:
	unsigned int _rendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VBLayout& layout);
	void Bind() const;
	void Unbind() const;
};