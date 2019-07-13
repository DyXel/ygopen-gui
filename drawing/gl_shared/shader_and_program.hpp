#ifndef DRAWING_GL_SHARED_SHADER_AND_PROGRAM_HPP
#define DRAWING_GL_SHARED_SHADER_AND_PROGRAM_HPP
#include <string_view>
#include "../gl_include.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
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
	
	void Attach(const Shader& shader);
	bool Link() const;
	void Use() const;
	
	GLuint GetGLRef() const;
private:
	GLuint ref{0};
};

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_SHADER_AND_PROGRAM_HPP
