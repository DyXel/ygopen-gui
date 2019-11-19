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
#include "animations/call.hpp"
#include "animations/move_card.hpp"
#include "animations/set_card_image.hpp"
#include "elements/action_btn.hpp"
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
	
	Zone() = default;
	Zone(const Zone&) = delete;
	Zone(Zone&&) = default;
	Zone& operator=(const Zone&) = delete;
	Zone& operator=(Zone&&) = default;
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

class CGraphicBoard final : public IGraphicBoard, public DuelBoard<GraphicCard>
{
public:
	CGraphicBoard(GUI::Environment& env, int flags)
		: IGraphicBoard(env), ctm(env.renderer)
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
		cover = TextureFromPath(env.renderer, "TEMP/cover.png");
		// zones textures
		auto zTex = TextureFromPath(env.renderer, "TEMP/zone.png");
		for(const auto& kv : locations)
		{
			// Dont add hand locations
			if(LOC(kv.first) == LOCATION_HAND)
				continue;
			Zone& zone = zones.emplace(kv.first, Zone{}).first->second;
			zone.model = glm::translate(kv.second + glm::vec3(0.0f, 0.0f, -0.001f));
			zone.prim = env.renderer->NewPrimitive();
			zone.prim->depthTest = true;
			zone.prim->SetDrawMode(Drawing::GetQuadDrawMode());
			zone.prim->SetVertices(ZONE_VERTICES);
			zone.prim->SetTexCoords(Drawing::GetQuadTexCoords());
			zone.prim->SetTexture(zTex);
			zone.hitbox.resize(ZONE_HITBOX_VERTICES.size());
#if defined(DEBUG_HITBOXES)
			zone.hitboxPrim = env.renderer->NewPrimitive();
			zone.hitboxPrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
			const glm::vec4 RED = {1.0f, 0.0f, 0.0f, 1.0f};
			zone.hitboxPrim->SetColors({RED, RED, RED, RED, RED});
#endif // defined(DEBUG_HITBOXES)
		}
#if defined(DEBUG_MOUSE_POS)
		mousePrim = env.renderer->NewPrimitive();
		mousePrim->SetDrawMode(Drawing::PDM_LINE_LOOP);
		mousePrim->SetVertices(MOUSE_POS_VERTICES);
		const glm::vec4 BLUE = {0.0f, 1.0f, 0.0f, 0.5f};
		mousePrim->SetColors({BLUE, BLUE, BLUE, BLUE});
#endif // defined(DEBUG_MOUSE_POS)

#define ACTBTN(csel, string) \
	do \
	{ \
		actBtn[csel] = GUI::CActBtn::New(env); \
		actBtn[csel]->SetImage(TextureFromPath(env.renderer, string)); \
		actBtn[csel]->SetCallback(std::bind(&CGraphicBoard::ActBtnSubmit, this, csel)); \
		actBtn[csel]->visible = false; \
	}while(0)
		ACTBTN(Core::CSELECT_W_EFFECT, "TEMP/act_act.png");
		ACTBTN(Core::CSELECT_SUMMONABLE, "TEMP/act_ns.png");
		ACTBTN(Core::CSELECT_SPSUMMONABLE, "TEMP/act_ss.png");
		ACTBTN(Core::CSELECT_REPOSITIONABLE, "TEMP/act_ad.png");
		ACTBTN(Core::CSELECT_MSETABLE, "TEMP/act_mset.png");
		ACTBTN(Core::CSELECT_SSETABLE, "TEMP/act_sset.png");
		ACTBTN(Core::CSELECT_CAN_ATTACK, "TEMP/act_atk.png");
#undef ACTBTN
		for(int i = Core::CSELECT_W_EFFECT; i <= Core::CSELECT_CAN_ATTACK; i++)
			env.Add(actBtn[i]);
	}
	
	virtual ~CGraphicBoard() = default;
	
	/******************** IGraphicBoard overrides ********************/
	void SetCameraPosition(const glm::vec3& pos) override
	{
		cam.pos = pos;
	}
	
	void UpdateHitboxes() override
	{
		const glm::mat4 vvp = CalcVpMatrix() * cam.vp;
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
		HandHitbox(0);
		HandHitbox(1);
	}
	
	void SetAnswerSubmitter(AnswerCallback cb) override
	{
		answerSubmitter = cb;
	}
	
	void Resize(const SDL_Rect& parent, const SDL_Rect& rect) override
	{
		cam.pCan = parent;
		cam.can = rect;
		const glm::mat4 ortho = glm::ortho<float>(0.0f, parent.w, parent.h, 0.0f);
		
		SDL_Rect bCanvas = {10, 10, 64, 64};
		for(int i = Core::CSELECT_W_EFFECT; i <= Core::CSELECT_CAN_ATTACK; i++)
		{
			actBtn[i]->Resize(ortho, bCanvas);
			bCanvas.y += 64 + 10;
		}
		
		UpdateMatrices();
	}
	
	void AddMsg(const Core::AnyMsg& msg) override
	{
		targetState += targetState == msgs.size();
		DuelBoard<GraphicCard>::AppendMsg(msg);
	}
	
	uint32_t GetState() const override
	{
		return state;
	}

	uint32_t GetStatesCount() const override
	{
		return msgs.size();
	}

	uint32_t GetTargetState() const override
	{
		return targetState;
	}

	bool SetTargetState(uint32_t tState) override
	{
		if(tState > msgs.size())
			return false;
		targetState = tState;
		return true;
	}
	
	void FillPile(uint32_t controller, uint32_t location, int num) override
	{
		DuelBoard<GraphicCard>::FillPile(controller, location, num);
		auto& pile = GetPile(controller, location);
		uint32_t seq = 0;
		for(auto& card : pile)
		{
			const Place p = {controller, location, seq, -1};
			InitializeGraphicCard(card);
			card.loc = GetLocXYZ(p);
			card.rot = GetRotXYZ(p, card.pos());
			seq++;
		}
	}
	/*****************************************************************/
private:
	// ************************
	Drawing::Texture cover;
	CardTextureManager ctm;
	// ************************ TODO: Move to own structure (perhaps `gfx`)
#if defined(DEBUG_MOUSE_POS)
	Drawing::Primitive mousePrim;
#endif // defined(DEBUG_MOUSE_POS)
	
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
	Animator ani;
	
	uint32_t targetState{};
	std::map<LitePlace, glm::vec3> locations;
	std::map<LitePlace, Zone> zones;
	
	LitePlace const* selectedZone{nullptr};
	GraphicCard* selectedCard{nullptr};
	
	//*********************************
	
	// Card selection
	bool multiSelect;
	std::vector<std::reference_wrapper<GraphicCard>> selectedCards;
	std::map<Place, GraphicCard&> cardsWithAction;
	GUI::ActBtn actBtn[Core::CardSelectionType_ARRAYSIZE];
	// Zone selection
	unsigned zoneSelectCount; // total number of zones needed to select
	std::set<LitePlace> selectedZones;
	std::set<LitePlace> selectableZones;
	
	AnswerCallback answerSubmitter;
	
	void ActBtnSubmit(Core::CardSelectionType csel)
	{
		Core::Answer answer;
		auto cardSeq = answer.mutable_card_seqs()->add_card_seq();
		cardSeq->set_type(csel);
		GraphicCard& card = selectedCards.rbegin()->get();
		const int seq = card.action->ts[csel];
		cardSeq->set_sequence(seq);
		answerSubmitter(answer);
	}
	
	/**********************************************************************/
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
	
	template<typename T, typename... Args>
	inline void PushAnimation(Args&&... args)
	{
		ani.Push(std::make_shared<T>(std::forward<Args>(args)...));
	}
	
	inline glm::mat4 CalcVpMatrix()
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
		return glm::scale(sVec) * glm::translate(tVec);
	}
	
	inline void InitializeGraphicCard(GraphicCard& card)
	{
		// Front face (Card picture)
		card.front = env.renderer->NewPrimitive();
		card.front->depthTest = true;
		card.front->SetDrawMode(Drawing::GetQuadDrawMode());
		card.front->SetVertices(CARD_VERTICES);
		card.front->SetTexCoords(Drawing::GetQuadTexCoords());
		card.front->SetTexture(ctm.GetCardTextureByCode(0u));
		// Back face (Cover image)
		card.cover = env.renderer->NewPrimitive();
		card.cover->depthTest = true;
		card.cover->SetDrawMode(Drawing::GetQuadDrawMode());
		card.cover->SetVertices(CARD_COVER_VERTICES);
		card.cover->SetTexCoords({{1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});
		card.cover->SetTexture(cover);
	}
	
	inline void CancelRequestActions()
	{
		for(int i = Core::CSELECT_W_EFFECT; i <= Core::CSELECT_CAN_ATTACK; i++)
			actBtn[i]->visible = false;
		for(const auto& kv : cardsWithAction)
			kv.second.action.reset(nullptr);
		cardsWithAction.clear();
		selectedZones.clear();
		selectableZones.clear();
	}
	
	/**********************************************************************/
	
	/******************** IElement overrides ********************/
	void Resize([[maybe_unused]] const glm::mat4& mat,
	            [[maybe_unused]] const SDL_Rect& rect) override
	{}
	
	void Draw() override
	{
		env.renderer->SetViewport(cam.can.x, cam.can.y, cam.can.w, cam.can.h);
		for(const auto& kv : zones)
			kv.second.prim->Draw();
		ForAllCards([](GraphicCard& card)
		{
			card.front->Draw();
			card.cover->Draw();
		});
		env.renderer->SetViewport(cam.pCan.x, cam.pCan.y, cam.pCan.w, cam.pCan.h);
#if defined(DEBUG_HITBOXES)
		for(const auto& kv : zones)
			kv.second.hitboxPrim->Draw();
		for(const auto& pile : hand)
			for(const auto& card : pile)
				if(card.hitbox)
					card.hitbox->prim->Draw();
#endif // defined(DEBUG_HITBOXES)
#if defined(DEBUG_MOUSE_POS)
		mousePrim->Draw();
#endif // defined(DEBUG_MOUSE_POS)
	}
	
	void Tick() override
	{
		const float elapsed = env.GetTimeElapsed();
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
	
	bool OnMouseMotion(const SDL_MouseMotionEvent& motion)
	{
		glm::vec3 mPos = {motion.x / (cam.pCan.w * 0.5f) - 1.0f,
		                 -(motion.y / (cam.pCan.h * 0.5f) - 1.0f), 0.0f};
#if defined(DEBUG_MOUSE_POS)
		mousePrim->SetMatrix(glm::translate(mPos));
#endif // defined(DEBUG_MOUSE_POS)
		// Try checking for cards
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
		for(auto& pile : hand)
		{
			for(auto& card : pile)
			{
				if(card.hitbox && PointInPoly(mPos, card.hitbox->vertices))
				{
					selectedCard = &card;
					// TODO: card selection and/or hover animation
					return true;
				}
			}
		}
		// Try checking for zones
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
	
	bool OnMouseButtonDown([[maybe_unused]] const SDL_MouseButtonEvent& button)
	{
		auto SelectUnselectCard = [&](GraphicCard& card)
		{
			if(!multiSelect)
			{
				selectedCards.clear();
				selectedCards.emplace_back(card);
				return;
			}
			auto SearchFunc = [&card](GraphicCard& vcard)
			{
				return &card == &vcard;
			};
			auto search = std::find_if(selectedCards.begin(),
			                           selectedCards.end(), SearchFunc);
			if(search == selectedCards.cend())
				selectedCards.push_back(card);
			else
				selectedCards.erase(search);
		};
		if(selectedCard && selectedCard->action)
		{
			for(const auto& kv : selectedCard->action->ts)
				actBtn[kv.first]->visible = true;
			SelectUnselectCard(*selectedCard);
			return true;
		}
		if(selectedZone)
		{
			const auto& sZone = *selectedZone; // saves us few dereferences
			// Handle zone selection
			if(selectableZones.find(sZone) != selectableZones.cend())
			{
				if(selectedZones.find(sZone) == selectedZones.cend())
				{
					selectedZones.insert(sZone);
					if(selectedZones.size() == zoneSelectCount)
					{
						Core::Answer answer;
						auto places = answer.mutable_places();
						for(const auto& zone : selectedZones)
						{
							auto place = places->add_places();
							place->set_controller(CON(zone));
							place->set_location(LOC(zone));
							place->set_sequence(SEQ(zone));
						}
						answerSubmitter(answer);
					}
				}
				else
				{
					selectedZones.erase(sZone);
				}
				return true;
			}
			// Handle zone card selection
			const auto t = std::tuple_cat(sZone, std::tuple<int32_t>(-1));
			auto search = zoneCards.find(t);
			if(search != zoneCards.cend() && search->second.action)
			{
				for(const auto& kv : search->second.action->ts)
					actBtn[kv.first]->visible = true;
				SelectUnselectCard(search->second);
				return true;
			}
		}
		return false;
	}
	
	bool OnKeyDownEvent(const SDL_KeyboardEvent& key)
	{
		switch(key.keysym.scancode)
		{
		case SDL_SCANCODE_SPACE:
		{
		Core::Answer answer;
		answer.set_cancel(true);
		answerSubmitter(answer);
		return true;
		}
		// Animation controls
		case SDL_SCANCODE_END:
		{
		ani.FinishAll();
		return true;
		}
		case SDL_SCANCODE_Z:
		{
		ani.SetSpeed(0.1f);
		return true;
		}
		case SDL_SCANCODE_X:
		{
		ani.SetSpeed(1.0f);
		return true;
		}
		case SDL_SCANCODE_C:
		{
		ani.SetSpeed(2.0f);
		return true;
		}
		default: break;
		}
		return false;
	}
	
	bool OnEvent(const SDL_Event& e) override
	{
		if(e.type == SDL_MOUSEMOTION && OnMouseMotion(e.motion))
		{
			return true;
		}
		else if(e.type == SDL_MOUSEBUTTONDOWN && OnMouseButtonDown(e.button))
		{
			return true;
		}
		else if(e.type == SDL_KEYDOWN && !e.key.repeat && OnKeyDownEvent(e.key))
		{
			
		}
		return false;
	}
	/************************************************************/
	
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
	
	void UpdateMatrices()
	{
		// Recalculate View-Projection matrix
		const auto aspectRatio = static_cast<float>(cam.can.w) / cam.can.h;
		const auto proj = glm::perspective(1.0f, aspectRatio, 0.1f, 10.0f);
		const auto view = glm::lookAt(cam.pos, {0.0f, 0.0f, 0.0f}, UP);
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
	
	// Refreshes the hitboxes for the cards in hand.
	void HandHitbox(uint8_t player)
	{
		const glm::mat4 vvp = CalcVpMatrix() * cam.vp;
		for(auto& card : hand[player])
		{
			if(!card.hitbox)
				card.hitbox = std::make_unique<GraphicCard::HitboxData>();
			card.hitbox->vertices = CARD_HITBOX_VERTICES;
#if defined(DEBUG_HITBOXES)
			card.hitbox->prim = env.renderer->NewPrimitive();
			card.hitbox->prim->SetDrawMode(Drawing::PDM_LINE_LOOP);
			const glm::vec4 BLU = {0.0f, 0.0f, 1.0f, 1.0f};
			card.hitbox->prim->SetColors({BLU, BLU, BLU, BLU, BLU});
#endif // defined(DEBUG_HITBOXES)
			const glm::mat4 mvp = vvp * GetModel(card.loc, card.rot);
			for(size_t i = 0; i < CARD_HITBOX_VERTICES.size(); i++)
			{
				const glm::vec4 v4 = mvp * glm::vec4(CARD_HITBOX_VERTICES[i], 1.0f);
				card.hitbox->vertices[i] = glm::vec3(v4) / v4.w;
			}
#if defined(DEBUG_HITBOXES)
			card.hitbox->prim->SetVertices(card.hitbox->vertices);
#endif // defined(DEBUG_HITBOXES)
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
	
	bool Forward()
	{
		if(!DuelBoard<GraphicCard>::Forward())
			return false;
		CancelRequestActions();
		AnimateMsg(msgs[state - 1]);
		return true;
	}
	
	bool Backward()
	{
		if(!DuelBoard<GraphicCard>::Backward())
			return false;
		CancelRequestActions();
		AnimateMsg(msgs[state]);
		return true;
	}
	
	void AnimateMsg(const Core::AnyMsg& msg)
	{
		if(msg.AnyMsg_case() == Core::AnyMsg::kSpecific)
		{
			const auto& specific = msg.specific();
			if((state != msgs.size() || !advancing) &&
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
		default: break;
		}
	}
	
	void AnimateRequestMsg(const Core::Request& request)
	{
		switch(request.Request_case())
		{
#include "graphic_board_animate_request.inl"
		default: break;
		}
	}
};

std::shared_ptr<IGraphicBoard> IGraphicBoard::New(GUI::Environment& env, int flags)
{
	return std::make_shared<CGraphicBoard>(env, flags);
}

} // namespace YGOpen
