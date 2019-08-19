#ifndef DRAWING_GL_CORE_RENDERER_HPP
#define DRAWING_GL_CORE_RENDERER_HPP
#include "../gl_shared/renderer.hpp"
#include "../types.hpp"

struct SDL_Window;

namespace Drawing
{

namespace Detail
{

namespace GLCore
{

class Renderer : public GLShared::Renderer
{
public:
	Renderer(SDL_Window* sdlWindow);
	
	Drawing::Primitive NewPrimitive() override;
private:
	SDL_Window* sdlWindow{nullptr};
};

} // GLCore

} // Detail

} // Drawing

#endif // DRAWING_GL_CORE_RENDERER_HPP
