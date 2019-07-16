#ifndef DRAWING_DUMMY_PRIMITIVE_HPP
#define DRAWING_DUMMY_PRIMITIVE_HPP
#include <array>
#include "../primitive.hpp"
#include "../types.hpp"

namespace Drawing
{

namespace Detail
{

namespace Dummy
{

class Primitive : public IPrimitive
{
public:
	Primitive();
	virtual ~Primitive();
	
	void SetDrawMode(const PrimitiveDrawMode& pdm) override;
	void SetVertices(const Vertices& vertices) override;
	void SetColors(const Colors& colors) override;
// 	void SetTransform();
	
	void Draw() override;
};

} // Dummy

} // Detail

} // Drawing

#endif // DRAWING_DUMMY_PRIMITIVE_HPP
