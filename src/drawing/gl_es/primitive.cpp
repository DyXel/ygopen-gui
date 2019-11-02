#include "primitive.hpp"
#include "../gl_shared/program.hpp"
#include "../gl_shared/texture.hpp"

namespace Drawing
{

namespace Detail
{

namespace GLES
{

Primitive::Primitive(const GLShared::Program& program,
                     const GLShared::Program& texProgram) :
	program(program),
	texProgram(texProgram)
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
	drawCount = static_cast<GLsizei>(vertices.size());
	const std::size_t numBytes = vertices.size() * VERTEX_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER,  static_cast<GLsizeiptr>(numBytes),
	             vertices.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_VERTICES] = true;
}

void Primitive::SetColors(const Colors& colors)
{
	const std::size_t numBytes = colors.size() * COLOR_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_COLORS]);
	glBufferData(GL_ARRAY_BUFFER,  static_cast<GLsizeiptr>(numBytes),
	             colors.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_COLORS] = true;
}

void Primitive::SetIndices(const Indices& indices)
{
	drawCount = static_cast<GLsizei>(indices.size());
	const std::size_t numBytes = indices.size() * INDEX_SIZE;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[GLShared::ATTR_INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  static_cast<GLsizeiptr>(numBytes),
	             indices.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_INDICES] = true;
}

void Primitive::SetMatrix(const glm::mat4& matrix)
{
	mat = matrix;
}

void Primitive::SetBrightness(const float val)
{
	brightness = val;
}

void Primitive::SetTexCoords(const TexCoords& texCoords)
{
	const std::size_t numBytes = texCoords.size() * TEXCOORD_SIZE;
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLShared::ATTR_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER,  static_cast<GLsizeiptr>(numBytes),
	             texCoords.data(), GL_STATIC_DRAW);
	usedVbo[GLShared::ATTR_TEXCOORDS] = true;
}

void Primitive::SetTexture(const Drawing::Texture& texture)
{
	tex = std::dynamic_pointer_cast<GLShared::Texture>(texture);
}

void Primitive::Draw()
{
	if(depthTest)
		glDepthFunc(GL_LESS);
	else
		glDepthFunc(GL_ALWAYS);
	if(tex)
	{
		texProgram.Use();
		tex->Bind();
		texProgram.SetModelMatrix(mat);
		texProgram.SetBrightness(brightness);
	}
	else
	{
		program.Use();
		glBindTexture(GL_TEXTURE_2D, 0);
		program.SetModelMatrix(mat);
		texProgram.SetBrightness(brightness);
	}
	TryEnableVBO(GLShared::ATTR_VERTICES);
	TryEnableVBO(GLShared::ATTR_COLORS);
	TryEnableVBO(GLShared::ATTR_TEXCOORDS);
	// TODO(dyxel): indices are never bound. 
	// This will break as soon as using diff indices
// 	TryEnableVBO(GLShared::ATTR_INDICES);
	if(usedVbo[GLShared::ATTR_INDICES])
		glDrawElements(mode, drawCount, GL_UNSIGNED_SHORT, nullptr);
	else
		glDrawArrays(mode, 0, drawCount);
}

void Primitive::TryEnableVBO(const GLShared::AttrLocation& attrLoc)
{
	static constexpr GLint ATTR_LENGTHS[GLShared::ATTR_COUNT] =
	{
		static_cast<GLint>(VERTEX_LENGTH),
		static_cast<GLint>(COLOR_LENGTH),
		static_cast<GLint>(TEXCOORD_LENGTH),
	};
	if(!usedVbo[attrLoc])
	{
		glDisableVertexAttribArray(attrLoc);
		return;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[attrLoc]);
	glVertexAttribPointer(attrLoc, ATTR_LENGTHS[attrLoc], GL_FLOAT,
	                      GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(attrLoc);
}

}  // namespace GLES

}  // namespace Detail

}  // namespace Drawing
