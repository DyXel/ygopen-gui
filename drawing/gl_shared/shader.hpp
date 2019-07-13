#ifndef DRAWING_GL_SHARED_SHADER_HPP
#define DRAWING_GL_SHARED_SHADER_HPP
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

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_SHADER_HPP
