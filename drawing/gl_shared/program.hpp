#ifndef DRAWING_GL_SHARED_PROGRAM_HPP
#define DRAWING_GL_SHARED_PROGRAM_HPP
#include "../gl_include.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{

class Shader;

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

#endif // DRAWING_GL_SHARED_PROGRAM_HPP
