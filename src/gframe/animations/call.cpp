#include "call.hpp"

namespace YGOpen
{

namespace Animation
{

Call::Call(std::function<void()> f) :
	IAnimation(0.0f), f(f)
{}

float Call::Tick(float elapsed)
{
	f();
	return elapsed;
}

void Call::Skip()
{
	Tick(0.0f);
}

} // namespace Animation

} // namespace YGOpen
