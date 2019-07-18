#ifndef DRAWING_TYPES_HPP
#define DRAWING_TYPES_HPP
#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat4x4.hpp>

namespace Drawing
{

// Position
using Vertex = glm::vec3;
using Vertices = std::vector<Vertex>;
constexpr std::size_t VERTEX_LENGTH = 3;
constexpr std::size_t VERTEX_SIZE = sizeof(Vertex);

// Color
using Color = glm::vec4;
using Colors = std::vector<Color>;
constexpr std::size_t COLOR_LENGTH = 4;
constexpr std::size_t COLOR_SIZE = sizeof(Color);

// Texture Coordinate
using TexCoord = glm::vec2;
using TexCoords = std::vector<TexCoord>;
constexpr std::size_t TEXCOORD_LENGTH = 2;
constexpr std::size_t TEXCOORD_SIZE = sizeof(TexCoord);

// Index
using Index = short;
using Indices = std::vector<Index>;
constexpr std::size_t INDEX_SIZE = sizeof(Index);

// Matrix
using Matrix = glm::mat4;
constexpr std::size_t MATRIX_SIZE = sizeof(Matrix);

// Primitive drawing modes
enum PrimitiveDrawMode
{
	PDM_POINTS = 0, // Separated points
	PDM_LINE_STRIP = 1, // Lines, current one connected to previous one
	PDM_LINE_LOOP = 2, // Lines, same as above, last connected to first
	PDM_LINES = 3, // Lines, each one taking a span of 2 vertices
	PDM_TRIANGLE_STRIP = 4, // Triangles, last two vertices connected to current
	PDM_TRIANGLE_FAN = 5, // Triangles, current connected to first and last
	PDM_TRIANGLES = 6 // Triangles, each one taking a span of 3 vertices
};

namespace Detail
{
class IPrimitive;
class ITexture;
}

using Primitive = std::shared_ptr<class Detail::IPrimitive>;
using Texture = std::shared_ptr<class Detail::ITexture>;

} // Drawing
#endif // DRAWING_TYPES_HPP
