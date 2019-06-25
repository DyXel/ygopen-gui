#ifndef __GAME_INSTANCE_HPP__
#define __GAME_INSTANCE_HPP__
#include <memory>
#include <SDL.h>
#include <SDL_opengles2.h>

namespace YGOpen
{
class GameState;

class GameInstance
{
public:
	GameInstance();
	~GameInstance();
	int Init(/*int argc, char argv**/);
	bool IsExiting() const;
	void PropagateEvent(const SDL_Event& e);
	void TickOnce();
	void SetState(std::shared_ptr<GameState> state);
private:
	bool exiting{false};
	SDL_Window* window{nullptr};
	SDL_GLContext glCtx{nullptr};
	std::shared_ptr<GameState> state;

	void LogGLString(const char* nameStr, const GLenum name);
};

} // YGOpen

#endif // __GAME_INSTANCE_HPP__
