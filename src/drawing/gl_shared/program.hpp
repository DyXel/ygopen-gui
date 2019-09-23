#ifndef DRAWING_GL_SHARED_PROGRAM_HPP
#define DRAWING_GL_SHARED_PROGRAM_HPP
#include <array>
#include "../gl_include.hpp"
#include "../types.hpp"

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
	bool Link();
	void Use() const;
	
	void SetModelMatrix(const glm::mat4& mat) const;
	void SetBrightness(const GLfloat val) const;
	
	GLuint GetGLRef() const;
private:
	GLuint ref{0};
	enum {UNI_MODEL_MAT, UNI_BRIGHTNESS_FLOAT, UNI_COUNT};
	std::array<GLint, UNI_COUNT> uni{};
};

} // GLShared

} // Detail

} // Drawing

#endif // DRAWING_GL_SHARED_PROGRAM_HPP
