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
	
	void SetVertices(const Vertices& vertices);
	void SetColors(const Colors& colors);
// 	void SetTransform();
	
	void Draw();
};

} // Dummy

} // Detail

} // Drawing

#endif // DRAWING_DUMMY_PRIMITIVE_HPP
