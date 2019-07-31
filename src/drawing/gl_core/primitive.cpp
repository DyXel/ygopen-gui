#include "primitive.hpp"
#include "../gl_shared/program.hpp"
#include "../gl_shared/texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLCore
{

Primitive::Primitive(const GLShared::Program& program,
                     const GLShared::Program& texProgram) :
	program(program),
	texProgram(texProgram)
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
	drawCount = static_cast<GLsizei>(vertices.size());
	const std::size_t numBytes = vertices.size() * VERTEX_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(numBytes),
	             vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(GLShared::ATTR_VERTICES, VERTEX_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(GLShared::ATTR_VERTICES);
}

void Primitive::SetColors(const Colors& colors)
{
	const std::size_t numBytes = colors.size() * COLOR_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(numBytes),
	             colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(GLShared::ATTR_COLORS, COLOR_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(GLShared::ATTR_COLORS);
}

void Primitive::SetIndices(const Indices& indices)
{
	drawCount = static_cast<GLsizei>(indices.size());
	const std::size_t numBytes = indices.size() * INDEX_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[GLShared::ATTR_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(numBytes),
	             indices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(GLShared::ATTR_INDICES);
	drawByIndex = true;
}

void Primitive::SetMatrix(const Matrix& matrix)
{
	mat = matrix;
}

void Primitive::SetTexCoords(const TexCoords& texCoords)
{
	const std::size_t numBytes = texCoords.size() * TEXCOORD_SIZE;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(numBytes),
	             texCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(GLShared::ATTR_TEXCOORDS, TEXCOORD_LENGTH, GL_FLOAT,
	                      GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(GLShared::ATTR_TEXCOORDS);
}

void Primitive::SetTexture(const Drawing::Texture& texture)
{
	tex = std::dynamic_pointer_cast<GLShared::Texture>(texture);
}

void Primitive::Draw()
{
	if(tex)
	{
		texProgram.Use();
		tex->Bind();
		texProgram.SetModelMatrix(mat);
	}
	else
	{
		program.Use();
		glBindTexture(GL_TEXTURE_2D, 0);
		program.SetModelMatrix(mat);
	}
	glBindVertexArray(vao);
	if(drawByIndex)
		glDrawElements(mode, drawCount, GL_UNSIGNED_SHORT, nullptr);
	else
		glDrawArrays(mode, 0, drawCount);
}

} // GLCore

} // Detail

} // Drawing
