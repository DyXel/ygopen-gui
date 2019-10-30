#ifndef GRAPHIC_BOARD_ANIMATION_HPP
#define GRAPHIC_BOARD_ANIMATION_HPP

namespace YGOpen
{

class IAnimation
{
public:
	IAnimation(float duration);
	// Returns the time distance needed to finish the animation (negative)
	// or surplus time when the animation is finished (positive)
	virtual float Tick(float elapsed) = 0;
	// Should call Tick with elapsed == duration
	// Needed to skip stuff such as playing audio.
	// Called by SkipAll
	virtual void Skip() = 0;
protected:
	const float duration;
	float progress{0.0f};
	
	float Distance() const;
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_ANIMATION_HPP
