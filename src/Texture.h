#pragma once
#include <string>

class Texture
{
private:
	unsigned int _rendererID;
	std::string _filepath;
	unsigned char* _buffer;
	int _width, _height, _bitsPerPic;
	unsigned _slot;
public:
	Texture(const std::string& path, unsigned slot);
	void LoadTexture();
	void ChangeTexture(const std::string& newFilePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return _width; }
	inline int GetHeight() const { return _height; }
	inline unsigned int GetID() const { return _rendererID; }
};
