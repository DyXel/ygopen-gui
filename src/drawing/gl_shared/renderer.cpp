#include "renderer.hpp"

#include <SDL.h>

#include "../gl_include.hpp"
#include "texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

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
	vec4 ogColor = texture2D(in_tex, out_texCoord);
	vec4 brightness = vec4(in_brightness, in_brightness, in_brightness, 0.0);
	gl_FragColor = ((out_color + ogColor) * brightness) +
	               vec4(0, 0, 0, ogColor.w);
})";

Renderer::Renderer(SDL_Window* sdlWindow) : sdlWindow(sdlWindow)
{
	// Enable face culling
	glEnable(GL_CULL_FACE);
	
	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Clear();
	
	// Create programs
	{
		Shader vs(GL_VERTEX_SHADER, PRIMITIVE_VERTEX_SHADER_SRC);
		Shader fs(GL_FRAGMENT_SHADER, PRIMITIVE_FRAGMENT_SHADER_SRC);
		primProg.Attach(vs);
		primProg.Attach(fs);
		primProg.Link();
	}
	
	{
		Shader vs(GL_VERTEX_SHADER, TEXTURED_PRIMITIVE_VERTEX_SHADER_SRC);
		Shader fs(GL_FRAGMENT_SHADER, TEXTURED_PRIMITIVE_FRAGMENT_SHADER_SRC);
		texPrimProg.Attach(vs);
		texPrimProg.Attach(fs);
		texPrimProg.Link();
	}
}

void Renderer::Clear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Present()
{
	SDL_GL_SwapWindow(sdlWindow);
}

void Renderer::SetViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

Drawing::Texture Renderer::NewTexture()
{
	return std::make_shared<Texture>();
}

bool Renderer::SetVSync([[maybe_unused]] VSyncState state)
{
	switch(state)
	{
		case VSyncState::VSYNC_STATE_DISABLED:
		{
			return SDL_GL_SetSwapInterval(0) == 0;
		}
		case VSyncState::VSYNC_STATE_ENABLED:
		{
			return SDL_GL_SetSwapInterval(1) == 0;
		}
		case VSyncState::VSYNC_STATE_ADAPTIVE:
		{
			return SDL_GL_SetSwapInterval(-1) == 0;
		}
	}
	return false;
}

}  // namespace GLShared

}  // namespace Detail

}  // namespace Drawing
