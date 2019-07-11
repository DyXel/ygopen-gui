#include "gl_shader_and_program.hpp"
#include <SDL_log.h>

namespace OGL
{

static const char* attrNames[ATTR_COUNT] =
{
	"pos", // VERTPOS
	"color", // COLOR
	"texCoord" // TEXCOORD
};

// Shader IMPLEMENTATION

Shader::Shader(const GLenum type, std::string_view source)
	: type(type)
{
	ref = ctx.glCreateShader(type);
	if(ref == 0)
		return; // TODO: add log info
	const char* src[] = {source.data()};
	GL_CHECK(ctx.glShaderSource(ref, 1, src, NULL));
	GL_CHECK(ctx.glCompileShader(ref));
	GLint success;
	GL_CHECK(ctx.glGetShaderiv(ref, GL_COMPILE_STATUS, &success));
	if(success == GL_FALSE)
	{
		GLint logLength;
		GL_CHECK(ctx.glGetShaderiv(ref, GL_INFO_LOG_LENGTH, &logLength));
		auto logText = new char[logLength];
		GL_CHECK(ctx.glGetShaderInfoLog(ref, logLength, NULL, logText));
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Could not compile shader: %s", logText);
		delete[] logText;
		return;
	}
	compiled = true;
}

Shader::~Shader()
{
	// NOTE: From reference: A value of 0 for shader will be silently ignored.
	GL_CHECK(ctx.glDeleteShader(ref));
}

GLuint Shader::GetGLRef() const
{
	return ref;
}

// Program IMPLEMENTATION

Program::Program()
{
	ref = ctx.glCreateProgram();
	if(ref == 0)
		return; // TODO: add log info
}

Program::~Program()
{
	// NOTE: From reference: A value of 0 for program will be silently ignored.
	ctx.glDeleteProgram(ref);
}

Program& Program::Attach(const Shader& shader)
{
	GL_CHECK(ctx.glAttachShader(ref, shader.GetGLRef()));
	return *this;
}

bool Program::Link()
{
	// Bind attribute names to their indexes
	for(int i = 0; i < ATTR_COUNT; i++)
		GL_CHECK(ctx.glBindAttribLocation(ref, i, attrNames[i]));
	// Link program
	GL_CHECK(ctx.glLinkProgram(ref));
	GLint success;
	GL_CHECK(ctx.glGetProgramiv(ref, GL_LINK_STATUS, &success));
	if(success == GL_FALSE)
	{
		GLint logLength;
		GL_CHECK(ctx.glGetProgramiv(ref, GL_INFO_LOG_LENGTH, &logLength));
		auto logText = new char[logLength];
		GL_CHECK(ctx.glGetProgramInfoLog(ref, logLength, NULL, logText));
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Could not link program: %s", logText);
		delete[] logText;
		return false;
	}
	return true;
}

void Program::Use()
{
	ctx.glUseProgram(ref);
}

GLuint Program::GetGLRef() const
{
	return ref;
}

} // OGL
