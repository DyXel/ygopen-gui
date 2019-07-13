#include "program.hpp"
#include <SDL_log.h>

#include "common.hpp"
#include "shader.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

Program::Program()
{
	ref = glCreateProgram();
	if(ref == 0)
		return;
}

Program::~Program()
{
	// NOTE: From reference: A value of 0 for program will be silently ignored.
	glDeleteProgram(ref);
}

void Program::Attach(const Shader& shader)
{
	glAttachShader(ref, shader.GetGLRef());
}

bool Program::Link() const
{
	// Bind attribute names to their indexes
	static const char* ATTR_NAMES[ATTR_COUNT] =
	{
		"pos",
		"color",
		"index",
		"texCoord"
	};
	for(int i = 0; i < ATTR_COUNT; i++)
		glBindAttribLocation(ref, i, ATTR_NAMES[i]);
	// Link program
	glLinkProgram(ref);
	GLint success;
	glGetProgramiv(ref, GL_LINK_STATUS, &success);
	if(success == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(ref, GL_INFO_LOG_LENGTH, &logLength);
		auto logText = new char[logLength];
		glGetProgramInfoLog(ref, logLength, NULL, logText);
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

} // GLShared

} // Detail

} // Drawing
