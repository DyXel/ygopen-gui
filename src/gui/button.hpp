#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP
#include <functional>
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class CButton : public IElement
{
public:
	CButton(Environment& env);
	void Resize(const Drawing::Matrix& mat, const SDL_Rect& rect) override;
	void Draw() override;
	bool OnEvent(const SDL_Event& e) override;
	
	// Function to call when the button is pressed
	using Callback = std::function<void(void)>;
	void SetCallback(Callback callback);
private:
	SDL_Rect r;
	Callback cb;
	Drawing::Primitive shadow;
	Drawing::Primitive content;
	Drawing::Primitive lines;
};

using Button = std::shared_ptr<CButton>;

}

}

#endif // GUI_BUTTON_HPP
