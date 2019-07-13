#include <SDL.h>
#include <SDL_assert.h>
#include "api.hpp"
#include "primitive.hpp"
#include "gl_include.hpp"
#include "gl_es/primitive.hpp"
#include "gl_es/shader_and_program.hpp"

namespace Drawing
{

namespace API
{

static Backend preloadedBackend = NOT_LOADED;
static Backend activeBackend = NOT_LOADED;
static SDL_Window* window = nullptr;
static SDL_GLContext glCtx = nullptr;

// GLES stuff
static const GLchar* VERTEX_SHADER_SRC =
"#version 100\n"
"attribute vec3 pos;\n"
"attribute vec4 color;\n"
"varying vec4 fsColor;\n"
"void main() {\n"
"   gl_Position = vec4(pos, 1.0);\n"
"   fsColor = color;\n"
"}\n";
static const GLchar* FRAGMENT_SHADER_SRC =
"#version 100\n"
"precision mediump float;\n"
"varying vec4 fsColor;\n"
"void main() {\n"
"   gl_FragColor = fsColor;\n"
"}\n";

static std::shared_ptr<Detail::Program> glesProgram;

void LogGLString(const char* nameStr, const GLenum name)
{
	const GLubyte* ret = glGetString(name);
	if (ret == 0)
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

bool PreloadBackend(Backend backend)
{
	switch(backend)
	{
		case OPENGL_CORE:
		{
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			// 	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			                    SDL_GL_CONTEXT_PROFILE_CORE);
			break;
		}
		case OPENGL_ES:
		{
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			// 	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			                    SDL_GL_CONTEXT_PROFILE_ES);
			break;
		}
		default:
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			             "Undefined backend selected (%i)", backend);
			return false;
		}
	}
	preloadedBackend = backend;
	return true;
}

bool LoadBackend(SDL_Window* w, Backend backend)
{
	SDL_assert(w);
	if(preloadedBackend != backend)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Preload backend mismatch (preloaded: %i, current: %i)",
		             preloadedBackend, backend);
		return false;
	}
	if(activeBackend != NOT_LOADED)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "There was a backend already active (%i)", activeBackend);
		return false;
	}
	switch(backend)
	{
		case OPENGL_CORE:
		{
			glCtx = SDL_GL_CreateContext(w);
			if(glCtx == NULL)
			{
				glCtx = nullptr;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				             "Unable to create OpenGL context: %s",
				             SDL_GetError());
				return false;
			}
#ifndef USE_PROTOTYPES_GL
#define SDL_PROC(ret,f,params) \
			do \
			{ \
				f = reinterpret_cast<decltype(f)>(SDL_GL_GetProcAddress(#f)); \
				if (!f) \
				{ \
					SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, \
					                "Couldn't load GL function %s: %s", \
					                #f, SDL_GetError()); \
					return false; \
				} \
			}while(0);
#include "gl_es2_funcs.h"
#include "gl_core_funcs.h"
#undef SDL_PROC
#endif
			LOG_GL_STRING(GL_VENDOR);
			LOG_GL_STRING(GL_RENDERER);
			LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
			LOG_GL_STRING(GL_VERSION);
			// 	LOG_GL_STRING(GL_EXTENSIONS); // Too spammy
			break;
		}
		case OPENGL_ES:
		{
			glCtx = SDL_GL_CreateContext(w);
			if(glCtx == NULL)
			{
				glCtx = nullptr;
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				             "Unable to create OpenGL context: %s",
				 SDL_GetError());
				return false;
			}
#ifndef USE_PROTOTYPES_GL
#define SDL_PROC(ret,f,params) \
			do \
			{ \
				f = reinterpret_cast<decltype(f)>(SDL_GL_GetProcAddress(#f)); \
				if (!f) \
				{ \
					SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, \
					                "Couldn't load GL function %s: %s", \
					                #f, SDL_GetError()); \
					return false; \
				} \
			}while(0);
#include "gl_es2_funcs.h"
#undef SDL_PROC
#endif
			LOG_GL_STRING(GL_VENDOR);
			LOG_GL_STRING(GL_RENDERER);
			LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
			LOG_GL_STRING(GL_VERSION);
			// 	LOG_GL_STRING(GL_EXTENSIONS); // Too spammy
			glesProgram = std::make_shared<Detail::Program>();
			{
				Detail::Shader vs(GL_VERTEX_SHADER, VERTEX_SHADER_SRC);
				Detail::Shader fs(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SRC);
				glesProgram->Attach(vs).Attach(fs).Link();
			}
			break;
		}
		default:
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			             "Undefined backend selected (%i)", backend);
			return false;
		}
	}
	activeBackend = backend;
	window = w;
	return true;
}

void UnloadBackend()
{
	switch(activeBackend)
	{
		case OPENGL_CORE:
		{
			SDL_GL_DeleteContext(glCtx);
			break;
		}
		case OPENGL_ES:
		{
			glesProgram.reset();
			SDL_GL_DeleteContext(glCtx);
			break;
		}
		case NOT_LOADED:
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
			            "There was no active backend");
			return;
		}
	}
	activeBackend = NOT_LOADED;
}

void Clear()
{
	SDL_assert(activeBackend);
	switch(activeBackend)
	{
		case OPENGL_CORE:
		case OPENGL_ES:
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		}
		case NOT_LOADED: break;
	}
}

void Present()
{
	SDL_assert(activeBackend);
	switch(activeBackend)
	{
		case OPENGL_CORE:
		case OPENGL_ES:
		{
			SDL_GL_SwapWindow(window);
			break;
		}
		case NOT_LOADED: break;
	}
}

void UpdateDrawableSize(int* w, int* h)
{
	SDL_assert(activeBackend);
	switch(activeBackend)
	{
		case OPENGL_CORE:
		case OPENGL_ES:
		{
			SDL_GL_GetDrawableSize(window, w, h);
			glViewport(0, 0, *w, *h);
			break;
		}
		case NOT_LOADED: break;
	}
}

Primitive NewPrimitive()
{
	SDL_assert(activeBackend);
	/*if(activeBackend == OPENGL_CORE)
		return std::make_shared<Detail::GLCorePrimitive>();
	else */if(activeBackend == OPENGL_ES)
		return std::make_shared<Detail::GLESPrimitive>(*glesProgram);
}

} // API

} // Drawing
