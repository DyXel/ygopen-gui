#include "primitive.hpp"

namespace Drawing
{

namespace Detail
{

namespace Dummy
{

Primitive::Primitive()
{}

Primitive::~Primitive()
{}

void Primitive::SetDrawMode([[maybe_unused]] const PrimitiveDrawMode& pdm)
{}

void Primitive::SetVertices([[maybe_unused]] const Vertices& vertices)
{}

void Primitive::SetColors([[maybe_unused]] const Colors& colors)
{}

void Primitive::SetIndices([[maybe_unused]] const Indices& indices)
{}

void Primitive::SetMatrix([[maybe_unused]] const Matrix& matrix)
{}

void Primitive::Draw()
{}

} // Dummy

} // Detail

} // Drawing
