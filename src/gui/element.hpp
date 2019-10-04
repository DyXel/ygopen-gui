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
	virtual ~IElement() = default;
	virtual void Resize(const glm::mat4& mat, const SDL_Rect& rect) = 0;
	virtual void Draw() = 0;
protected:
	IElement(Environment& env) : env(env) {}
	friend Environment;
	Environment& env;

	virtual void Tick() {}

	// Controls both when gaining and losing focus
	virtual void OnFocus([[maybe_unused]] bool gained) {}

	// Handles any event. The event is not passed to subsequent elements
	// (managed by GUI::Environment) if the function returns true.
	virtual bool OnEvent([[maybe_unused]] const SDL_Event& e) = 0;
};

using Element = std::shared_ptr<IElement>;

} // GUI

} // YGOpen

#endif // GUI_ELEMENT_HPP
