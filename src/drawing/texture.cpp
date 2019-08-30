#include "texture.hpp"

namespace Drawing
{

namespace Detail
{

int ITexture::GetWidth() const
{
	return w;
}

int ITexture::GetHeight() const
{
	return h;
}

} // Detail

} // Drawing
