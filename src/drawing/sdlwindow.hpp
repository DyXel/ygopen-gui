#ifndef DRAWING_SDLWINDOW_HPP
#define DRAWING_SDLWINDOW_HPP
#include <memory>
#include <vector>
#include <SDL.h>

#include "types.hpp"

namespace Drawing
{

// Available Backends
enum Backend
{
	NOT_LOADED = 0,  // Used to check if any backend has been loaded yet
	OPENGL_CORE, // OpenGL 4.1+
	OPENGL_ES    // OpenGL ES 2.0+
};

class SDLWindow
{
public:
	SDLWindow(const Backend backend);
	~SDLWindow();
	Renderer renderer;
protected:
	SDL_Window* window;
private:
	SDL_GLContext glCtx;
	
	bool GLCreateContext();
	bool LoadGLCore();
	bool LoadGLES();
};

} // Drawing

#endif // DRAWING_SDLWINDOW_HPP
