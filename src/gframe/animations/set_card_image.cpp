#include "set_card_image.hpp"

#include "../card_texture_manager.hpp"
#include "../../drawing/primitive.hpp"

namespace YGOpen
{

namespace Animation
{

SetCardImage::SetCardImage(CardTextureManager& ctm, GraphicCard& card) :
	IAnimation(0.0f), ctm(ctm), card(card)
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

/*** MoveCards ***/

SetCardImages::SetCardImages(CardTextureManager& ctm, Container&& cards) :
	IAnimation(0.0f), ctm(ctm), cards(std::move(cards))
{}

float SetCardImages::Tick(float elapsed)
{
	for(auto& rcard : cards)
	{
		GraphicCard& card = rcard.get();
		card.front->SetTexture(ctm.GetCardTextureByCode(card.code()));
	}
	return elapsed;
}

void SetCardImages::Skip()
{
	Tick(0.0f);
}

} // namespace Animation

} // namespace YGOpen
