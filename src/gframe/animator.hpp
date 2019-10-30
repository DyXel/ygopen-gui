#ifndef GRAPHIC_BOARD_ANIMATOR_HPP
#define GRAPHIC_BOARD_ANIMATOR_HPP
#include <memory>
#include <queue>

namespace YGOpen
{

class IAnimation;

class Animator
{
public:
	void Push(std::shared_ptr<IAnimation> animation);
	void Tick(float elapsed);
	void FinishAll(); // Finish all remaining animations instantaneously
	void SkipAll(); // Animations get King Crimson'd
	void SetSpeed(float spd);
	bool IsAnimating() const;
private:
	float speed{1.0f};
	std::queue<std::shared_ptr<IAnimation>> animations;
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_ANIMATOR_HPP
