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

void Primitive::SetDrawMode(const PrimitiveDrawMode& pdm)
{}

void Primitive::SetVertices([[maybe_unused]] const Vertices& vertices)
{}

void Primitive::SetColors([[maybe_unused]] const Colors& colors)
{}

void Primitive::Draw()
{}

} // Dummy

} // Detail

} // Drawing
