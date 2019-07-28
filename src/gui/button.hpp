#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class Button : public IElement
{
public:
	Button();
	void Resize(const Drawing::Matrix& mat, const SDL_Rect& rect);
	void Draw();
// 	void OnSelect(bool selecting);
// 	void OnPress(bool releasing);
private:
	Drawing::Primitive shadow;
	Drawing::Primitive content;
	Drawing::Primitive lines;
};

}

}

#endif // GUI_BUTTON_HPP
