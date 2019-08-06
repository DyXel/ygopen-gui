#ifndef GUI_ENVIRONMENT_HPP
#define GUI_ENVIRONMENT_HPP
#include <set>
#include <queue>
#include "common.hpp"
#include "element.hpp"

namespace YGOpen
{

namespace GUI
{

class Environment
{
public:
	Environment(const float& elapsed);
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
