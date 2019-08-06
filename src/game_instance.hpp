#ifndef GAME_INSTANCE_HPP
#define GAME_INSTANCE_HPP
#include <memory>
#include <SDL.h>
#include "game_data.hpp"
#include "drawing/api.hpp"

namespace YGOpen
{

namespace State
{

class IState;

} // State

class GameInstance
{
public:
	GameInstance();
	~GameInstance();
	
	GameInstance(const GameInstance&) = delete;
	GameInstance(GameInstance&&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;
	GameInstance& operator=(GameInstance&&) = delete;
	
	int Init(Drawing::Backend backend);
	bool IsExiting() const;
	void Exit();
	void PropagateEvent(const SDL_Event& e);
	void TickOnce();
	void DrawOnce();
	void SetState(std::shared_ptr<State::IState> newState);
private:
	GameData data;
	bool exiting{false};
	SDL_Window* window{nullptr};
	std::shared_ptr<State::IState> state;
	
	unsigned now{0u}, then{0u};
	unsigned recording{0u}; // if non 0. Framerate being recorded at.
};

} // YGOpen

#endif // GAME_INSTANCE_HPP
