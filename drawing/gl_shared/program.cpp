#include "program.hpp"
#include <SDL_log.h>

#include "common.hpp"
#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

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

bool Program::Link()
{
	// Bind attribute names to their indexes
	static const char* ATTR_NAMES[ATTR_COUNT] =
	{
		"in_pos",
		"in_color",
		"in_texCoord",
		"in_index", // unused?
	};
	for(std::size_t i = 0; i < ATTR_COUNT; i++)
		glBindAttribLocation(ref, static_cast<GLuint>(i), ATTR_NAMES[i]);
	// Link program
	glLinkProgram(ref);
	GLint success;
	glGetProgramiv(ref, GL_LINK_STATUS, &success);
	if(success == GL_FALSE)
	{
		GLint logLength;
		glGetProgramiv(ref, GL_INFO_LOG_LENGTH, &logLength);
		auto logText = new char[static_cast<std::size_t>(logLength)];
		glGetProgramInfoLog(ref, logLength, nullptr, logText);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Could not link program: %s", logText);
		delete[] logText;
		return false;
	}
	// Get uniform locations (if it has them)
	static const char* UNI_NAMES[UNI_COUNT] =
	{
		"in_model",
	};
	for(std::size_t i = 0; i < UNI_COUNT; i++)
		uni[i] = glGetUniformLocation(ref, UNI_NAMES[i]);
	return true;
}

void Program::Use() const
{
	glUseProgram(ref);
}

void Program::SetModelMatrix(const Matrix& mat) const
{
	if(uni[UNI_MODEL_MAT] == -1)
		return;
	glUniformMatrix4fv(uni[UNI_MODEL_MAT], 1, GL_FALSE, glm::value_ptr(mat));
}

GLuint Program::GetGLRef() const
{
	return ref;
}

} // GLShared

} // Detail

} // Drawing
