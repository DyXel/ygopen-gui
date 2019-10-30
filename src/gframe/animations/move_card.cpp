#include "move_card.hpp"

#include <glm/gtx/compatibility.hpp>

#include "../../drawing/primitive.hpp"

namespace YGOpen
{

namespace Animation
{

constexpr float DURATION = 0.250f;

/*** MoveCard ***/

MoveCard::MoveCard(GraphicCard& card, const glm::mat4& vp,
                   const glm::vec3& startLoc, const glm::vec3& startRot,
                   const glm::vec3& endLoc, const glm::vec3& endRot) :
	IAnimation(DURATION), card(card), vp(vp),
	startLoc(startLoc), startRot(startRot),
	endLoc(endLoc), endRot(endRot)
{}

float MoveCard::Tick(float elapsed)
{
	progress += elapsed;
	const float a = progress / duration;
	card.loc = glm::lerp(startLoc, endLoc, a);
	card.rot = glm::lerp(startRot, endRot, a);
	const auto mvp = vp * GetModel(card.loc, card.rot);
	card.front->SetMatrix(mvp);
	card.cover->SetMatrix(mvp);
	return Distance();
}

void MoveCard::Skip()
{
	progress = duration;
	Tick(0.0f);
}

/*** MoveCards ***/

MoveCards::MoveCards(const glm::mat4& vp, const std::vector<CardToMove>& cards)
	: IAnimation(DURATION), vp(vp), cards(cards)
{}

float MoveCards::Tick(float elapsed)
{
	progress += elapsed;
	const float a = progress / duration;
	for(auto& card : cards)
	{
		card.gc.loc = glm::lerp(card.startLoc, card.endLoc, a);
		card.gc.rot = glm::lerp(card.startRot, card.endRot, a);
		const auto mvp = vp * GetModel(card.gc.loc, card.gc.rot);
		card.gc.front->SetMatrix(mvp);
		card.gc.cover->SetMatrix(mvp);
	}
	return Distance();
}

void MoveCards::Skip()
{
	progress = duration;
	Tick(0.0f);
}

} // namespace Animation

} // namespace YGOpen
