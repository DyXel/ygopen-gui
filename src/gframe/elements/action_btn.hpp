#ifndef ACTION_BTN_HPP
#define ACTION_BTN_HPP
#include <functional>
#include "../../gui/element.hpp"

namespace YGOpen
{

namespace GUI
{

ELEMENT_DECLARATION(CActBtn)
{
public:
	ELEMENT_IMPLEMENT_New_METHOD(CActBtn)
	void Resize(const glm::mat4& mat, const SDL_Rect& rect) override;
	void Draw() override;
	using Callback = std::function<void(void)>;
	void SetCallback(Callback callback);
	void SetImage(Drawing::Texture tex);
protected:
	CActBtn(Environment& env);
	bool OnEvent(const SDL_Event& e) override;
private:
	SDL_Rect r{};
	bool pressed{false};
	Callback cb;
	using DyPrimitive = std::pair<Drawing::Primitive, Drawing::Vertices>;
	DyPrimitive content;
	DyPrimitive img;
};

using ActBtn = std::shared_ptr<CActBtn>;

} // namespace GUI

} // namespace YGOpen

#endif // ACTION_BTN_HPP
