#ifndef SET_CARD_IMAGE_ANIMATION_HPP
#define SET_CARD_IMAGE_ANIMATION_HPP
#include <cstdint>
#include "../animation.hpp"
// Cannot forward declare due to SetCardImages
#include "../graphic_card.hpp"

namespace YGOpen
{

class CardTextureManager;

namespace Animation
{

class SetCardImage : public IAnimation
{
public:
	SetCardImage(CardTextureManager& ctm, GraphicCard& card);
	virtual ~SetCardImage() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	CardTextureManager& ctm;
	GraphicCard& card;
};

class SetCardImages : public IAnimation
{
public:
	using Container = std::vector<std::reference_wrapper<GraphicCard>>;
	SetCardImages(CardTextureManager& ctm, Container&& card);
	virtual ~SetCardImages() = default;
	float Tick(float elapsed) override;
	void Skip() override;
private:
	CardTextureManager& ctm;
	const Container cards;
};

} // namespace Animation

} // namespace YGOpen

#endif // SET_CARD_IMAGE_ANIMATION_HPP
