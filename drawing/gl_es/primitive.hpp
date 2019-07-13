#ifndef DRAWING_GL_ES_PRIMITIVE_HPP
#define DRAWING_GL_ES_PRIMITIVE_HPP
#include <array>
#include "../gl_include.hpp"
#include "../primitive.hpp"
#include "../types.hpp"
#include "common.hpp"
#include "shader_and_program.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

class Primitive : public IPrimitive
{
public:
	Primitive(const Program& program);
	virtual ~Primitive();
	
	void SetVertices(const Vertices& vertices);
	void SetColors(const Colors& colors);
// 	void SetTransform();
	
	void Draw();
	
	// Extended public functions
	void SetProgram(const Program& program);
private:
	const Program& program;
	std::array<GLuint, ATTR_COUNT> vbo;
	std::array<bool, ATTR_COUNT> usedVbo;
	std::array<std::size_t, ATTR_COUNT> vboSize;
	
	void TryEnableVBO(PrimitiveAttrLoc attrLoc);
};

} // GLES

} // Detail

} // Drawing

#endif // DRAWING_GL_ES_PRIMITIVE_HPP
