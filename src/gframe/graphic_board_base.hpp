#ifndef GRAPHIC_BOARD_BASE_HPP
#define GRAPHIC_BOARD_BASE_HPP
#include <SDL.h>
#include <glm/vec3.hpp>
#include <client/board_base.hpp>

#include "../drawing/types.hpp"

#if defined(_DEBUG) || defined(DEBUG)
#define DEBUG_HITBOXES
#define DEBUG_MOUSE_POS
#endif // defined(_DEBUG) || defined(DEBUG)

// #undef DEBUG_HITBOXES
// #undef DEBUG_MOUSE_POS

#include "graphic_card.hpp"

namespace YGOpen
{

using LitePlace = std::tuple<uint32_t /*controller*/,
                             uint32_t /*location*/,
                             uint32_t /*sequence*/>;

struct Zone
{
	glm::mat4 model{1.0f};
	glm::vec3 loc{};
	glm::vec3 rot{};
	Drawing::Primitive prim;
	Drawing::Vertices hitbox;
#if defined(DEBUG_HITBOXES)
	Drawing::Primitive hitboxPrim;
#endif // defined(DEBUG_HITBOXES)
	
	Zone() = default;
	Zone(const Zone&) = delete;
	Zone(Zone&&) = default;
	Zone& operator=(const Zone&) = delete;
	Zone& operator=(Zone&&) = default;
};

class GraphicBoardBase : public virtual ClientBoardBase<GraphicCard>
{
public:
	GraphicBoardBase(Drawing::Renderer renderer);
	// Setup.
	void SetCardsTextures(Drawing::Texture u, Drawing::Texture c);
	void SetZonesTextures(Drawing::Texture face);
	void RebuildLocations(int flags); // Adds/removes zones/cards accordingly.
protected:
	struct // Camera info. // TODO: move to private
	{
		// Parent canvas, in pixels, represents the size of the drawable area.
		SDL_Rect pCan{};
		// Canvas, in pixels, where this graphic board is drawn.
		SDL_Rect can{};
		// Camera position in world coordinates.
		glm::vec3 pos{0.0f, -2.0f, 3.0f};
		// View-Projection matrix.
		// to be combined with Viewport and/or Model matrix for each object.
		glm::mat4 vp{1.0f};
		// Viewport matrix.
		// used to calculate hitboxes
		glm::mat4 v{1.0f};
	}cam;
	
	// Creates or updates the graphics of a card.
	void LazyCardGraphic(GraphicCard& card);
	// Creates or updates the hitbox of a card.
	void LazyCardHitbox(GraphicCard& card);
	
	void Resize(const SDL_Rect& parent, const SDL_Rect& rect);
	bool OnEvent(const SDL_Event& e);
	void Draw(); // TODO: remove on rework
	
	LitePlace const* SelectedZone() const;
	GraphicCard* SelectedCard() const;
	
	glm::vec3 GetHandLocXYZ(const Place& p, int count) const;
	glm::vec3 GetLocXYZ(const Place& p) const;
	glm::vec3 GetRotXYZ(const Place& p, uint32_t pos) const;
private:
	Drawing::Renderer renderer;
	Drawing::Texture cardUnknown, cardCover;
	Drawing::Texture zoneFace;
#if defined(DEBUG_MOUSE_POS)
	Drawing::Primitive mousePrim;
#endif // defined(DEBUG_MOUSE_POS)

	std::map<LitePlace, glm::vec3> locations;
	std::map<LitePlace, Zone> zones;
	
	LitePlace const* selectedZone{nullptr};
	GraphicCard* selectedCard{nullptr};
};

} // namespace YGOpen

#endif // GRAPHIC_BOARD_BASE_HPP
