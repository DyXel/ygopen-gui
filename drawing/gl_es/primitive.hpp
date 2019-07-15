#ifndef DRAWING_GL_ES_PRIMITIVE_HPP
#define DRAWING_GL_ES_PRIMITIVE_HPP
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

namespace GLES
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
	std::array<GLuint, GLShared::ATTR_COUNT> vbo;
	std::array<bool, GLShared::ATTR_COUNT> usedVbo;
	std::array<std::size_t, GLShared::ATTR_COUNT> vboSize;
	
	void TryEnableVBO(const GLShared::AttrLocation& attrLoc);
};

} // GLES

} // Detail

} // Drawing

#endif // DRAWING_GL_ES_PRIMITIVE_HPP
