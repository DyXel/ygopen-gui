#include "environment.hpp"

#include <utility>

namespace YGOpen
{

namespace GUI
{

Environment::Environment(Drawing::Renderer renderer, TextSmith& font,
                         const float& elapsed) :
	renderer(renderer), font(font), elapsed(elapsed)
{}

void Environment::Add(Element ele)
{
	elements.insert(ele);
}

void Environment::Remove(Element ele)
{
	elements.erase(ele);
}

void Environment::AddToTickSet(Element ele)
{
	if(focused == ele || (elements.count(ele) != 0u))
		tickset.insert(ele);
}

void Environment::RemoveFromTickSet(Element ele)
{
	if(tickset.count(ele) != 0u)
		ticksetToRemove.push(ele);
}

void Environment::OnEvent(const SDL_Event& e)
{
	if(focused && focused->OnEvent(e))
		return;
	for(auto& ele : elements)
		if(ele->OnEvent(e))
			break;
}

void Environment::Tick()
{
	for(auto& ele : tickset)
		ele->Tick();
	while(!ticksetToRemove.empty())
	{
		tickset.erase(ticksetToRemove.front());
		ticksetToRemove.pop();
	}
}

void Environment::Draw()
{
	for(auto& ele : elements)
		ele->Draw();
	if(focused)
		focused->Draw();
}

bool Environment::Focus(Element ele)
{
	if(focused != ele && (elements.count(ele) != 0u))
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

float Environment::GetTimeElapsed() const
{
	return elapsed;
}

}  // namespace GUI

}  // namespace YGOpen
