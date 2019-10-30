#include "animation.hpp"

namespace YGOpen
{

IAnimation::IAnimation(float duration) : duration(duration)
{}

// float IAnimation::GetDuration() const
// {
// 	return duration;
// }

bool IAnimation::IsDone() const
{
	return progress >= duration;
}

}
