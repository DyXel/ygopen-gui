#include "graphic_board.hpp"

#include <SDL_image.h>

#include <enums/phase.hpp>

#include <client/msg_interpreter.hpp>
#include "animator.hpp"
#include "card_texture_manager.hpp"
#include "graphic_board_base.hpp"
#include "animations/call.hpp"
#include "animations/move_card.hpp"
#include "animations/set_card_image.hpp"
#include "elements/action_btn.hpp"
#include "../sdl_utility.hpp"
#include "../drawing/renderer.hpp"
#include "../drawing/primitive.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/quad.hpp"
#include "../gui/environment.hpp"

namespace YGOpen
{

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

class CGraphicBoard final : public IGraphicBoard, public GraphicBoardBase, public MsgInterpreter<GraphicCard>
{
public:
	CGraphicBoard(GUI::Environment& env, int flags)
		: IGraphicBoard(env), GraphicBoardBase(env.renderer), ctm(env.renderer)
	{
		SetCardsTextures(ctm.GetCardTextureByCode(0u),
		                 TextureFromPath(env.renderer, "TEMP/cover.png"));
		SetZonesTextures(TextureFromPath(env.renderer, "TEMP/zone.png"));
		RebuildLocations(flags);

#define ACTBTN(csel, string) \
	do \
	{ \
		actBtn[csel] = GUI::CActBtn::New(env); \
		actBtn[csel]->SetImage(TextureFromPath(env.renderer, string)); \
		actBtn[csel]->SetCallback(std::bind(&CGraphicBoard::ActBtnSubmit, this, csel)); \
		actBtn[csel]->visible = false; \
	}while(0)
		ACTBTN(Proto::CData::SELECTION_TYPE_ACTIVABLE, "TEMP/act_act.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_SUMMONABLE, "TEMP/act_ns.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_SPSUMMONABLE, "TEMP/act_ss.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_REPOSITIONABLE, "TEMP/act_ad.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_MSETABLE, "TEMP/act_mset.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_SSETABLE, "TEMP/act_sset.png");
		ACTBTN(Proto::CData::SELECTION_TYPE_CAN_ATTACK, "TEMP/act_atk.png");
#undef ACTBTN
		for(int i = Proto::CData::SELECTION_TYPE_ACTIVABLE; i <= Proto::CData::SELECTION_TYPE_CAN_ATTACK; i++)
			env.Add(actBtn[i]);
	}
	
	virtual ~CGraphicBoard() = default;
	
	/******************** IGraphicBoard overrides ********************/
	void UpdateHitboxes() override
	{
		HandHitbox(0);
		HandHitbox(1);
	}
	
	void SetAnswerSubmitter(AnswerCallback cb) override
	{
		answerSubmitter = cb;
	}
	
	void Resize(const SDL_Rect& parent, const SDL_Rect& rect) override
	{
		GraphicBoardBase::Resize(parent, rect);
		const glm::mat4 ortho = glm::ortho<float>(0.0f, parent.w, parent.h, 0.0f);
		SDL_Rect bCanvas = {10, 10, 64, 64};
		for(int i = Proto::CData::SELECTION_TYPE_ACTIVABLE; i <= Proto::CData::SELECTION_TYPE_CAN_ATTACK; i++)
		{
			actBtn[i]->Resize(ortho, bCanvas);
			bCanvas.y += 64 + 10;
		}
	}
	
	void AddMsg(const Proto::CMsg& msg) override
	{
		targetState += targetState == Msgs().size();
		MsgInterpreter<GraphicCard>::AppendMsg(msg);
	}
	
	uint32_t GetTargetState() const override
	{
		return targetState;
	}

	bool SetTargetState(uint32_t tState) override
	{
		if(tState > Msgs().size())
			return false;
		targetState = tState;
		return true;
	}
	
	void FillPile(uint32_t controller, uint32_t location, int num) override
	{
		MsgInterpreter<GraphicCard>::FillPile(controller, location, num);
		auto& pile = GetPile(controller, location);
		uint32_t seq = 0;
		for(auto& card : pile)
		{
			const Place p = {controller, location, seq, -1};
			LazyCardGraphic(card);
			card.loc = GetLocXYZ(p);
			card.rot = GetRotXYZ(p, card.pos());
			seq++;
		}
	}
	/*****************************************************************/
private:
	CardTextureManager ctm;
	Animator ani;
	uint32_t targetState{};
	
	//*********************************
	
	// Card selection
	bool multiSelect;
	std::vector<std::reference_wrapper<GraphicCard>> selectedCards;
	std::map<Place, GraphicCard&> cardsWithAction;
	GUI::ActBtn actBtn[Proto::CData::SelectionType_ARRAYSIZE];
	// Zone selection
	unsigned zoneSelectCount; // total number of zones needed to select
	std::set<LitePlace> selectedZones;
	std::set<LitePlace> selectableZones;
	
	AnswerCallback answerSubmitter;
	
	void ActBtnSubmit(Proto::CData::SelectionType csel)
	{
		Proto::CAnswer answer;
		auto sseq = answer.add_seqs();
		sseq->set_type(csel);
		GraphicCard& card = selectedCards.rbegin()->get();
		const int seq = card.action->ts[csel];
		sseq->set_seq(seq);
		answerSubmitter(answer);
	}
	
	/**********************************************************************/
	template<typename T, typename... Args>
	inline void PushAnimation(Args&&... args)
	{
		ani.Push(std::make_shared<T>(std::forward<Args>(args)...));
	}
	
	inline void CancelRequestActions()
	{
		for(int i = Proto::CData::SELECTION_TYPE_ACTIVABLE; i <= Proto::CData::SELECTION_TYPE_CAN_ATTACK; i++)
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
		GraphicBoardBase::Draw();
	}
	
	void Tick() override
	{
		const float elapsed = env.GetTimeElapsed();
		const auto state = State();
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
		if(SelectedCard() && SelectedCard()->action)
		{
			// TODO: hide previous buttons
			for(const auto& kv : SelectedCard()->action->ts)
				actBtn[kv.first]->visible = true;
			SelectUnselectCard(*SelectedCard());
			return true;
		}
		if(SelectedZone())
		{
			const auto& sZone = *SelectedZone(); // saves us few dereferences
			// Handle zone selection
			if(selectableZones.find(sZone) != selectableZones.cend())
			{
				if(selectedZones.find(sZone) == selectedZones.cend())
				{
					selectedZones.insert(sZone);
					if(selectedZones.size() == zoneSelectCount)
					{
						Proto::CAnswer answer;
						for(const auto& zone : selectedZones)
						{
							auto place = answer.add_places();
							place->set_con(CON(zone));
							place->set_loc(LOC(zone));
							place->set_seq(SEQ(zone));
							place->set_oseq(-1);
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
			if(CardExists(t))
			{
				auto& card = GetCard(t);
				if(card.action)
				{
					// TODO: hide previous buttons
					for(const auto& kv : card.action->ts)
						actBtn[kv.first]->visible = true;
					SelectUnselectCard(card);
					return true;
				}
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
		Proto::CAnswer answer;
		answer.set_finish(true);
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
		if(GraphicBoardBase::OnEvent(e))
		{
			return true;
		}
		else if(e.type == SDL_MOUSEBUTTONDOWN && OnMouseButtonDown(e.button))
		{
			return true;
		}
		else if(e.type == SDL_KEYDOWN && !e.key.repeat && OnKeyDownEvent(e.key))
		{
			return true;
		}
		return false;
	}
	/************************************************************/
	
	// Refreshes the hitboxes for the cards in hand.
	void HandHitbox(uint8_t player)
	{
		for(auto& card : GetPile(player, LOCATION_HAND))
			LazyCardHitbox(card);
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
				for(auto& card : GetPile(player, location))
				{
					SEQ(place) = i;
					UpdatePos(place, card);
					i++;
				}
			}
		}
		for(auto& kv : ZoneCards())
			UpdatePos(kv.first, kv.second);
	}
	
	bool Forward()
	{
		if(!MsgInterpreter<GraphicCard>::Forward())
			return false;
		CancelRequestActions();
		AnimateMsg(Msgs()[State() - 1u]);
		return true;
	}
	
	bool Backward()
	{
		if(!MsgInterpreter<GraphicCard>::Backward())
			return false;
		CancelRequestActions();
		AnimateMsg(Msgs()[State()]);
		return true;
	}
	
	void AnimateMsg(const Proto::CMsg& msg)
	{
		if(msg.t_case() == Proto::CMsg::kInfo)
		{
			AnimateInfoMsg(msg.info());
		}
		else if(msg.t_case() == Proto::CMsg::kRequest)
		{
			// Skip request message if it can't be answered
			// TODO: split between updating cards and setting up answering mechanism
			if(State() != Msgs().size() || !IsAdvancing())
				return;
			AnimateRequestMsg(msg.request());
		}
		else
		{
			// TODO
		}
	}
	
	void AnimateInfoMsg(const Proto::CMsg::Info& info)
	{
		switch(info.t_case())
		{
#include "graphic_board_animate_info.inl"
		default: break;
		}
	}
	
	void AnimateRequestMsg(const Proto::CMsg::Request& request)
	{
		switch(request.t_case())
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
