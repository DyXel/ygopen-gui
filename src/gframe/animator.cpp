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
	float distance = animations.front()->Tick(elapsed * speed);
	while(distance > 0.0f)
	{
		animations.front()->Skip();
		animations.pop();
		if(!animations.empty())
			distance = animations.front()->Tick(distance / speed);
		else
			break;
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
