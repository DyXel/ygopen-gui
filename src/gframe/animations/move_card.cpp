#include "move_card.hpp"

#include <glm/gtx/compatibility.hpp>

#include "../graphic_card.hpp"
#include "../../drawing/primitive.hpp"

namespace YGOpen
{

namespace Animation
{

constexpr float DURATION = 0.250f;

static inline void UpdateCard(const glm::mat4& vp, const MoveCardData& card,
                              float a)
{
	card.gc.loc = glm::lerp(card.startLoc, card.endLoc, a);
	card.gc.rot = glm::lerp(card.startRot, card.endRot, a);
	const auto mvp = vp * GetModel(card.gc.loc, card.gc.rot);
	card.gc.front->SetMatrix(mvp);
	card.gc.cover->SetMatrix(mvp);
}

/*** MoveCard ***/

MoveCard::MoveCard(const glm::mat4& vp, const MoveCardData& card) :
	IAnimation(DURATION), vp(vp), card(card)
{}

float MoveCard::Tick(float elapsed)
{
	progress += elapsed;
	const float a = progress / duration;
	UpdateCard(vp, card, a);
	return Distance();
}

void MoveCard::Skip()
{
	progress = duration;
	Tick(0.0f);
}

/*** MoveCards ***/

MoveCards::MoveCards(const glm::mat4& vp,
                     const std::vector<MoveCardData>& cards)
	: IAnimation(DURATION), vp(vp), cards(cards)
{}

float MoveCards::Tick(float elapsed)
{
	progress += elapsed;
	const float a = progress / duration;
	for(auto& card : cards)
		UpdateCard(vp, card, a);
	return Distance();
}

void MoveCards::Skip()
{
	progress = duration;
	Tick(0.0f);
}

} // namespace Animation

} // namespace YGOpen
