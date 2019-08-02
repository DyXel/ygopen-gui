#include <SDL.h>
#include <SDL_assert.h>
#include "api.hpp"
#include "gl_include.hpp"
#include "dummy/primitive.hpp"
#include "dummy/texture.hpp"
#include "gl_core/primitive.hpp"
#include "gl_es/primitive.hpp"
#include "gl_shared/program.hpp"
#include "gl_shared/shader.hpp"
#include "gl_shared/texture.hpp"

namespace Drawing
{

namespace API
{

static Backend preloadedBackend = NOT_LOADED;
static Backend activeBackend = NOT_LOADED;
static SDL_Window* sdlWindow = nullptr;

// GL stuff
static SDL_GLContext glCtx = nullptr;
static const GLchar* PRIMITIVE_VERTEX_SHADER_SRC =
R"(#version 100

attribute vec3 in_pos;
attribute vec4 in_color;

varying vec4 out_color;

uniform mat4 in_model;

void main()
{
	gl_Position = in_model * vec4(in_pos, 1.0);
	out_color = in_color;
})";
static const GLchar* PRIMITIVE_FRAGMENT_SHADER_SRC =
R"(#version 100
precision mediump float;

varying vec4 out_color; // input from vertex shader

uniform float in_brightness;

void main()
{
	gl_FragColor = out_color *
	               vec4(in_brightness, in_brightness, in_brightness, 1.0);
})";
static const GLchar* TEXTURED_PRIMITIVE_VERTEX_SHADER_SRC =
R"(#version 100

attribute vec3 in_pos;
attribute vec4 in_color;
attribute vec2 in_texCoord;

varying vec4 out_color;
varying vec2 out_texCoord;

uniform mat4 in_model;

void main()
{
	gl_Position = in_model * vec4(in_pos, 1.0);
	out_color = in_color;
	out_texCoord = in_texCoord;
})";
static const GLchar* TEXTURED_PRIMITIVE_FRAGMENT_SHADER_SRC =
R"(#version 100
precision mediump float;

varying vec4 out_color; // input from vertex shader
varying vec2 out_texCoord; // input from vertex shader

uniform sampler2D in_tex;
uniform float in_brightness;

void main()
{
	gl_FragColor = (out_color + texture2D(in_tex, out_texCoord)) *
	               vec4(in_brightness, in_brightness, in_brightness, 1.0);
})";

static std::shared_ptr<Detail::GLShared::Program> glPrimProg;
static std::shared_ptr<Detail::GLShared::Program> glTexPrimProg;

inline void GLLoadPrograms()
{
	glPrimProg = std::make_shared<Detail::GLShared::Program>();
	{
		Detail::GLShared::Shader vs(GL_VERTEX_SHADER,
		                            PRIMITIVE_VERTEX_SHADER_SRC);
		Detail::GLShared::Shader fs(GL_FRAGMENT_SHADER,
		                            PRIMITIVE_FRAGMENT_SHADER_SRC);
		glPrimProg->Attach(vs);
		glPrimProg->Attach(fs);
		glPrimProg->Link();
	}
	
	glTexPrimProg = std::make_shared<Detail::GLShared::Program>();
	{
		Detail::GLShared::Shader vs(GL_VERTEX_SHADER,
		                            TEXTURED_PRIMITIVE_VERTEX_SHADER_SRC);
		Detail::GLShared::Shader fs(GL_FRAGMENT_SHADER,
		                            TEXTURED_PRIMITIVE_FRAGMENT_SHADER_SRC);
		glTexPrimProg->Attach(vs);
		glTexPrimProg->Attach(fs);
		glTexPrimProg->Link();
	}
}

inline void GLEnableStuff()
{
	// Disable face culling
	glDisable(GL_CULL_FACE);
	// Enable default blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

inline bool GLCreateContext(SDL_Window* window)
{
	glCtx = SDL_GL_CreateContext(window);
	if(glCtx == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext: %s",
		             SDL_GetError());
		return false;
	}
	return true;
}

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

// Load functions (called from LoadBackend)

inline bool LoadGLCore(SDL_Window* window)
{
	if(!GLCreateContext(window))
		return false;
#ifndef USE_PROTOTYPES_GL
#include "gl_es2_funcs.h"
#include "gl_core_funcs.h"
#endif
	GLLogStrings();
	GLEnableStuff();
	GLLoadPrograms();
	return true;
}

inline bool LoadGLES(SDL_Window* window)
{
	if(!GLCreateContext(window))
		return false;
#ifndef USE_PROTOTYPES_GL
#include "gl_es2_funcs.h"
#endif
	GLLogStrings();
	GLEnableStuff();
	GLLoadPrograms();
	return true;
}

#undef SDL_PROC

bool PreloadBackend(Backend backend)
{
	switch(backend)
	{
		case OPENGL_CORE:
		{
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
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
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
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

bool LoadBackend(SDL_Window* window, Backend backend)
{
	SDL_assert(window);
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
			if(!LoadGLCore(window))
				return false;
			break;
		}
		case OPENGL_ES:
		{
			if(!LoadGLES(window))
				return false;
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
	sdlWindow = window;
	return true;
}

void UnloadBackend()
{
	switch(activeBackend)
	{
		case OPENGL_CORE:
		{
			glPrimProg.reset();
			SDL_GL_DeleteContext(glCtx);
			break;
		}
		case OPENGL_ES:
		{
			glPrimProg.reset();
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
			SDL_GL_SwapWindow(sdlWindow);
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
			SDL_GL_GetDrawableSize(sdlWindow, w, h);
			glViewport(0, 0, *w, *h);
			break;
		}
		case NOT_LOADED: break;
	}
}

Primitive NewPrimitive()
{
	SDL_assert(activeBackend);
	switch(activeBackend)
	{
		case OPENGL_CORE:
		{
			return std::make_shared<Detail::GLCore::Primitive>(*glPrimProg,
			                                                   *glTexPrimProg);
		}
		case OPENGL_ES:
		{
			return std::make_shared<Detail::GLES::Primitive>(*glPrimProg,
			                                                 *glTexPrimProg);
		}
		case NOT_LOADED: break;
	}
	return std::make_shared<Detail::Dummy::Primitive>();
}

Texture NewTexture()
{
	SDL_assert(activeBackend);
	switch(activeBackend)
	{
		case OPENGL_CORE:
		{
			return std::make_shared<Detail::GLShared::Texture>();
		}
		case OPENGL_ES:
		{
			return std::make_shared<Detail::GLShared::Texture>();
		}
		case NOT_LOADED: break;
	}
	return std::make_shared<Detail::Dummy::Texture>();
}

} // API

} // Drawing
