#ifndef DRAWING_GL_SHARED_RENDERER_HPP
#define DRAWING_GL_SHARED_RENDERER_HPP
#include "../renderer.hpp"

#include "program.hpp"
#include "shader.hpp"

#include "../types.hpp"

struct SDL_Window;

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

class Renderer : public IRenderer
{
public:
	Renderer(SDL_Window* sdlWindow);
	
	void Clear() override;
	void Present() override;
	void SetViewport(int x, int y, int width, int height) override;
	
	Drawing::Texture NewTexture() override;
	
	bool SetVSync(VSyncState state) override;
protected:
	Program primProg;
	Program texPrimProg;
private:
	SDL_Window* sdlWindow{nullptr};
};

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_RENDERER_HPP
