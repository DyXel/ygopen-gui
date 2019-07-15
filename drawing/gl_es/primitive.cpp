#include "primitive.hpp"
#include "../gl_shared/program.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

static constexpr std::size_t ATTR_LENGTHS[GLShared::ATTR_COUNT] =
{
	VERTEX_LENGTH,
	COLOR_LENGTH
};

Primitive::Primitive(const GLShared::Program& program) : program(program)
{
	glGenBuffers(GLShared::ATTR_COUNT, vbo.data());
	usedVbo.fill(false);
	vboSize.fill(0);
}

Primitive::~Primitive()
{
	glDeleteBuffers(GLShared::ATTR_COUNT, vbo.data());
}

void Primitive::SetDrawMode(const PrimitiveDrawMode& pdm)
{
	mode = GLShared::GLDrawModeFromPDM(pdm);
}

void Primitive::SetVertices(const Vertices& vertices)
{
	vboSize[GLShared::ATTR_VERTICES] = vertices.size();
	const std::size_t numBytes = vboSize[GLShared::ATTR_VERTICES] * VERTEX_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_VERTICES] = true;
}

void Primitive::SetColors(const Colors& colors)
{
	vboSize[GLShared::ATTR_COLORS] = colors.size();
	const std::size_t numBytes = vboSize[GLShared::ATTR_COLORS] * COLOR_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, colors.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_COLORS] = true;
}

void Primitive::Draw()
{
	program.Use();
	TryEnableVBO(GLShared::ATTR_VERTICES);
	TryEnableVBO(GLShared::ATTR_COLORS);
	glDrawArrays(mode, 0, vboSize[GLShared::ATTR_VERTICES]);
}

void Primitive::TryEnableVBO(const GLShared::AttrLocation& attrLoc)
{
	if(!usedVbo[attrLoc])
		return;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[attrLoc]);
	glVertexAttribPointer(attrLoc, ATTR_LENGTHS[attrLoc], GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(attrLoc);
}

} // GLES

} // Detail

} // Drawing
