#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include <SDL.h>

namespace YGOpen
{
namespace State
{

class IState
{
public:
	virtual void OnEvent(const SDL_Event& e) = 0;
	virtual void Tick() = 0;
	virtual void Draw() = 0;
};

} // State
} // YGOpen

#endif // GAME_STATE_HPP
