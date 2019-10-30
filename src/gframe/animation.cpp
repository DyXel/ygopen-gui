#include "animation.hpp"

namespace YGOpen
{

IAnimation::IAnimation(float duration) : duration(duration)
{}

float IAnimation::Distance() const
{
	return progress - duration;
}

}
