#include "shader.hpp"
#include <SDL_log.h>

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

Shader::Shader(const GLenum type, std::string_view source) : type(type)
{
	ref = glCreateShader(type);
	if(ref == 0)
		return;
	const char* src[] = {source.data()};
	glShaderSource(ref, 1, src, nullptr);
	glCompileShader(ref);
	GLint success;
	glGetShaderiv(ref, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE)
	{
		GLint logLength;
		glGetShaderiv(ref, GL_INFO_LOG_LENGTH, &logLength);
		auto logText = new char[static_cast<std::size_t>(logLength)];
		glGetShaderInfoLog(ref, logLength, nullptr, logText);
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
	glDeleteShader(ref);
}

GLuint Shader::GetGLRef() const
{
	return ref;
}

}  // namespace GLShared

}  // namespace Detail

}  // namespace Drawing
