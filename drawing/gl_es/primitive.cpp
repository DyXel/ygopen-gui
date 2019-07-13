#include "primitive.hpp"

namespace Drawing
{

namespace Detail
{

static const std::size_t ATTR_LENGTHS[ATTR_COUNT] =
{
	Vertex::length(),
	Color::length()
};

GLESPrimitive::GLESPrimitive(const Program& program) : program(program)
{
	glGenBuffers(ATTR_COUNT, vbo.data());
	usedVbo.fill(false);
	vboSize.fill(0);
}

GLESPrimitive::~GLESPrimitive()
{
	glDeleteBuffers(ATTR_COUNT, vbo.data());
}

void GLESPrimitive::SetVertices(const Vertices& vertices)
{
	vboSize[ATTR_VERTICES] = vertices.size();
	const std::size_t numBytes = vboSize[ATTR_VERTICES] * VERTEX_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices.data(), GL_STATIC_DRAW);
	usedVbo[ATTR_VERTICES] = true;
}

void GLESPrimitive::SetColors(const Colors& colors)
{
	vboSize[ATTR_COLORS] = colors.size();
	const std::size_t numBytes = vboSize[ATTR_COLORS] * COLOR_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, colors.data(), GL_STATIC_DRAW);
	usedVbo[ATTR_COLORS] = true;
}

void GLESPrimitive::Draw()
{
	program.Use();
	TryEnableVBO(ATTR_VERTICES);
	TryEnableVBO(ATTR_COLORS);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vboSize[ATTR_VERTICES]);
}

void GLESPrimitive::TryEnableVBO(PrimitiveAttrLoc attrLoc)
{
	if(!usedVbo[attrLoc])
		return;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[attrLoc]);
	glVertexAttribPointer(attrLoc, ATTR_LENGTHS[attrLoc], GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(attrLoc);
}

} // Detail

} // Drawing
