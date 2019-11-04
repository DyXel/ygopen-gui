#ifndef GUI_ELEMENT_HPP
#define GUI_ELEMENT_HPP
#include "common.hpp"

namespace YGOpen
{

namespace GUI
{

class Environment;

class IElement
{
public:
	bool visible{true};
	virtual ~IElement() = default;
	virtual void Resize(const glm::mat4& mat, const SDL_Rect& rect) = 0;
	virtual void Draw() = 0;
protected:
	friend Environment;
	Environment& env;

	IElement(Environment& env) : env(env) {}

	virtual void Tick() {}

	// Controls both when gaining and losing focus
	virtual void OnFocus([[maybe_unused]] bool gained) {}

	// Handles any event. The event is not passed to subsequent elements
	// (managed by GUI::Environment) if the function returns true.
	virtual bool OnEvent([[maybe_unused]] const SDL_Event& e) = 0;
};

using Element = std::shared_ptr<IElement>;

#define ELEMENT_DECLARATION(type) \
	class type final : public GUI::IElement, \
	                   public std::enable_shared_from_this<type>
#define ELEMENT_IMPLEMENT_New_METHOD(type) \
	template<typename... Args> \
	static std::shared_ptr<type> New(Args&&... args) \
	{ \
		return std::shared_ptr<type>( \
			new type(std::forward<Args>(args)...) \
		); \
	}

} // GUI

} // YGOpen

#endif // GUI_ELEMENT_HPP
