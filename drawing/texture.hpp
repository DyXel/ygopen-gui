#ifndef DRAWING_TEXTURE_HPP
#define DRAWING_TEXTURE_HPP

namespace Drawing
{

namespace Detail
{

class ITexture
{
public:
	virtual void SetImage(int w, int h, void* data) = 0;
};

} // Detail

} // Drawing
#endif // DRAWING_TEXTURE_HPP
