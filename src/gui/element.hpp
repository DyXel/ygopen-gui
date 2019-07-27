#ifndef GUI_ELEMENT_HPP
#define GUI_ELEMENT_HPP
#include <SDL.h>

#include "../drawing/types.hpp"

namespace YGOpen
{

namespace GUI
{

class IElement
{
public:
	virtual void Resize(const Drawing::Matrix& mat, const SDL_Rect& rect) = 0;
	virtual void Tick() {};
	virtual void Draw() = 0;
	virtual void OnSelect(bool selecting) {};
};

}

}

#endif // GUI_ELEMENT_HPP
