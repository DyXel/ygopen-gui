#ifndef GRAPHIC_BOARD_CARD_HPP
#define GRAPHIC_BOARD_CARD_HPP
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
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
	struct HitboxData
	{
		Drawing::Vertices vertices;
#if defined(DEBUG_HITBOXES)
		Drawing::Primitive prim;
#endif // defined(DEBUG_HITBOXES)
	};
	// Information used for selections and actions
	struct ActionData
	{
		std::map<Core::CardSelectionType, int> ts;
// 		int weight{0};
	};
	std::unique_ptr<HitboxData> hitbox;
	std::unique_ptr<ActionData> action;
	
	GraphicCard() = default;
	GraphicCard(const GraphicCard&) = delete;
	GraphicCard(GraphicCard&&) = default;
	GraphicCard& operator=(const GraphicCard&) = delete;
	GraphicCard& operator=(GraphicCard&&) = default;
};

inline const glm::mat4 GetModel(const glm::vec3& loc, const glm::vec3& rot)
{
	return glm::translate(loc) * glm::eulerAngleXYZ(rot.x, rot.y, rot.z);
}

} // namespace YGOpen

#endif // GRAPHIC_BOARD_CARD_HPP
