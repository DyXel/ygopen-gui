#include "texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace Dummy
{

Texture::Texture()
{}

Texture::~Texture()
{}

void Texture::SetImage([[maybe_unused]] int w, [[maybe_unused]] int h,
                       [[maybe_unused]] void* data)
{}

} // Dummy

} // Detail

} // Drawing
