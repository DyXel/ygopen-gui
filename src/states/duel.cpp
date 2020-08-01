 #include "duel.hpp"

#include "../game_instance.hpp"
#include "../game_data.hpp"
#include "../drawing/renderer.hpp"

#include "../sdl_utility.hpp"

#include "deck.hpp"
#include "enums/duel_mode.hpp"
#include "enums/location.hpp"
#include "enums/position.hpp"

#include <glm/gtx/transform.hpp>

namespace YGOpen
{
namespace State
{

static void DataReader(void* pl, int code, OCG_CardData* data)
{
	bool found;
	*data = static_cast<CardDatabase*>(pl)->CardDataFromCode(code, found);
}

static int ScriptReader(void* pl, OCG_Duel duel, const char* name)
{
	auto& ci = *static_cast<CoreInterface*>(pl);
	const auto filePath = std::string("TEMP/script/") + name;
	std::vector<char> fileData = SDLU_TextFromFile(filePath);
	const std::vector<char>::size_type fdSize = fileData.size();
	return fdSize && ci.OCG_LoadScript(duel, fileData.data(), fdSize, name);
}

static void LogHandler([[maybe_unused]] void* pl, const char* str, int t)
{
	SDL_Log("From duel: type %i: %s", t, str);
}

Duel::Duel(GameInstance& gi, GameData& data, Drawing::Renderer renderer) :
	gi(gi), data(data), renderer(renderer), cdb(), ci("TEMP/libocgcore.so"),
	env(renderer, data.guiFont, data.elapsed)
{
	cdb.Merge("TEMP/cards.cdb");
	// Create core duel
	OCG_DuelOptions dopts{};
	dopts.seed = 0;
	dopts.flags = DUEL_MODE_MR4;
	dopts.team1 = dopts.team2 = {8000, 5, 1};
	dopts.cardReader = &DataReader;
	dopts.payload1 = &cdb;
	dopts.scriptReader = &ScriptReader;
	dopts.payload2 = &ci;
	dopts.logHandler = &LogHandler;
	int status = ci.OCG_CreateDuel(&duel, dopts);
	SDL_Log("Duel status: %i", status);
	if(status != OCG_DUEL_CREATION_SUCCESS)
		throw std::exception();
	// Load script dependencies
	const auto prefix = std::string("TEMP/script/");
	std::vector<char> fileData = SDLU_TextFromFile(prefix + "constant.lua");
	ci.OCG_LoadScript(duel, fileData.data(), fileData.size(), "constant.lua");
	fileData = SDLU_TextFromFile(prefix + "utility.lua");
	ci.OCG_LoadScript(duel, fileData.data(), fileData.size(), "utility.lua");
	// Create and attach graphic board
	gb = IGraphicBoard::New(env, dopts.flags);
	env.Add(gb);
	env.AddToTickSet(gb);
	// Add decks
	auto LoadDeck = [&](uint8_t team, uint8_t player, std::string_view fp)
	{
		OCG_NewCardInfo ncinf{};
		ncinf.team = ncinf.con = team;
		ncinf.duelist = player;
		ncinf.loc = LOCATION_DECK;
		ncinf.seq = 1;
		ncinf.pos = POS_FACEDOWN_ATTACK;
		std::vector<char> fileData = SDLU_TextFromFile(fp);
		Deck deck = Deck::FromJSON(nlohmann::json::parse(fileData));
		// Main Deck
		const auto& main = deck.GetMsg().main();
		gb->FillPile(team, ncinf.loc, main.size());
		for(auto code : main)
		{
			ncinf.code = code;
			ci.OCG_DuelNewCard(duel, ncinf);
		}
		// Extra Deck
		ncinf.loc = LOCATION_EXTRA;
		const auto& extra = deck.GetMsg().extra();
		gb->FillPile(team, ncinf.loc, extra.size());
		for(auto code : extra)
		{
			ncinf.code = code;
			ci.OCG_DuelNewCard(duel, ncinf);
		}
	};
	LoadDeck(0, 0, "TEMP/deck1.json");
	LoadDeck(1, 0, "TEMP/deck2.json");
	// Start duel
	ci.OCG_StartDuel(duel);
	// Update
	gb->SetAnswerSubmitter([&](const Proto::CAnswer& answer)
	{
		auto buffer = decoder.Decode(lastMsg.request(), answer);
		ci.OCG_DuelSetResponse(duel, buffer.data(), buffer.size());
		Process();
	});
	OnResize();
	Process();
}

Duel::~Duel()
{
	ci.OCG_DestroyDuel(duel);
}

void Duel::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_WINDOWEVENT &&
	   e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	{
		OnResize();
	}
	else if(e.type == SDL_KEYDOWN)
	{
		switch(e.key.keysym.scancode)
		{
			case SDL_SCANCODE_PAGEUP:
			{
				gb->SetTargetState(gb->GetTargetState() - 1);
				break;
			}
			case SDL_SCANCODE_PAGEDOWN:
			{
				gb->SetTargetState(gb->GetTargetState() + 1);
				break;
			}
			default:
			{
				break;
			}
		}
	}
	env.OnEvent(e);
}

void Duel::Tick()
{
	env.Tick();
}

void Duel::Draw()
{
	renderer->Clear();
	env.Draw();
	renderer->Present();
}

void Duel::Process()
{
	std::vector<uint8_t> buffer;
	int flag;
	do
	{
		flag = ci.OCG_DuelProcess(duel);
		{
			uint32_t size;
			void* msg = ci.OCG_DuelGetMessage(duel, &size);
			buffer.resize(size);
			std::memcpy(buffer.data(), msg, size);
		}
		// Encode to protobuf message
		auto msgs = encoder.Encode(buffer.data(), buffer.size());
		lastMsg = msgs.back();
		msgs.pop_back();
		for(const auto& msg : msgs)
			gb->AddMsg(msg);
		auto IsEmptySelectToChain = [](const Proto::CMsg& msg) -> bool
		{
			if(msg.t_case() != Proto::CMsg::kRequest)
				return false;
			const auto& request = msg.request();
			if(request.t_case() != Proto::CMsg::Request::kSelectToChain)
				return false;
			if(request.select_to_chain().activable_cards_size() != 0)
				return false;
			return true;
		};
		// Ignore empty SelectToChain messages
		if(IsEmptySelectToChain(lastMsg))
		{
			const auto& request = lastMsg.request();
			auto buffer = decoder.Decode(request, Proto::CAnswer());
			ci.OCG_DuelSetResponse(duel, buffer.data(), buffer.size());
			flag = OCG_DUEL_STATUS_CONTINUE;
			continue;
		}
		gb->AddMsg(lastMsg);
	}while(flag == OCG_DUEL_STATUS_CONTINUE);
}

void Duel::OnResize()
{
	const int& w = data.canvas.w;
	const int& h = data.canvas.h;
	SDL_Rect boardRect{};
	if(w < h)
	{
		boardRect.w = boardRect.h = w;
		boardRect.y = (h - w) / 2;
	}
	else
	{
		boardRect.w = boardRect.h = h;
		boardRect.x = (w - h) / 2;
	}
	gb->Resize(data.canvas, boardRect);
	gb->UpdateHitboxes();
}

}  // namespace State
}  // namespace YGOpen
