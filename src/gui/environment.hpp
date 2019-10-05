#ifndef GUI_ENVIRONMENT_HPP
#define GUI_ENVIRONMENT_HPP
#include <set>
#include <queue>
#include "common.hpp"
#include "element.hpp"

#include "../text_smith.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{

namespace GUI
{

class Environment
{
public:
	Drawing::Renderer renderer;
	TextSmith& font;

	Environment(Drawing::Renderer renderer, TextSmith& font,
	            const float& elapsed);
	void Add(Element ele);
	void Remove(Element ele);
	
	void AddToTickSet(Element ele);
	void RemoveFromTickSet(Element ele);
	
	// Propagate a event to the managed elements. Starting always from the
	// focused element.
	void OnEvent(const SDL_Event& e);
	void Tick();
	void Draw();
	
	// Focus a element (and unfocus previous element, if it exists)
	// returns false if the element was not focused.
	bool Focus(Element ele);
	
	float GetTimeElapsed() const;
private:
	std::set<Element> elements;
	std::set<Element> tickset;
	std::queue<Element> ticksetToRemove;
	Element focused;
	const float& elapsed;
};

} // GUI

} // YGOpen


#endif // GUI_ENVIRONMENT_HPP
