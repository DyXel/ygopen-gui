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

void Texture::SetImage([[maybe_unused]] std::size_t w,
                       [[maybe_unused]] std::size_t h,
                       [[maybe_unused]] void* data)
{}

} // Dummy

} // Detail

} // Drawing
