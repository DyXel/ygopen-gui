#include "animator.hpp"

#include "animation.hpp"

namespace YGOpen
{

void Animator::Push(std::shared_ptr<IAnimation> animation)
{
	animations.push(animation);
}

void Animator::Tick(float elapsed)
{
	if(animations.empty())
		return;
	if(animations.front()->Tick(elapsed * speed))
	{
		animations.front()->Skip();
		animations.pop();
	}
}

void Animator::FinishAll()
{
	while(!animations.empty())
	{
		animations.front()->Skip();
		animations.pop();
	}
}

void Animator::SkipAll()
{
	animations = {};
}

void Animator::SetSpeed(float spd)
{
	speed = spd;
}

bool Animator::IsAnimating() const
{
	return !animations.empty();
}

} // namespace YGOpen
