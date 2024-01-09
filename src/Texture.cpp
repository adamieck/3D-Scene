#include "Texture.h"
#include "stb/stb_image.h"
#include <glad/glad.h>

Texture::Texture(const std::string& path, unsigned slot)
	: _filepath(path), _buffer(nullptr),
	_width(0), _height(0), _bitsPerPic(0), _slot(slot)
{
	LoadTexture();
}

void Texture::LoadTexture()
{
	stbi_set_flip_vertically_on_load(1);
	_buffer = stbi_load(_filepath.c_str(), &_width, &_height,
		&_bitsPerPic, 4);


	glGenTextures(1, &_rendererID);
	glActiveTexture(GL_TEXTURE0 + _slot);
	glBindTexture(GL_TEXTURE_2D, _rendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (_buffer)
		stbi_image_free(_buffer);
}

void Texture::ChangeTexture(const std::string& newFilePath)
{
	_filepath = newFilePath;
	_buffer = nullptr;
	_width = 0;
	_height = 0;
	_bitsPerPic = 0;
	LoadTexture();
}

Texture::~Texture()
{
	glDeleteTextures(1, &_rendererID);
}

void Texture::Bind(unsigned slot) const
{
	glBindTexture(GL_TEXTURE_2D, _rendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);

}
