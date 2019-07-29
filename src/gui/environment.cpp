#include "environment.hpp"

namespace YGOpen
{

namespace GUI
{

void Environment::Add(const Element& ele)
{
	elements.insert(ele);
}

void Environment::Remove(const Element& ele)
{
	elements.erase(ele);
}

void Environment::AddToTickSet(const Element& ele)
{
	if(elements.count(ele))
		tickset.insert(ele);
}

void Environment::RemoveFromTickSet(const Element& ele)
{
	tickset.erase(ele);
}

void Environment::Tick()
{
	for(auto& ele : tickset)
		ele->Tick();
}

void Environment::Draw()
{
	for(auto& ele : elements)
		ele->Draw();
	if(focused)
		focused->Draw();
}

bool Environment::Focus(const Element& ele)
{
	if(focused != ele && elements.count(ele))
	{
		if(focused)
		{
			focused->OnFocus(false);
			elements.insert(focused);
		}
		focused = ele;
		elements.erase(ele);
		focused->OnFocus(true);
		return true;
	}
	return false;
}

void Environment::PropagateEvent(const SDL_Event& e)
{
	if(focused && focused->OnEvent(e))
		return;
	for(auto& ele : elements)
		if(ele->OnEvent(e))
			break;
}

} // GUI

} // YGOpen
