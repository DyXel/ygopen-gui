#include "renderer.hpp"
#include "primitive.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

Renderer::Renderer(SDL_Window* sdlWindow) : GLShared::Renderer(sdlWindow)
{}

Drawing::Primitive Renderer::NewPrimitive()
{
	return std::make_shared<Primitive>(primProg, texPrimProg);
}

}  // namespace GLES

}  // namespace Detail

}  // namespace Drawing
