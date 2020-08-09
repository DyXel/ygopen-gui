#ifndef GAME_INSTANCE_HPP
#define GAME_INSTANCE_HPP
#include <memory>
#include <SDL.h>

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
	int width, height;
	float dpi;
	SDL_Window* sdlWindow;
	SDL_GLContext sdlGLCtx;
	bool exiting{false};
	
	unsigned now{0u}, then{0u};
	unsigned recording{0u}; // if non 0. Framerate being recorded at.
	
	void ConstructWindowAndGLCtx();
	
	void OnEvent(const SDL_Event& e);
	void Tick();
};

} // YGOpen

#endif // GAME_INSTANCE_HPP
