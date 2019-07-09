#ifndef GL_SHADER_AND_PROGRAM_HPP
#define GL_SHADER_AND_PROGRAM_HPP
#include <string_view>

#include "gl_common_include.h"

namespace OGL
{

class Shader
{
public:
	Shader(const GLenum type, std::string_view source);
	~Shader();
	
	bool IsCompiled() const;
	GLuint GetGLRef() const;
private:
	bool compiled{false};
	GLuint ref{0};
	GLenum type;
// 	std::string source;
};

class Program
{
public:
	Program();
	~Program();
	
	Program& Attach(const Shader& shader);
	bool Link();
	void Use();
	
	GLuint GetGLRef() const;
private:
	GLuint ref{0};
};

} // OGL

#endif // GL_SHADER_AND_PROGRAM_HPP
