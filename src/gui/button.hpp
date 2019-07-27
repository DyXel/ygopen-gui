#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class Button : public IElement
{
	void Resize(const Drawing::Matrix& mat, const SDL_Rect& rect);
	void Draw();
};

}

}

#endif // GUI_BUTTON_HPP
