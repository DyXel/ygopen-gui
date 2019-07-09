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
	ref = glCreateShader(type);
	if(ref == 0)
		return; // TODO: add log info
	const char* src[] = {source.data()};
	glShaderSource(ref, 1, src, NULL);
	glCompileShader(ref);
	GLint success;
	glGetShaderiv(ref, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE)
	{
		GLint logLength;
		glGetShaderiv(ref, GL_INFO_LOG_LENGTH, &logLength);
		auto logText = new char[logLength];
		glGetShaderInfoLog(ref, logLength, NULL, logText);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Could not compile shader: %s", logText);
		delete[] logText;
		return;
	}
	compiled = true;
	
	// Set attribute names
	for(int i = 0; i < ATTR_COUNT; i++)
		glBindAttribLocation(ref, i, attrNames[i]);
}

Shader::~Shader()
{
	// NOTE: From reference: A value of 0 for shader will be silently ignored.
	glDeleteShader(ref);
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
	glAttachShader(ref, shader.GetGLRef());
	return *this;
}

bool Program::Link()
{
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

void Program::Use()
{
	glUseProgram(ref);
}

GLuint Program::GetGLRef() const
{
	return ref;
}

} // OGL
