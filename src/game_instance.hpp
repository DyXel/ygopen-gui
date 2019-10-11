#ifndef GAME_INSTANCE_HPP
#define GAME_INSTANCE_HPP
#include <memory>
#include <SDL.h>
#include "game_data.hpp"
#include "drawing/sdlwindow.hpp"

namespace YGOpen
{

namespace State
{
class IState;
} // State

class GameInstance : public Drawing::SDLWindow
{
public:
	GameInstance(const Drawing::Backend backend);
	~GameInstance();
	
	GameInstance(const GameInstance&) = delete;
	GameInstance(GameInstance&&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;
	GameInstance& operator=(GameInstance&&) = delete;
	
	void Run();
	
	void Exit();
	void SetState(std::shared_ptr<State::IState> newState);
private:
	GameData data;
	bool exiting{false};
	std::shared_ptr<State::IState> state;
	
	unsigned now{0u}, then{0u};
	unsigned recording{0u}; // if non 0. Framerate being recorded at.
	
	void OnEvent(const SDL_Event& e);
	void Tick();
	void Draw() const;
	
	inline void UpdateCanvas();
};

} // YGOpen

#endif // GAME_INSTANCE_HPP
