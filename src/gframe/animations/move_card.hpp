#ifndef MOVE_CARD_ANIMATION_HPP
#define MOVE_CARD_ANIMATION_HPP
#include <vector>
#include "../animation.hpp"
#include "../../gframe/graphic_card.hpp"

namespace YGOpen
{

namespace Animation
{

class MoveCard : public IAnimation
{
public:
	MoveCard(GraphicCard& card, const glm::mat4& vp,
	         const glm::vec3& startLoc, const glm::vec3& startRot,
	         const glm::vec3& endLoc, const glm::vec3& endRot);
	virtual ~MoveCard() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	GraphicCard& card;
	const glm::mat4& vp;
	const glm::vec3 startLoc, startRot, endLoc, endRot;
};

class MoveCards : public IAnimation
{
public:
	struct CardToMove
	{
		GraphicCard& gc;
		const glm::vec3 startLoc, startRot, endLoc, endRot;
	};
	MoveCards(const glm::mat4& vp, const std::vector<CardToMove>& cards);
	virtual ~MoveCards() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	const glm::mat4& vp;
	std::vector<CardToMove> cards;
};


} // namespace Animation

} // namespace YGOpen

#endif // MOVE_CARD_ANIMATION_HPP
