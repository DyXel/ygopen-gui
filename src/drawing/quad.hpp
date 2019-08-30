#ifndef DRAWING_QUAD_HPP
#define DRAWING_QUAD_HPP
#include <optional>
#include "types.hpp"

namespace Drawing
{

using OptFloat = std::optional<float>;

Matrix Get2DProjMatrix(int w, int h);

Vertices GetQuadVertices(OptFloat w, OptFloat h);
const TexCoords GetQuadTexCoords();
PrimitiveDrawMode GetQuadDrawMode();

void ResizeQuad(Vertices& vertices, OptFloat w, OptFloat h);

} // Drawing

#endif // DRAWING_QUAD_HPP
