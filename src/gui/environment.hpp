#ifndef GUI_ENVIRONMENT_HPP
#define GUI_ENVIRONMENT_HPP
#include <set>
#include "common.hpp"
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class Environment
{
public:
	void Add(const Element& ele);
	void Remove(const Element& ele);
	
	void AddToTickSet(const Element& ele);
	void RemoveFromTickSet(const Element& ele);
	
	void Tick();
	void Draw();
	
	// Focus a element (and unfocus previous element, if it exists)
	// returns false if the element was not focused.
	bool Focus(const Element& ele);
	
	// Propagate a event to the managed elements. Starting always from the
	// focused element.
	void PropagateEvent(const SDL_Event& e);
private:
	std::set<Element> elements;
	std::set<Element> tickset;
	Element focused;
};

} // GUI

} // YGOpen


#endif // GUI_ENVIRONMENT_HPP
