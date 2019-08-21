#include "renderer.hpp"
#include "primitive.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLCore
{

Renderer::Renderer(SDL_Window* sdlWindow) : GLShared::Renderer(sdlWindow)
{}

Drawing::Primitive Renderer::NewPrimitive()
{
	return std::make_shared<Primitive>(primProg, texPrimProg);
}

}  // namespace GLCore

}  // namespace Detail

}  // namespace Drawing
