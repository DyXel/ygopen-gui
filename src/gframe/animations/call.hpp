#ifndef CALL_ANIMATION_HPP
#define CALL_ANIMATION_HPP
#include <functional>

#include "../animation.hpp"

namespace YGOpen
{

namespace Animation
{

class Call : public IAnimation
{
public:
	Call(std::function<void()> f);
	virtual ~Call() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	std::function<void()> f;
};

} // namespace Animation

} // namespace YGOpen

#endif // CALL_ANIMATION_HPP
