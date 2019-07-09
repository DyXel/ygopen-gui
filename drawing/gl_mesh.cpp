#include "gl_mesh.hpp"

namespace OGL
{

static const std::size_t VVV = sizeof(Mesh::VertexType::value_type) * Mesh::VertexType::length();
static const std::size_t CCC = sizeof(Mesh::ColorType::value_type) * Mesh::ColorType::length();
static const std::size_t TCTCTC = sizeof(Mesh::TexCoordType::value_type) * Mesh::TexCoordType::length();

Mesh::Mesh()
{
	glGenBuffers(GLAttrLoc::ATTR_COUNT, vbo);
}

Mesh::~Mesh()
{
	glDeleteBuffers(GLAttrLoc::ATTR_COUNT, vbo);
}

Mesh& Mesh::SetVerticesBuffer(const VerticesContainer& vc)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLAttrLoc::VERTPOS]);
	glBufferData(GL_ARRAY_BUFFER, vc.size() * VVV, vc.data(), GL_STATIC_DRAW);
	usedVBOs[GLAttrLoc::VERTPOS] = true;
	return *this;
}

Mesh& Mesh::SetColorsBuffer(const ColorsContainer& cc)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLAttrLoc::COLOR]);
	glBufferData(GL_ARRAY_BUFFER, cc.size() * CCC, cc.data(), GL_STATIC_DRAW);
	usedVBOs[GLAttrLoc::COLOR] = true;
	return *this;
}

Mesh& Mesh::SetTexCoordBuffer(const TexCoordsContainer& tcc)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[GLAttrLoc::TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, tcc.size() * TCTCTC, tcc.data(), GL_STATIC_DRAW);
	usedVBOs[GLAttrLoc::TEXCOORD] = true;
	return *this;
}

void Mesh::BindBuffers()
{
	TryEnableBO(GLAttrLoc::VERTPOS, VertexType::length());
	TryEnableBO(GLAttrLoc::COLOR, ColorType::length());
	TryEnableBO(GLAttrLoc::TEXCOORD, TexCoordType::length());
}

void Mesh::TryEnableBO(const GLAttrLoc attrLoc, const std::size_t size)
{
	if(usedVBOs[attrLoc])
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[attrLoc]);
		glVertexAttribPointer(attrLoc, size, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(attrLoc);
	}
}

} // OGL
