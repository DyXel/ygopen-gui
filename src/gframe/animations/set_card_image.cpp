#include "set_card_image.hpp"

#include "../graphic_card.hpp"
#include "../card_texture_manager.hpp"
#include "../../drawing/primitive.hpp"

namespace YGOpen
{

namespace Animation
{

SetCardImage::SetCardImage(GraphicCard& card, CardTextureManager& ctm) :
	IAnimation(0.0f), card(card), ctm(ctm)
{}

float SetCardImage::Tick(float elapsed)
{
	card.front->SetTexture(ctm.GetCardTextureByCode(card.code()));
	return elapsed;
}

void SetCardImage::Skip()
{
	Tick(0.0f);
}

} // namespace Animation

} // namespace YGOpen
