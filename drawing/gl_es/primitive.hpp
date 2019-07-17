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
	
	void SetDrawMode(const PrimitiveDrawMode& pdm) override;
	void SetVertices(const Vertices& vertices) override;
	void SetColors(const Colors& colors) override;
	void SetIndices(const Indices& indices) override;
	
	void SetMatrix(const Matrix& matrix) override;
	
	void Draw() override;
	
	// Extended public functions
	void SetProgram(const GLShared::Program& program);
private:
	const GLShared::Program& program;
	std::array<GLuint, GLShared::ATTR_COUNT> vbo;
	std::array<bool, GLShared::ATTR_COUNT> usedVbo;
	GLenum mode{GL_TRIANGLES};
	std::size_t drawCount{0};
	Matrix mat{1.0f};
	
	void TryEnableVBO(const GLShared::AttrLocation& attrLoc);
};

} // GLES

} // Detail

} // Drawing

#endif // DRAWING_GL_ES_PRIMITIVE_HPP
