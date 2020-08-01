#ifndef DUEL_HPP
#define DUEL_HPP

#include "../state.hpp"
#include "../drawing/types.hpp"
#include "../gui/environment.hpp"

#include "../gframe/graphic_board.hpp"

#include "card_database.hpp"
#include "core_interface.hpp"
#include "codec/msg_codec.hpp"

namespace YGOpen
{

class GameInstance;
struct GameData;

namespace State
{

class Duel : public IState
{
public:
	Duel(GameInstance& gi, GameData& data, Drawing::Renderer renderer);
	virtual ~Duel();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
	
	void StartDuel();
private:
	GameInstance& gi;
	GameData& data;
	Drawing::Renderer renderer;
	
	CardDatabase cdb;
	CoreInterface ci;
	OCG_Duel duel{nullptr};
	Proto::CMsg lastMsg;
	MsgEncoder encoder;
	MsgDecoder decoder;
	
	GUI::Environment env;
	GraphicBoard gb;
	
	void Process();
	
	void OnResize();
};

} // State

} // YGOpen

#endif // DUEL_HPP
