#ifndef GAME_INSTANCE_HPP
#define GAME_INSTANCE_HPP
#include <memory>
#include <SDL.h>

#include "service.hpp"
#include "service/config.hpp"
#include "service/immediate.hpp"

namespace YGOpen
{

class GameInstance final
{
public:
	GameInstance();
	~GameInstance();
	
	GameInstance(const GameInstance&) = delete;
	GameInstance(GameInstance&&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;
	GameInstance& operator=(GameInstance&&) = delete;
	
	void Run();
private:
	Service::Config cfg;
	Service::Immediate imm;
	Service svc;
	
	SDL_Window* sdlWindow;
	SDL_GLContext sdlGLCtx;
	unsigned now{0u}, then{0u};
	
	void ConstructWindowAndGLCtx();
	void ConstructEgoRenderer();
	
	void OnEvent(const SDL_Event& e);
	void Tick();
};

} // namespace YGOpen

#endif // GAME_INSTANCE_HPP
