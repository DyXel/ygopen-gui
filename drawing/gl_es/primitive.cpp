#include "primitive.hpp"
#include "../gl_shared/program.hpp"
#include "../gl_shared/texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

Primitive::Primitive(const GLShared::Program& program) : program(program)
{
	glGenBuffers(GLShared::ATTR_COUNT, vbo.data());
	usedVbo.fill(false);
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
	drawCount = vertices.size();
	const std::size_t numBytes = drawCount * VERTEX_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, vertices.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_VERTICES] = true;
}

void Primitive::SetColors(const Colors& colors)
{
	const std::size_t numBytes = colors.size() * COLOR_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, colors.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_COLORS] = true;
}

void Primitive::SetIndices(const Indices& indices)
{
	drawCount = indices.size();
	const std::size_t numBytes = indices.size() * INDEX_SIZE;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[GLShared::ATTR_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numBytes, indices.data(),
	             GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_INDICES] = true;
}

void Primitive::SetTexCoords(const TexCoords& texCoords)
{
	const std::size_t numBytes = texCoords.size() * TEXCOORD_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER, numBytes, texCoords.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_TEXCOORDS] = true;
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
	TryEnableVBO(GLShared::ATTR_VERTICES);
	TryEnableVBO(GLShared::ATTR_COLORS);
	TryEnableVBO(GLShared::ATTR_TEXCOORDS);
	TryEnableVBO(GLShared::ATTR_INDICES);
	if(usedVbo[GLShared::ATTR_INDICES])
		glDrawElements(mode, drawCount, GL_UNSIGNED_SHORT, (GLvoid*)0);
	else
		glDrawArrays(mode, 0, drawCount);
}

void Primitive::SetMatrix(const Matrix& matrix)
{
	mat = matrix;
}

void Primitive::TryEnableVBO(const GLShared::AttrLocation& attrLoc)
{
	static constexpr std::size_t ATTR_LENGTHS[GLShared::ATTR_COUNT] =
	{
		VERTEX_LENGTH,
		COLOR_LENGTH,
		TEXCOORD_LENGTH
	};
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
