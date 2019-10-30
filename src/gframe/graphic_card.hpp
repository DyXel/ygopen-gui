#ifndef GRAPHIC_BOARD_CARD_HPP
#define GRAPHIC_BOARD_CARD_HPP
#include <glm/vec3.hpp>
#include "../card.hpp"
#include "../drawing/types.hpp"

#include "core_msg_answer.pb.h"

namespace YGOpen
{

struct GraphicCard : public Card
{
	glm::vec3 loc{}; // Location
	glm::vec3 rot{}; // Rotation
	Drawing::Primitive front;
	Drawing::Primitive cover;
	struct CSelectInfo
	{
		std::map<Core::CardSelectionType, int> ts;
		Drawing::Vertices hitbox;
	};
	std::unique_ptr<CSelectInfo> cselectInfo;
	
	GraphicCard() = default;
	GraphicCard(const GraphicCard&) = delete;
	GraphicCard(GraphicCard&&) = default;
	GraphicCard& operator=(const GraphicCard&) = delete;
	GraphicCard& operator=(GraphicCard&&) = default;
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_CARD_HPP
