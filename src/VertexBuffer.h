#pragma once


class VertexBuffer
{
private:
	unsigned int _rendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Update(const void* data, unsigned size) const;
	void Unbind() const;
};