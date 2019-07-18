#ifndef DRAWING_DUMMY_TEXTURE_HPP
#define DRAWING_DUMMY_TEXTURE_HPP
#include "../texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace Dummy
{

class Texture : public ITexture
{
public:
	Texture();
	virtual ~Texture();
	
	void SetImage(std::size_t w, std::size_t h, void* data) override;
};

} // Dummy

} // Detail

} // Drawing

#endif // DRAWING_DUMMY_TEXTURE_HPP
