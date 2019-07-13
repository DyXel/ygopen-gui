#include "primitive.hpp"
#include "../gl_shared/program.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLCore
{

static const std::size_t ATTR_LENGTHS[ATTR_COUNT] =
{
	Vertex::length(),
	Color::length()
};

Primitive::Primitive(const GLShared::Program& program) : program(program)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(ATTR_COUNT, vbo.data());
}

Primitive::~Primitive()
{
	glDeleteBuffers(ATTR_COUNT, vbo.data());
	glDeleteVertexArrays(1, &vao);
}

void Primitive::SetVertices(const Vertices& vertices)
{
	vertexCount = vertices.size();
	const std::size_t numBytes = vertexCount * VERTEX_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(ATTR_VERTICES, VERTEX_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(ATTR_VERTICES);
}

void Primitive::SetColors(const Colors& colors)
{
	const std::size_t numBytes = colors.size() * COLOR_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(ATTR_COLORS, COLOR_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(ATTR_COLORS);
}

void Primitive::Draw()
{
	program.Use();
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
}

} // GLCore

} // Detail

} // Drawing
