#include "graphic_board_base.hpp"

#include <enums/duel_mode.hpp>
#include <enums/location.hpp>
#include <enums/position.hpp>

#include "../drawing/renderer.hpp"
#include "../drawing/primitive.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/quad.hpp"

namespace YGOpen
{

static bool PointInPoly(const glm::vec3& p, const Drawing::Vertices& v)
{
	std::size_t count{};
	for(std::size_t i = 0u; i < v.size() - 1u; i++)
	{
		if(((v[i].y <= p.y) && (v[i + 1u].y > p.y)) ||
		   ((v[i].y > p.y) && (v[i + 1u].y <=  p.y)))
		{
			const float vt = (p.y - v[i].y) / (v[i + 1u].y - v[i].y);
			if(p.x < v[i].x + vt * (v[i + 1u].x - v[i].x))
				count++;
		}
	}
	return count & 1u;
}

const float CARD_THICKNESS = 0.0040f;
const glm::vec3 OVERLAY_OFFSET = {0.01f, -0.01f, -0.0001f};
const glm::vec3 UP = {0.0f, 0.0f, 1.0f};

static const Drawing::Vertices ZONE_HITBOX_VERTICES =
{
	{ -0.2f,  0.2f, 0.0f},
	{  0.2f,  0.2f, 0.0f},
	{  0.2f, -0.2f, 0.0f},
	{ -0.2f, -0.2f, 0.0f},
	{ -0.2f,  0.2f, 0.0f},
};

static const Drawing::Vertices CARD_HITBOX_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{ -0.1233f,  0.18f, 0.0f},
};

#if defined(DEBUG_MOUSE_POS)
static const Drawing::Vertices MOUSE_POS_VERTICES =
{
	{ -0.01f,  0.01f, 0.0f},
	{  0.01f,  0.01f, 0.0f},
	{  0.01f, -0.01f, 0.0f},
	{ -0.01f, -0.01f, 0.0f},
};
#endif // defined(DEBUG_MOUSE_POS)

static const Drawing::Vertices ZONE_VERTICES =
{
	{ -0.2f,  0.2f, 0.0f},
	{ -0.2f, -0.2f, 0.0f},
	{  0.2f,  0.2f, 0.0f},
	{  0.2f, -0.2f, 0.0f},
};

static const Drawing::Vertices CARD_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
};

static const Drawing::Vertices CARD_COVER_VERTICES =
{
	{ -0.1233f,  0.18f, 0.0f},
	{  0.1233f,  0.18f, 0.0f},
	{ -0.1233f, -0.18f, 0.0f},
	{  0.1233f, -0.18f, 0.0f},
};

static const std::map<LitePlace, glm::vec3> BASE_LOCATIONS =
{
	// Monster Zones
	{{0, LOCATION_MZONE  , 0}, {-0.8f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 1}, {-0.4f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 2}, { 0.0f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 3}, { 0.4f, -0.4f, 0.0f}},
	{{0, LOCATION_MZONE  , 4}, { 0.8f, -0.4f, 0.0f}},
	// Spell & Trap Zones
	{{0, LOCATION_SZONE  , 0}, {-0.8f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 1}, {-0.4f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 2}, { 0.0f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 3}, { 0.4f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 4}, { 0.8f, -0.8f, 0.0f}},
	{{0, LOCATION_SZONE  , 5}, {-1.2f, -0.2f, 0.0f}}, // Field Zone
	// Pendulum Zones
	{{0, LOCATION_PZONE  , 0}, {-1.2f, -0.6f, 0.0f}},
	{{0, LOCATION_PZONE  , 1}, { 1.2f, -0.6f, 0.0f}},
	// Piles
	{{0, LOCATION_DECK   , 0}, { 1.2f, -1.0f, 0.0f}},
	{{0, LOCATION_EXTRA  , 0}, {-1.2f, -1.0f, 0.0f}},
	{{0, LOCATION_GRAVE  , 0}, { 1.2f, -0.2f, 0.0f}},
	{{0, LOCATION_REMOVED, 0}, { 1.6f, -0.2f, 0.0f}},
	{{0, LOCATION_HAND   , 0}, { 0.0f, -1.2f, 0.2f}},
};

static const std::map<LitePlace, glm::vec3> EMZ_LOCATIONS =
{
	// Extra Monster Zones
	{{0, LOCATION_MZONE  , 5}, {-0.4f,  0.0f, 0.0f}},
	{{0, LOCATION_MZONE  , 6}, { 0.4f,  0.0f, 0.0f}},
};

// public

GraphicBoardBase::GraphicBoardBase(Drawing::Renderer renderer) :
	renderer(renderer)
{}

void GraphicBoardBase::SetCardsTextures(Drawing::Texture u, Drawing::Texture c)
{
	cardUnknown = u;
	cardCover = c;
}

void GraphicBoardBase::SetZonesTextures(Drawing::Texture face)
{
	zoneFace = face;
}

void GraphicBoardBase::RebuildLocations(int flags)
{
#define LOCATION_ERASE(loc, seq) locations.erase({0, loc, seq})
#define LOCATION_X(loc, seq, am) locations[{0, loc, seq}].x += am
#define LOCATION_Y(loc, seq, am) locations[{0, loc, seq}].y += am
	locations = BASE_LOCATIONS;
	if(flags & DUEL_SPEED)
	{
		LOCATION_ERASE(LOCATION_MZONE, 0);
		LOCATION_ERASE(LOCATION_MZONE, 4);
		LOCATION_ERASE(LOCATION_SZONE, 0);
		LOCATION_ERASE(LOCATION_SZONE, 4);
		// Shift zone locations.
		LOCATION_X(LOCATION_SZONE  , 5,  0.4f);
		LOCATION_X(LOCATION_PZONE  , 0,  0.4f);
		LOCATION_X(LOCATION_PZONE  , 1, -0.4f);
		LOCATION_X(LOCATION_DECK   , 0, -0.4f);
		LOCATION_X(LOCATION_EXTRA  , 0,  0.4f);
		LOCATION_X(LOCATION_GRAVE  , 0, -0.4f);
		LOCATION_X(LOCATION_REMOVED, 0, -0.4f);
	}
	if(!(flags & DUEL_PZONE) || !(flags & DUEL_SEPARATE_PZONE))
	{
		LOCATION_ERASE(LOCATION_PZONE, 0);
		LOCATION_ERASE(LOCATION_PZONE, 1);
		// Shift zone locations.
		LOCATION_Y(LOCATION_SZONE  , 5, -0.4f);
		LOCATION_Y(LOCATION_GRAVE  , 0, -0.4f);
		LOCATION_X(LOCATION_REMOVED, 0, -0.4f);
	}
	// Mirror all locations for player 1.
	{
		// 180 Degrees rotation on Z-axis matrix.
		static const glm::mat4 ROT_180_Z = glm::rotate(glm::radians(180.0f),
		                                   glm::vec3(0.0f, 0.0f, 1.0f));
		std::map<LitePlace, glm::vec3> p1locations;
		for(const auto& kv : locations)
		{
			std::pair<LitePlace, glm::vec3> kvc = kv;
			CON(kvc.first) = 1;
			kvc.second = glm::vec3(ROT_180_Z * glm::vec4(kvc.second, 1.0f));
			p1locations.emplace(std::move(kvc));
		}
		locations.insert(p1locations.begin(), p1locations.end());
	}
	// Insert Extra Monster Zones.
	// NOTE: they are only inserted once for player 0.
	if(flags & DUEL_EMZONE)
		locations.insert(EMZ_LOCATIONS.begin(), EMZ_LOCATIONS.end());
#undef LOCATION_ERASE
#undef LOCATION_X
#undef LOCATION_Y
	zones.clear();
	for(const auto& kv : locations)
	{
		// Dont add hand locations
		if(LOC(kv.first) == LOCATION_HAND)
			continue;
		Zone& zone = zones.emplace(kv.first, Zone{}).first->second;
		zone.model = glm::translate(kv.second + glm::vec3(0.0f, 0.0f, -0.001f));
		zone.prim = renderer->NewPrimitive();
		zone.prim->depthTest = true;
		zone.prim->SetDrawMode(Drawing::GetQuadDrawMode());
		zone.prim->SetVertices(ZONE_VERTICES);
		zone.prim->SetTexCoords(Drawing::GetQuadTexCoords());
		zone.prim->SetTexture(zoneFace);
		zone.hitbox.resize(ZONE_HITBOX_VERTICES.size());
#if defined(DEBUG_HITBOXES)
		zone.hitboxPrim = renderer->NewPrimitive();
		zone.hitboxPrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
		const glm::vec4 RED = {1.0f, 0.0f, 0.0f, 1.0f};
		zone.hitboxPrim->SetColors({RED, RED, RED, RED, RED});
#endif // defined(DEBUG_HITBOXES)
	}
#if defined(DEBUG_MOUSE_POS)
	mousePrim = renderer->NewPrimitive();
	mousePrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
	mousePrim->SetVertices(MOUSE_POS_VERTICES);
	const glm::vec4 BLUE = {0.0f, 1.0f, 0.0f, 0.5f};
	mousePrim->SetColors({BLUE, BLUE, BLUE, BLUE});
#endif // defined(DEBUG_MOUSE_POS)
}

// protected

void GraphicBoardBase::LazyCardGraphic(GraphicCard& card)
{
	// Front face (Card picture).
	if(!card.front)
	{
		card.front = renderer->NewPrimitive();
		card.front->depthTest = true;
		card.front->SetDrawMode(Drawing::GetQuadDrawMode());
		card.front->SetVertices(CARD_VERTICES);
		card.front->SetTexCoords(Drawing::GetQuadTexCoords());
		card.front->SetTexture(cardUnknown); // Only apply once for creation.
	}
	// Back face (Cover image).
	if(!card.cover)
	{
		card.cover = renderer->NewPrimitive();
		card.cover->depthTest = true;
		card.cover->SetDrawMode(Drawing::GetQuadDrawMode());
		card.cover->SetVertices(CARD_COVER_VERTICES);
		card.cover->SetTexCoords({{1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});
	}
	card.cover->SetTexture(cardCover);
}

void GraphicBoardBase::LazyCardHitbox(GraphicCard& card)
{
	if(!card.hitbox)
		card.hitbox = std::make_unique<GraphicCard::HitboxData>();
	card.hitbox->vertices = CARD_HITBOX_VERTICES;
#if defined(DEBUG_HITBOXES)
	card.hitbox->prim = renderer->NewPrimitive();
	card.hitbox->prim->SetDrawMode(Drawing::PDM_LINE_LOOP);
	const glm::vec4 BLU = {0.0f, 0.0f, 1.0f, 1.0f};
	card.hitbox->prim->SetColors({BLU, BLU, BLU, BLU, BLU});
#endif // defined(DEBUG_HITBOXES)
	const glm::mat4 mvp = cam.v * cam.vp * GetModel(card.loc, card.rot);
	for(size_t i = 0; i < CARD_HITBOX_VERTICES.size(); i++)
	{
		const glm::vec4 v4 = mvp * glm::vec4(CARD_HITBOX_VERTICES[i], 1.0f);
		card.hitbox->vertices[i] = glm::vec3(v4) / v4.w;
	}
#if defined(DEBUG_HITBOXES)
	card.hitbox->prim->SetVertices(card.hitbox->vertices);
#endif // defined(DEBUG_HITBOXES)
}

void GraphicBoardBase::Resize(const SDL_Rect& parent, const SDL_Rect& rect)
{
	cam.pCan = parent;
	cam.can = rect;
	// Recalculate View-Projection matrix.
	const auto aspectRatio = static_cast<float>(cam.can.w) / cam.can.h;
	const glm::mat4 proj = glm::perspective(1.0f, aspectRatio, 0.1f, 10.0f);
	const glm::mat4 view = glm::lookAt(cam.pos, {0.0f, 0.0f, 0.0f}, UP);
	cam.vp = proj * view;
	// Recalculate Viewport matrix.
	glm::vec3 sVec{}, tVec{};
	// NOTE: this works as long as `can` is smaller than `pCan`.
	sVec.x = static_cast<float>(cam.can.w) / cam.pCan.w;
	sVec.y = static_cast<float>(cam.can.h) / cam.pCan.h;
	const float canvasOriginX = cam.can.x + cam.can.w * 0.5f;
	const float canvasOriginY = cam.can.y + cam.can.h * 0.5f;
	tVec.x = canvasOriginX / (cam.pCan.w * 0.5f) - 1.0f;
	tVec.y = canvasOriginY / (cam.pCan.h * 0.5f) - 1.0f;
	cam.v = glm::scale(sVec) * glm::translate(tVec);
	// Refresh objects MVP Matrix.
	for(auto& kv : zones)
		kv.second.prim->SetMatrix(cam.vp * kv.second.model);
	for(uint8_t player = 0; player < 2; player++)
	{
		for(const auto location : {LOCATION_DECK, LOCATION_HAND, LOCATION_GRAVE,
		                           LOCATION_REMOVED, LOCATION_EXTRA})
		{
			for(auto& card : GetPile(player, location))
			{
				const auto mvp = cam.vp * GetModel(card.loc, card.rot);
				card.front->SetMatrix(mvp);
				card.cover->SetMatrix(mvp);
			}
		}
	}
	for(auto& kv : ZoneCards())
	{
		auto& card = kv.second;
		const auto mvp = cam.vp * GetModel(card.loc, card.rot);
		card.front->SetMatrix(mvp);
		card.cover->SetMatrix(mvp);
	}
	// Recalculate zones hitbox.
	const glm::mat4 vvp = cam.v * cam.vp;
	for(auto& kv : zones)
	{
		const glm::mat4 mvp = vvp * kv.second.model;
		for(size_t i = 0; i < ZONE_HITBOX_VERTICES.size(); i++)
		{
			const glm::vec4 v4 = mvp * glm::vec4(ZONE_HITBOX_VERTICES[i], 1.0f);
			kv.second.hitbox[i] = glm::vec3(v4) / v4.w;
		}
#if defined(DEBUG_HITBOXES)
		kv.second.hitboxPrim->SetVertices(kv.second.hitbox);
#endif // defined(_DEBUG)
	}
}

bool GraphicBoardBase::OnEvent(const SDL_Event& e)
{
	if(e.type != SDL_MOUSEMOTION)
		return false;
	const glm::vec3 mPos =
	{
		e.motion.x / (cam.pCan.w * 0.5f) - 1.0f,
		-(e.motion.y / (cam.pCan.h * 0.5f) - 1.0f),
		0.0f
	};
#if defined(DEBUG_MOUSE_POS)
	mousePrim->SetMatrix(glm::translate(mPos));
#endif // defined(DEBUG_MOUSE_POS)
	// Try checking for cards.
	if(selectedCard)
	{
		if(PointInPoly(mPos, selectedCard->hitbox->vertices))
		{
			return true;
		}
		else
		{
			// TODO: card deselection animation
			selectedCard = nullptr;
		}
	}
	for(auto& card : GetPile(0, LOCATION_HAND))
	{
		if(card.hitbox && PointInPoly(mPos, card.hitbox->vertices))
		{
			selectedCard = &card;
			// TODO: card selection and/or hover animation
			return true;
		}
	}
	for(auto& card : GetPile(1, LOCATION_HAND))
	{
		if(card.hitbox && PointInPoly(mPos, card.hitbox->vertices))
		{
			selectedCard = &card;
			// TODO: card selection and/or hover animation
			return true;
		}
	}
	// Try checking for zones.
	if(selectedZone)
	{
		if(PointInPoly(mPos, zones[*selectedZone].hitbox))
		{
			return true;
		}
		else
		{
			// TODO: zone deselection animation
			selectedZone = nullptr;
		}
	}
	for(const auto& kv : zones)
	{
		if(PointInPoly(mPos, kv.second.hitbox))
		{
			selectedZone = &kv.first;
			// TODO: zone selection animation
			return true;
		}
	}
	return false;
}

void GraphicBoardBase::Draw()
{
	renderer->SetViewport(cam.can.x, cam.can.y, cam.can.w, cam.can.h);
	for(const auto& kv : zones)
		kv.second.prim->Draw();
	for(uint8_t player = 0; player < 2; player++)
	{
		for(const auto location : {LOCATION_DECK, LOCATION_HAND, LOCATION_GRAVE,
		                           LOCATION_REMOVED, LOCATION_EXTRA})
		{
			for(auto& card : GetPile(player, location))
			{
				card.front->Draw();
				card.cover->Draw();
			}
		}
	}
	for(auto& kv : ZoneCards())
	{
		kv.second.front->Draw();
		kv.second.cover->Draw();
	}
	renderer->SetViewport(cam.pCan.x, cam.pCan.y, cam.pCan.w, cam.pCan.h);
#if defined(DEBUG_HITBOXES)
	for(const auto& kv : zones)
		kv.second.hitboxPrim->Draw();
	for(const auto& card : GetPile(0, LOCATION_HAND))
		if(card.hitbox)
			card.hitbox->prim->Draw();
	for(const auto& card : GetPile(1, LOCATION_HAND))
		if(card.hitbox)
			card.hitbox->prim->Draw();
#endif // defined(DEBUG_HITBOXES)
#if defined(DEBUG_MOUSE_POS)
	mousePrim->Draw();
#endif // defined(DEBUG_MOUSE_POS)
}

LitePlace const* GraphicBoardBase::SelectedZone() const
{
	return selectedZone;
}

GraphicCard* GraphicBoardBase::SelectedCard() const
{
	return selectedCard;
}

glm::vec3 GraphicBoardBase::GetHandLocXYZ(const Place& p, int count) const
{
	glm::vec3 loc = {0.0f, 0.0f, 0.0f};
	const float cardWidth = glm::abs(CARD_VERTICES[0].x) * 2.0f;
	// Card centered sequence
	const float cenSeq = SEQ(p) - glm::floor(count * 0.5f) +
	                     (!(count & 1) * 0.5f);
	// Translation vector, flipped if player is 1
	const auto tVec = glm::vec3(cardWidth, 0.0f, 0.0f) *
	                  ((CON(p)) ? -1.0f : 1.0f);
	const glm::vec3 offset = tVec * cenSeq;
	const auto search = locations.find({CON(p), LOCATION_HAND, 0});
	if(search != locations.end())
		loc = search->second + offset;
	return loc;
}

glm::vec3 GraphicBoardBase::GetLocXYZ(const Place& p) const
{
	glm::vec3 loc = {0.0f, 0.0f, 0.0f};
	if(LOC(p) & LOCATION_HAND)
	{
		loc = GetHandLocXYZ(p, GetPile(CON(p), LOC(p)).size());
	}
	else if(IsPile(p))
	{
		const auto offset = static_cast<float>(SEQ(p)) *
		                    glm::vec3(0.0f, 0.0f, CARD_THICKNESS);
		LitePlace lp = {CON(p), LOC(p), 0};
		const auto search = locations.find(lp);
		if(search != locations.end())
			loc = search->second + offset;
	}
	else
	{
		LitePlace lp;
		// Remove overlay for searching purposes
		LOC(lp) = LOC(p) & (~LOCATION_OVERLAY);
		// If its extra monster zone
		if((LOC(p) & LOCATION_MZONE) && SEQ(p) > 4)
		{
			CON(lp) = 0;
			// Flip sequence to get properly oriented zones
			SEQ(lp) = (CON(p) == 1) ? 11 - SEQ(p) : SEQ(p);
		}
		else
		{
			CON(lp) = CON(p);
			SEQ(lp) = SEQ(p);
		}
		const auto search = locations.find(lp);
		if(search != locations.end())
			loc = search->second;
		if(LOC(p) & LOCATION_OVERLAY)
		{
			const auto offset = static_cast<float>(OSEQ(p)) * OVERLAY_OFFSET;
			if(CON(p) == 0)
				loc += OVERLAY_OFFSET + offset;
			else
				loc -= OVERLAY_OFFSET + offset;
		}
	}
	return loc;
}

glm::vec3 GraphicBoardBase::GetRotXYZ(const Place& p, uint32_t pos) const
{
	glm::vec3 rot = {0.0f, 0.0f, 0.0f};
	if(LOC(p) & LOCATION_HAND)
	{
		const LitePlace place = {CON(p), LOCATION_HAND, 0};
		const glm::vec3 loc = locations.at(place);
		if(pos & POS_FACEDOWN)
		{
			glm::mat4 lookAt = glm::lookAt({}, cam.pos - loc, UP);
			glm::extractEulerAngleXYZ(lookAt, rot.x, rot.y, rot.z);
		}
		else
		{
			glm::mat4 lookAt = glm::lookAt(loc, cam.pos, UP);
			glm::extractEulerAngleXYZ(lookAt, rot.x, rot.y, rot.z);
			rot.y += glm::radians(-180.0f);
		}
	}
	else if(IsPile(p))
	{
		if(CON(p) == 1)
			rot.z = glm::radians(180.0f);
		if(pos & POS_FACEDOWN)
			rot.y = glm::radians(180.0f);
	}
	else
	{
		if(CON(p) == 1)
			rot.z = glm::radians(180.0f);
		if(!(LOC(p) & LOCATION_OVERLAY) && (pos & POS_FACEDOWN))
			rot.y = glm::radians(180.0f);
		if(pos & POS_DEFENSE && !(LOC(p) & LOCATION_SZONE))
			rot.z -= glm::radians(90.0f); // NOTE: Substraction
	}
	return rot;
}

// private



} // namespace YGOpen
