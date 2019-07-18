#ifndef DRAWING_TEXTURE_HPP
#define DRAWING_TEXTURE_HPP
#include <cstddef>

namespace Drawing
{

namespace Detail
{

class ITexture
{
public:
	virtual void SetImage(std::size_t w, std::size_t h, void* data) = 0;
};

} // Detail

} // Drawing
#endif // DRAWING_TEXTURE_HPP
