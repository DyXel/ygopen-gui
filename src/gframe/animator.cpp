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
	if(!IsAnimating())
		return;
	if(animations.front()->Tick(elapsed * speed))
		animations.pop();
}

void Animator::SkipAll()
{
	while(IsAnimating())
	{
		animations.front()->Skip();
		animations.pop();
	}
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
