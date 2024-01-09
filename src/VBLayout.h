#pragma once

#include <vector>
#include <glad/glad.h>

#include <stdexcept>

struct VBElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int SizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		return 0;
	}
};

class VBLayout
{
private:
	std::vector<VBElement> _elements;
	unsigned int _stride;
public:
	VBLayout() : _stride(0) {}

	void Push(unsigned int type, unsigned int count)
	{
		_elements.push_back({ type, count, GL_FALSE });
		_stride += count * VBElement::SizeOfType(type);
	}

	inline const std::vector<VBElement> GetElements() const
	{
		return _elements;
	}
	inline unsigned int GetStride() const { return _stride; }
};