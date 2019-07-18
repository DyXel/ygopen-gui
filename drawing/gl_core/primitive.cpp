#include "primitive.hpp"
#include "../gl_shared/program.hpp"
#include "../gl_shared/texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLCore
{

Primitive::Primitive(const GLShared::Program& program) : program(program)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(GLShared::ATTR_COUNT, vbo.data());
}

Primitive::~Primitive()
{
	glDeleteBuffers(GLShared::ATTR_COUNT, vbo.data());
	glDeleteVertexArrays(1, &vao);
}

void Primitive::SetDrawMode(const PrimitiveDrawMode& pdm)
{
	mode = GLShared::GLDrawModeFromPDM(pdm);
}

void Primitive::SetVertices(const Vertices& vertices)
{
	drawCount = vertices.size();
	const std::size_t numBytes = drawCount * VERTEX_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(GLShared::ATTR_VERTICES, VERTEX_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(GLShared::ATTR_VERTICES);
}

void Primitive::SetColors(const Colors& colors)
{
	const std::size_t numBytes = colors.size() * COLOR_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(GLShared::ATTR_COLORS, COLOR_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(GLShared::ATTR_COLORS);
}

void Primitive::SetIndices(const Indices& indices)
{
	drawCount = indices.size();
	const std::size_t numBytes = indices.size() * INDEX_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[GLShared::ATTR_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, indices.data(),
	             GL_STATIC_DRAW);
	glEnableVertexAttribArray(GLShared::ATTR_INDICES);
	drawByIndex = true;
}

void Primitive::SetMatrix(const Matrix& matrix)
{
	mat = matrix;
}

void Primitive::SetTexture(const Drawing::Texture& texture)
{
	tex = std::dynamic_pointer_cast<GLShared::Texture>(texture);
}

void Primitive::Draw()
{
	program.Use();
	if(tex.use_count() > 0)
		tex->Bind();
	else
		glBindTexture(GL_TEXTURE_2D, 0);
	program.SetModelMatrix(mat);
	glBindVertexArray(vao);
	if(drawByIndex)
		glDrawElements(mode, drawCount, GL_UNSIGNED_SHORT, (GLvoid*)0);
	else
		glDrawArrays(mode, 0, drawCount);
}

} // GLCore

} // Detail

} // Drawing
