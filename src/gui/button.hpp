#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP
#include <functional>
#include <string_view>
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

ELEMENT_DECLARATION(CButton)
{
public:
	ELEMENT_IMPLEMENT_New_METHOD(CButton)
	void Resize(const glm::mat4& mat, const SDL_Rect& rect) override;
	void Draw() override;
	using Callback = std::function<void(void)>;
	void SetCallback(Callback callback);
	void SetText(std::string_view txt);
protected:
	CButton(Environment& env);
	void Tick() override;
	void OnFocus(bool gained) override;
	bool OnEvent(const SDL_Event& e) override;
private:
	SDL_Rect r{};
	float brightness{1.0f};
	bool pressed{false};
	Callback cb;
	Drawing::Texture strTex;
	using DyPrimitive = std::pair<Drawing::Primitive, Drawing::Vertices>;
	DyPrimitive shadow;
	DyPrimitive content;
	DyPrimitive lines;
	DyPrimitive text;

	void SetBrightness(float b);
};

using Button = std::shared_ptr<CButton>;

}

}

#endif // GUI_BUTTON_HPP
