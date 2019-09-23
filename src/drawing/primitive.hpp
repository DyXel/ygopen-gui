#ifndef DRAWING_PRIMITIVE_HPP
#define DRAWING_PRIMITIVE_HPP
#include "types.hpp"

namespace Drawing
{

namespace Detail
{

class IPrimitive
{
public:
	virtual void SetDrawMode(const PrimitiveDrawMode& pdm) = 0;
	virtual void SetVertices(const Vertices& vertices) = 0;
	virtual void SetColors(const Colors& colors) = 0;
	virtual void SetIndices(const Indices& indices) = 0;
	
	virtual void SetMatrix(const glm::mat4& matrix) = 0; // Normally Model Matrix
	virtual void SetBrightness(const float val) = 0; // Defaults to 1.0f
	
	virtual void SetTexCoords(const TexCoords& texCoords) = 0;
	virtual void SetTexture(const Texture& texture) = 0;
	
	virtual void Draw() = 0;
};

} // Detail

} // Drawing
#endif // DRAWING_PRIMITIVE_HPP
