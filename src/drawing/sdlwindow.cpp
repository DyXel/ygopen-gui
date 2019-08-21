#include "sdlwindow.hpp"

#include <string>

#include "gl_include.hpp"
#include "renderer.hpp"

#include "gl_core/renderer.hpp"
#include "gl_es/renderer.hpp"

namespace Drawing
{

inline void LogGLString(const char* nameStr, const GLenum name)
{
	const GLubyte* ret = glGetString(name);
	if (ret == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Unable to get OpenGL string: %s (0x%X)", nameStr, name);
	}
	else
	{
		SDL_Log("OpenGL: %s (0x%X): %s", nameStr, name, ret);
	}
}
#define LOG_GL_STRING(n) LogGLString(#n, n)

inline void GLLogStrings()
{
	LOG_GL_STRING(GL_VENDOR);
	LOG_GL_STRING(GL_RENDERER);
	LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
	LOG_GL_STRING(GL_VERSION);
}

SDLWindow::SDLWindow(const Backend backend)
{
	if(backend == NOT_LOADED)
		throw std::runtime_error("Undefined backend selected");
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	if(backend == OPENGL_CORE)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		                    SDL_GL_CONTEXT_PROFILE_CORE);
	}
	else if(backend == OPENGL_ES)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		                    SDL_GL_CONTEXT_PROFILE_ES);
	}
	
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, 69, 69,
	                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
	                          SDL_WINDOW_HIDDEN);
	
	if(window == nullptr)
	{
		std::string errStr("Unable to create SDL Window: ");
		errStr += SDL_GetError();
		throw std::runtime_error(errStr);
	}
	
	glCtx = SDL_GL_CreateContext(window);
	if(glCtx == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext: %s",
		             SDL_GetError());
		SDL_DestroyWindow(window);
		window = nullptr;
		throw std::runtime_error("Unable to create OpenGL Context");
	}
	
	if(backend == OPENGL_CORE && LoadGLCore())
		renderer = std::make_shared<Detail::GLCore::Renderer>(window);
	else if(backend == OPENGL_ES && LoadGLES())
		renderer = std::make_shared<Detail::GLES::Renderer>(window);
	else
		throw std::runtime_error("Unable to load target API");
}

SDLWindow::~SDLWindow()
{
	renderer.reset();
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
}

#define SDL_PROC(ret,f,params) \
	do \
	{ \
		(f) = reinterpret_cast<decltype(f)>(SDL_GL_GetProcAddress(#f)); \
		if (!(f)) \
		{ \
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, \
			                "Couldn't load GL function %s: %s", \
			                #f, SDL_GetError()); \
			return false; \
		} \
	}while(0);

bool SDLWindow::LoadGLCore()
{
#ifndef USE_PROTOTYPES_GL
#include "gl_core_funcs.h"
#include "gl_es2_funcs.h"
#endif
	GLLogStrings();
	return true;
}

bool SDLWindow::LoadGLES()
{
#ifndef USE_PROTOTYPES_GL
#include "gl_es2_funcs.h"
#endif
	GLLogStrings();
	return true;
}

#undef SDL_PROC

} // namespace Drawing
