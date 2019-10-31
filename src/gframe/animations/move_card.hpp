#ifndef MOVE_CARD_ANIMATION_HPP
#define MOVE_CARD_ANIMATION_HPP
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "../animation.hpp"

namespace YGOpen
{

struct GraphicCard; // Forward declare

namespace Animation
{

struct MoveCardData
{
	GraphicCard& gc;
	const glm::vec3 startLoc, startRot, endLoc, endRot;
};

class MoveCard : public IAnimation
{
public:
	MoveCard(const glm::mat4& vp, const MoveCardData& card);
	virtual ~MoveCard() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	const glm::mat4& vp;
	const MoveCardData card;
};

class MoveCards : public IAnimation
{
public:
	MoveCards(const glm::mat4& vp, const std::vector<MoveCardData>& cards);
	virtual ~MoveCards() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	const glm::mat4& vp;
	const std::vector<MoveCardData> cards;
};


} // namespace Animation

} // namespace YGOpen

#endif // MOVE_CARD_ANIMATION_HPP
