#ifndef DRAWING_TEXTURE_HPP
#define DRAWING_TEXTURE_HPP

namespace Drawing
{

namespace Detail
{

class ITexture
{
public:
	virtual void SetImage(int width, int height, void* data) = 0;
	
	int GetWidth() const;
	int GetHeight() const;
protected:
	int w, h;
};

} // Detail

} // Drawing
#endif // DRAWING_TEXTURE_HPP
