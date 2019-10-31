#ifndef SET_CARD_IMAGE_ANIMATION_HPP
#define SET_CARD_IMAGE_ANIMATION_HPP
#include <cstdint>
#include "../animation.hpp"

namespace YGOpen
{

struct GraphicCard;
class CardTextureManager;

namespace Animation
{

class SetCardImage : public IAnimation
{
public:
	SetCardImage(GraphicCard& card, CardTextureManager& ctm);
	virtual ~SetCardImage() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	GraphicCard& card;
	CardTextureManager& ctm;
};

} // namespace Animation

} // namespace YGOpen

#endif // SET_CARD_IMAGE_ANIMATION_HPP
