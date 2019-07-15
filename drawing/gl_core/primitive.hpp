#ifndef DRAWING_GL_CORE_PRIMITIVE_HPP
#define DRAWING_GL_CORE_PRIMITIVE_HPP
#include <array>
#include "../gl_include.hpp"
#include "../primitive.hpp"
#include "../types.hpp"
#include "../gl_shared/common.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLShared
{
class Program;
} // GLShared

namespace GLCore
{

class Primitive : public IPrimitive
{
public:
	Primitive(const GLShared::Program& program);
	virtual ~Primitive();
	
	void SetDrawMode(const PrimitiveDrawMode& pdm);
	void SetVertices(const Vertices& vertices);
	void SetColors(const Colors& colors);
// 	void SetTransform();
	
	void Draw();
	
	// Extended public functions
	void SetProgram(const GLShared::Program& program);
private:
	const GLShared::Program& program;
	GLenum mode{GL_TRIANGLES};
	GLuint vao;
	std::array<GLuint, GLShared::ATTR_COUNT> vbo;
	std::size_t vertexCount;
};

} // GLCore

} // Detail

} // Drawing

#endif // DRAWING_GL_CORE_PRIMITIVE_HPP
