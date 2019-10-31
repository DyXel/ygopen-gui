#include "graphic_board.hpp"

#include <SDL_image.h>

#if defined(_DEBUG) || defined(DEBUG)
#define DEBUG_HITBOXES
#define DEBUG_MOUSE_POS
#endif // defined(_DEBUG) || defined(DEBUG)

// #undef DEBUG_HITBOXES
// #undef DEBUG_MOUSE_POS

#include "animator.hpp"
#include "card_texture_manager.hpp"
#include "constants.hpp"
#include "graphic_card.hpp"
#include "animations/move_card.hpp"
#include "animations/set_card_image.hpp"
#include "../board.hpp"
#include "../sdl_utility.hpp"
#include "../drawing/renderer.hpp"
#include "../drawing/primitive.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/quad.hpp"
#include "../gui/environment.hpp"

namespace YGOpen
{

struct Zone
{
	glm::mat4 model{1.0f};
	glm::vec3 loc{}; // Location
	glm::vec3 rot{}; // Rotation
	Drawing::Primitive prim;
	Drawing::Vertices hitbox;
#if defined(DEBUG_HITBOXES)
	Drawing::Primitive hitboxPrim;
#endif // defined(DEBUG_HITBOXES)
};

// TODO: ****************** ORGANIZE THIS ******************

bool PointInPoly(const glm::vec3& p, const Drawing::Vertices& v)
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

Drawing::Texture TextureFromPath(Drawing::Renderer ren, std::string_view path)
{
	auto tex = ren->NewTexture();
	SDL_Surface* image = IMG_Load(path.data());
	if((image != nullptr) &&
	   ((image = SDLU_SurfaceToRGBA32(image)) != nullptr))
	{
		tex->SetImage(image->w, image->h, image->pixels);
		SDL_FreeSurface(image);
	}
	return tex;
}

// *********************************************************

class CGraphicBoard::impl : protected DuelBoard<GraphicCard>
{
	friend CGraphicBoard;
	// ************************
	Drawing::Renderer renderer;
	Drawing::Texture cover;
	CardTextureManager ctm;
	// ************************ TODO: Move to own structure (perhaps `gfx`)
	std::map<LitePlace, glm::vec3> locations;
	std::map<LitePlace, Zone> zones;
	std::map<LitePlace, Zone>::iterator selectedZone;
	struct // Camera info
	{
		// Parent canvas, in pixels, represents the size of the drawable area.
		SDL_Rect pCan{};
		// Canvas, in pixels, where this graphic board is drawn.
		SDL_Rect can{};
		// Camera position on world coordinates
		glm::vec3 pos{};
		// View-Projection matrix.
		// to be combined with Viewport and/or Model matrix for each object
		glm::mat4 vp{1.0f};
	}cam;

	uint32_t targetState{};
	Animator ani;

	AnswerCallback acb; // TODO: find a better name

#if defined(DEBUG_MOUSE_POS)
	Drawing::Primitive mousePrim;
#endif // defined(DEBUG_MOUSE_POS)
	impl(Drawing::Renderer renderer, int flags) :
		renderer(renderer), ctm(renderer)
	{
		cover = TextureFromPath(renderer, "TEMP/cover.png");
		// zones textures
		auto zTex = TextureFromPath(renderer, "TEMP/zone.png");
		InitLocations(flags);
		for(const auto& kv : locations)
		{
			// Dont add hand locations
			if(LOC(kv.first) == LOCATION_HAND)
				continue;
			Zone& zone = zones.emplace(kv.first, Zone{}).first->second;
			zone.model = glm::translate(kv.second + glm::vec3(0.0f, 0.0f, -0.001f));
			zone.prim = renderer->NewPrimitive();
			zone.prim->SetDrawMode(Drawing::GetQuadDrawMode());
			zone.prim->SetVertices(ZONE_VERTICES);
			zone.prim->SetTexCoords(Drawing::GetQuadTexCoords());
			zone.prim->SetTexture(zTex);
			zone.hitbox.resize(ZONE_HITBOX_VERTICES.size());
#if defined(DEBUG_HITBOXES)
			zone.hitboxPrim = renderer->NewPrimitive();
			zone.hitboxPrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
			const glm::vec4 RED = {1.0f, 0.0f, 0.0f, 1.0f};
			zone.hitboxPrim->SetColors({RED, RED, RED, RED, RED});
#endif // defined(DEBUG_HITBOXES)
		}
		selectedZone = zones.end();
#if defined(DEBUG_MOUSE_POS)
		mousePrim = renderer->NewPrimitive();
		mousePrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
		mousePrim->SetVertices(MOUSE_POS_VERTICES);
		const glm::vec4 BLUE = {0.0f, 1.0f, 0.0f, 0.5f};
		mousePrim->SetColors({BLUE, BLUE, BLUE, BLUE});
#endif // defined(DEBUG_MOUSE_POS)
	}
	
	inline void InitLocations(int flags)
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
			// Shift zone locations
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
			// Shift zone locations
			LOCATION_Y(LOCATION_SZONE  , 5, -0.4f);
			LOCATION_Y(LOCATION_GRAVE  , 0, -0.4f);
			LOCATION_X(LOCATION_REMOVED, 0, -0.4f);
		}
		// Mirror all locations for player 1
		{
			// 180 Degrees rotation on Z-axis matrix
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
		// Insert Extra Monster Zones
		// NOTE: they are only inserted once for player 0.
		if(flags & DUEL_EMZONE)
			locations.insert(EMZ_LOCATIONS.begin(), EMZ_LOCATIONS.end());
#undef LOCATION_ERASE
#undef LOCATION_X
#undef LOCATION_Y
	}
	
	template<typename DoFunc>
	inline void ForAllCards(DoFunc Do)
	{
		for(uint8_t player = 0; player < 2; player++)
		{
#define X(name, enums) for(auto& card : name[player]) { Do(card); }
			DUEL_PILES();
#undef X
		}
		for(auto& kv : zoneCards)
			Do(kv.second);
	}
	
	void UpdateMatrices()
	{
		// Recalculate View-Projection matrix
		const auto aspectRatio = static_cast<float>(cam.can.w) / cam.can.h;
		const auto proj = glm::perspective(1.0f, aspectRatio, 0.1f, 10.0f);
		const auto view = glm::lookAt(cam.pos, {0.0f, 0.0f, 0.0f},
		                              {0.0f, 1.0f, 0.0f});
		cam.vp = proj * view;
		// Apply matrix to all elements
		for(auto& kv : zones)
			kv.second.prim->SetMatrix(cam.vp * kv.second.model);
		ForAllCards([this](GraphicCard& card)
		{
			const auto mvp = cam.vp * GetModel(card.loc, card.rot);
			card.front->SetMatrix(mvp);
			card.cover->SetMatrix(mvp);
		});
	}
	
	void UpdateHitboxes()
	{
		glm::vec3 sVec{};
		// NOTE: this works as long as can is smaller than pCan
		sVec.x = static_cast<float>(cam.can.w) / cam.pCan.w;
		sVec.y = static_cast<float>(cam.can.h) / cam.pCan.h;
		glm::vec3 tVec{};
		const float canvasOriginX = cam.can.x + cam.can.w * 0.5f;
		const float canvasOriginY = cam.can.y + cam.can.h * 0.5f;
		tVec.x = canvasOriginX / (cam.pCan.w * 0.5f) - 1.0f;
		tVec.y = canvasOriginY / (cam.pCan.h * 0.5f) - 1.0f;
		const glm::mat4 viewport = glm::scale(sVec) * glm::translate(tVec);
		const glm::mat4 vvp = viewport * cam.vp;
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
	
	void UpdateAllCards()
	{
		auto UpdatePos = [this](const Place& place, GraphicCard& card)
		{
			card.loc = GetLocXYZ(place);
			card.rot = GetRotXYZ(place, card.pos());
			const auto mvp = cam.vp * GetModel(card.loc, card.rot);
			card.front->SetMatrix(mvp);
			card.cover->SetMatrix(mvp);
		};
		Place place = {0, 0, 0, -1};
		for(uint8_t player = 0; player < 2; player++)
		{
			CON(place) = player;
			for(auto location : {LOCATION_DECK, LOCATION_HAND, LOCATION_GRAVE,
			                     LOCATION_REMOVED, LOCATION_EXTRA})
			{
				LOC(place) = location;
				int i = 0;
				for(auto& card : GetPile(CON(place), LOC(place)))
				{
					SEQ(place) = i;
					UpdatePos(place, card);
					i++;
				}
			}
		}
		for(auto& kv : zoneCards)
			UpdatePos(kv.first, kv.second);
	}
	
	void SetAnswerCallback(AnswerCallback answerCallback)
	{
		acb = answerCallback;
	}
	
	bool OnEvent(const SDL_Event& e)
	{
		if(e.type == SDL_MOUSEMOTION)
		{
			glm::vec3 mousePos{};
			// Convert mouse coordinates to NDC
			mousePos.x = e.motion.x / (cam.pCan.w * 0.5f) - 1.0f;
			mousePos.y = -(e.motion.y / (cam.pCan.h * 0.5f) - 1.0f);
#if defined(DEBUG_MOUSE_POS)
			mousePrim->SetMatrix(glm::translate(mousePos));
#endif // defined(DEBUG_MOUSE_POS)
			// TODO: zone deselection
			selectedZone = zones.end();
			for(auto it = zones.begin(); it != zones.end(); ++it)
			{
				if(PointInPoly(mousePos, it->second.hitbox))
				{
					selectedZone = it;
					return true;
				}
			}
		}
		else if(e.type == SDL_KEYDOWN && !e.key.repeat &&
		        e.key.keysym.scancode == SDL_SCANCODE_SPACE)
		{
			Core::Answer answer;
			answer.set_number(PHASE_END);
			acb(answer);
		}
		else if(e.type == SDL_KEYDOWN && !e.key.repeat &&
		        e.key.keysym.scancode == SDL_SCANCODE_END)
		{
			ani.FinishAll();
		}
		return false;
	}
	
	void Resize(const SDL_Rect& parent, const SDL_Rect& rect)
	{
		cam.pCan = parent;
		cam.can = rect;
		UpdateMatrices();
	}
	
	void Tick(float elapsed)
	{
		ani.Tick(elapsed);
		if(!ani.IsAnimating() && targetState != state)
		{
			if(targetState > state)
				Forward();
			else if (targetState < state)
				Backward();
			ani.Tick(elapsed);
		}
	}
	
	void Draw()
	{
		renderer->SetViewport(cam.can.x, cam.can.y, cam.can.w, cam.can.h);
		for(const auto& kv : zones)
			kv.second.prim->Draw();
		ForAllCards([](GraphicCard& card)
		{
			card.front->Draw();
			card.cover->Draw();
		});
#if defined(DEBUG_HITBOXES)
		renderer->SetViewport(cam.pCan.x, cam.pCan.y, cam.pCan.w, cam.pCan.h);
		for(const auto& kv : zones)
			kv.second.hitboxPrim->Draw();
#endif // defined(DEBUG_HITBOXES)
#if defined(DEBUG_MOUSE_POS)
		renderer->SetViewport(cam.pCan.x, cam.pCan.y, cam.pCan.w, cam.pCan.h);
		mousePrim->Draw();
#endif // defined(DEBUG_MOUSE_POS)
	}
	
	bool Forward()
	{
		if(!DuelBoard<GraphicCard>::Forward())
			return false;
		AnimateMsg(msgs[state - 1]);
		return true;
	}
	
	bool Backward()
	{
		if(!DuelBoard<GraphicCard>::Backward())
			return false;
		AnimateMsg(msgs[state]);
		return true;
	}
	
	void AnimateMsg(const Core::AnyMsg& msg)
	{
		if(msg.AnyMsg_case() == Core::AnyMsg::kSpecific)
		{
			const auto& specific = msg.specific();
			if(state != msgs.size() - 1u &&
			   specific.Specific_case() == Core::Specific::kRequest)
			{
				// Skip request message if it can't be answered
				return;
			}
			else if(specific.Specific_case() == Core::Specific::kInformation)
			{
				AnimateInfoMsg(specific.information());
			}
			else // (specific.Specific_case() == Core::Specific::kRequest)
			{
				AnimateRequestMsg(specific.request());
			}
		}
		else // (msg.AnyMsg_case() == Core::AnyMsg::kInformation)
		{
			AnimateInfoMsg(msg.information());
		}
	}
	
	void AnimateInfoMsg(const Core::Information& info)
	{
		switch(info.Information_case())
		{
#include "graphic_board_animate_info.inl"
		}
	}
	
	void AnimateRequestMsg(const Core::Request& request)
	{
		
	}
	
	const glm::vec3 GetHandLocXYZ(const Place& p, int count) const
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
	
	const glm::vec3 GetLocXYZ(const Place& p) const
	{
		glm::vec3 loc = {0.0f, 0.0f, 0.0f};
		if(LOC(p) & LOCATION_HAND)
		{
			loc = GetHandLocXYZ(p, hand[CON(p)].size());
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
				const auto offset = static_cast<float>(OSEQ(p)) *
				                    OVERLAY_OFFSET;
				if(CON(p) == 0)
					loc += OVERLAY_OFFSET + offset;
				else
					loc -= OVERLAY_OFFSET + offset;
			}
		}
		return loc;
	}
	
	const glm::vec3 GetRotXYZ(const Place& p, uint32_t pos) const
	{
		glm::vec3 rot = {0.0f, 0.0f, 0.0f};
		if(LOC(p) & LOCATION_HAND)
		{
			static const glm::vec3 UP = {0.0f, 1.0f, 0.0f};
			const LitePlace place = {CON(p), LOCATION_HAND, 0};
			const glm::vec3 loc = locations.at(place);
			if(pos & POS_FACEDOWN)
			{
				glm::mat4 lookAt = glm::lookAt({}, cam.pos - loc, UP);
				glm::extractEulerAngleXYZ(lookAt, rot.x, rot.y, rot.z);
			}
			else
			{
				glm::mat4 lookAt = glm::lookAt(cam.pos, loc, UP);
				glm::extractEulerAngleXYZ(lookAt, rot.x, rot.y, rot.z);
				rot.x = glm::abs(rot.x);
				rot.z = -rot.z;
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
			if(!IsPile(p) && (pos & POS_DEFENSE))
				rot.z += glm::radians(90.0f); // NOTE: Addition
		}
		return rot;
	}
	
	Drawing::Primitive NewCardFrontPrim()
	{
		auto prim = renderer->NewPrimitive();
		prim->SetDrawMode(Drawing::GetQuadDrawMode());
		prim->SetVertices(CARD_VERTICES);
		prim->SetTexCoords(Drawing::GetQuadTexCoords());
		prim->SetTexture(ctm.GetCardTextureByCode(0u));
		return prim;
	}
	
	Drawing::Primitive NewCardCoverPrim() const
	{
		auto prim = renderer->NewPrimitive();
		prim->SetDrawMode(Drawing::GetQuadDrawMode());
		prim->SetVertices(CARD_COVER_VERTICES);
		prim->SetTexCoords({{1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});
		prim->SetTexture(cover);
		return prim;
	}
};

// *********************************************************

CGraphicBoard::CGraphicBoard(GUI::Environment& env, Drawing::Renderer renderer, int flags) : 
	IElement(env), pimpl(new impl(renderer, flags))
{}

CGraphicBoard::~CGraphicBoard()
{}

void CGraphicBoard::SetCameraPosition(const glm::vec3& cPos)
{
	pimpl->cam.pos = cPos;
}

void CGraphicBoard::UpdateHitboxes()
{
	pimpl->UpdateHitboxes();
}

void CGraphicBoard::SetAnswerCallback(AnswerCallback answerCallback)
{
	pimpl->SetAnswerCallback(answerCallback);
}

void CGraphicBoard::Resize(const SDL_Rect& parent, const SDL_Rect& rect)
{
	pimpl->Resize(parent, rect);
}

void CGraphicBoard::Draw()
{
	pimpl->Draw();
}

void CGraphicBoard::AppendMsg(const Core::AnyMsg& msg)
{
	pimpl->AppendMsg(msg);
}

uint32_t CGraphicBoard::GetState() const
{
	return pimpl->state;
}

uint32_t CGraphicBoard::GetStatesCount() const
{
	return pimpl->msgs.size();
}

uint32_t CGraphicBoard::GetTargetState() const
{
	return pimpl->targetState;
}

bool CGraphicBoard::SetTargetState(uint32_t tState)
{
	if(tState >= pimpl->msgs.size())
		return false;
	pimpl->targetState = tState;
	return true;
}

void CGraphicBoard::FillPile(uint32_t controller, uint32_t location, int num)
{
	pimpl->FillPile(controller, location, num);
	auto& pile = pimpl->GetPile(controller, location);
	uint32_t seq = 0;
	for(auto& c : pile)
	{
		const Place p = {controller, location, seq, -1};
		c.front = pimpl->NewCardFrontPrim();
		c.cover = pimpl->NewCardCoverPrim();
		c.loc = pimpl->GetLocXYZ(p);
		c.rot = pimpl->GetRotXYZ(p, c.pos());
		seq++;
	}
}

void CGraphicBoard::Resize([[maybe_unused]] const glm::mat4& mat,
                           [[maybe_unused]] const SDL_Rect& rect)
{}

void CGraphicBoard::Tick()
{
	pimpl->Tick(env.GetTimeElapsed());
}

bool CGraphicBoard::OnEvent(const SDL_Event& e)
{
	return pimpl->OnEvent(e);
}

} // namespace YGOpen
