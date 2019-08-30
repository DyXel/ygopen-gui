#ifndef GUI_BUTTON_HPP
#define GUI_BUTTON_HPP
#include <functional>
#include <string_view>
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class CButton;

using Button = std::shared_ptr<CButton>;

class CButton : public IElement
{
public:
	template<typename... Args>
	static Button New(Args&&... args)
	{
		return Button(new CButton(std::forward<Args>(args)...));
	}
	void Resize(const Drawing::Matrix& mat, const SDL_Rect& rect) override;
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
	Callback cb;
	float brightness{1.0f};
	Drawing::Primitive shadow;
	Drawing::Primitive content;
	Drawing::Primitive lines;
	Drawing::Primitive text;
	Drawing::Vertices shadowVertices;
	Drawing::Vertices contentVertices;
	Drawing::Vertices linesVertices;
	Drawing::Vertices textVertices;
	int txtWidth{0};
	int txtHeight{0};
};

}

}

#endif // GUI_BUTTON_HPP
