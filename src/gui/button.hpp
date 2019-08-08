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
	
	using Callback = std::function<void(void)>;
	void SetCallback(Callback callback);
	void SetText(std::string_view desired);
protected:
	void Tick() override;
	void OnFocus(bool gained) override;
	bool OnEvent(const SDL_Event& e) override;
private:
	SDL_Rect r;
	Callback cb;
	float brightness{1.0f};
	Drawing::Primitive shadow;
	Drawing::Primitive content;
	Drawing::Primitive lines;
	Drawing::Primitive text;
	int txtWidth{0};
	int txtHeight{0};
};

using Button = std::shared_ptr<CButton>;

}

}

#endif // GUI_BUTTON_HPP
