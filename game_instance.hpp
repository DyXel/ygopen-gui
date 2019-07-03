#ifndef __GAME_INSTANCE_HPP__
#define __GAME_INSTANCE_HPP__
#include <memory>
#include <SDL.h>
#include <SDL_opengles2.h>
#include "configs.hpp"

namespace YGOpen
{
namespace State
{
class IState;
}

class GameInstance
{
public:
	GameInstance();
	~GameInstance();
	int Init(/*int argc, char argv**/);
	bool IsExiting() const;
	void PropagateEvent(const SDL_Event& e);
	void TickOnce();
	void DrawOnce();
	void SetState(std::shared_ptr<State::IState> newState);
private:
	bool exiting{false};
	SDL_Window* window{nullptr};
	SDL_GLContext glCtx{nullptr};
	
	std::shared_ptr<Configs> cfgs;
	std::shared_ptr<State::IState> state;

	void LogGLString(const char* nameStr, const GLenum name);
};

} // YGOpen

#endif // __GAME_INSTANCE_HPP__
