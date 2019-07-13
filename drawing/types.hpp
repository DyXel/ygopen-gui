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
inline extern const std::size_t VERTEX_SIZE = sizeof(Vertex::value_type) *
                                              Vertex::length();
// Color
using Color = glm::vec4;
using Colors = std::vector<Color>;
inline extern const std::size_t COLOR_SIZE = sizeof(Color::value_type) *
                                      Color::length();
// Texture Coordinate
using TexCoord = glm::vec2;
using TexCoords = std::vector<TexCoord>;
inline extern const std::size_t TEXCOORD_SIZE = sizeof(TexCoord::value_type) *
                                                TexCoord::length();

} // Drawing
#endif // DRAWING_TYPES_HPP
