#ifndef GL_MESH_HPP
#define GL_MESH_HPP
#include <vector>
#include <array>

#include "gl_common_include.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace OGL
{

class Mesh
{
public:
	// Position
	using VertexType = glm::vec3;
	using VerticesContainer = std::vector<VertexType>;
	// Color
	using ColorType = glm::vec4;
	using ColorsContainer = std::vector<ColorType>;
	// Texture Coordinate
	using TexCoordType = glm::vec2;
	using TexCoordsContainer = std::vector<TexCoordType>;
	
	Mesh();
	~Mesh();
	
	Mesh& SetVerticesBuffer(const VerticesContainer& vc);
	Mesh& SetColorsBuffer(const ColorsContainer& cc);
	Mesh& SetTexCoordBuffer(const TexCoordsContainer& tcc);
	
// 	Mesh& UpdateOneV(const VertexType& v, std::size_t pos);
// 	Mesh& UpdateOneC(const ColorType& c, std::size_t pos);
// 	Mesh& UpdateOneTC(const TexCoordType& tc, std::size_t pos);
	
	void BindBuffers();
private:
	GLuint vbo[GLAttrLoc::ATTR_COUNT];
	std::array<bool, GLAttrLoc::ATTR_COUNT> usedVBOs{{false, false, false}};
	
	void TryEnableBO(const GLAttrLoc attrLoc, const std::size_t size);
};

} // OGL

#endif // GL_MESH_HPP
