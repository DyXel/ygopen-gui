#include "shader_and_program.hpp"
#include <SDL.h>

#include "common.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

// Shader IMPLEMENTATION

Shader::Shader(const GLenum type, std::string_view source)
	: type(type)
{
	ref = glCreateShader(type);
	if(ref == 0)
		return; // TODO: add log info
	const char* src[] = {source.data()};
	GL_CHECK(glShaderSource(ref, 1, src, NULL));
	GL_CHECK(glCompileShader(ref));
	GLint success;
	GL_CHECK(glGetShaderiv(ref, GL_COMPILE_STATUS, &success));
	if(success == GL_FALSE)
	{
		GLint logLength;
		GL_CHECK(glGetShaderiv(ref, GL_INFO_LOG_LENGTH, &logLength));
		auto logText = new char[logLength];
		GL_CHECK(glGetShaderInfoLog(ref, logLength, NULL, logText));
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
	GL_CHECK(glDeleteShader(ref));
}

GLuint Shader::GetGLRef() const
{
	return ref;
}

// Program IMPLEMENTATION

Program::Program()
{
	ref = glCreateProgram();
	if(ref == 0)
		return; // TODO: add log info
}

Program::~Program()
{
	// NOTE: From reference: A value of 0 for program will be silently ignored.
	glDeleteProgram(ref);
}

Program& Program::Attach(const Shader& shader)
{
	GL_CHECK(glAttachShader(ref, shader.GetGLRef()));
	return *this;
}

bool Program::Link() const
{
	// Bind attribute names to their indexes
	static const char* ATTR_NAMES[ATTR_COUNT] =
	{
		"pos", // VERTPOS
		"color", // COLOR
	};
	for(int i = 0; i < ATTR_COUNT; i++)
		GL_CHECK(glBindAttribLocation(ref, i, ATTR_NAMES[i]));
	// Link program
	GL_CHECK(glLinkProgram(ref));
	GLint success;
	GL_CHECK(glGetProgramiv(ref, GL_LINK_STATUS, &success));
	if(success == GL_FALSE)
	{
		GLint logLength;
		GL_CHECK(glGetProgramiv(ref, GL_INFO_LOG_LENGTH, &logLength));
		auto logText = new char[logLength];
		GL_CHECK(glGetProgramInfoLog(ref, logLength, NULL, logText));
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Could not link program: %s", logText);
		delete[] logText;
		return false;
	}
	return true;
}

void Program::Use() const
{
	glUseProgram(ref);
}

GLuint Program::GetGLRef() const
{
	return ref;
}

} // GLES

} // Detail

} // Drawing
