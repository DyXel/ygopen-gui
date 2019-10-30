#ifndef GRAPHIC_BOARD_ANIMATION_HPP
#define GRAPHIC_BOARD_ANIMATION_HPP

namespace YGOpen
{

class IAnimation
{
public:
	IAnimation(float duration);
// 	float GetDuration() const;
	// When the animation is done this function will return true.
	virtual bool Tick(float elapsed) = 0;
	// Should call Tick with elapsed == duration
	// Needed to skip stuff such as playing audio.
	// Called by SkipAll
	virtual void Skip() = 0;
protected:
	const float duration;
	float progress{0.0f};
	
	bool IsDone() const;
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_ANIMATION_HPP
