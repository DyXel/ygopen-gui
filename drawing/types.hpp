#ifndef DRAWING_TYPES_HPP
#define DRAWING_TYPES_HPP
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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

// Index
using Index = glm::vec2;
using Indeces = std::vector<Index>;
constexpr std::size_t INDEX_LENGTH = 2;
constexpr std::size_t INDEX_SIZE = sizeof(Index);

// Texture Coordinate
using TexCoord = glm::vec2;
using TexCoords = std::vector<TexCoord>;
constexpr std::size_t TEXCOORD_LENGTH = 2;
constexpr std::size_t TEXCOORD_SIZE = sizeof(TexCoord);

} // Drawing
#endif // DRAWING_TYPES_HPP
