#include "texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

Texture::Texture()
{
	glGenTextures(1, &ref);
	glBindTexture(GL_TEXTURE_2D, ref);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
	glDeleteTextures(1, &ref);
}

void Texture::SetImage(int width, int height, void* data)
{
	static const GLint MIPS_COUNT = 0;
	glTexImage2D(GL_TEXTURE_2D, MIPS_COUNT, GL_RGBA, width, height,
	             0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	w = width;
	h = height;
}

void Texture::Bind()
{
// 	glActiveTexture(GL_TEXTURE0); // Value starts at that.
	glBindTexture(GL_TEXTURE_2D, ref);
}

}  // namespace GLShared

}  // namespace Detail

}  // namespace Drawing
