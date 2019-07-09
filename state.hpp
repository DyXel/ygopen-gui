#ifndef __GAME_STATE_HPP__
#define __GAME_STATE_HPP__
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

#endif // __GAME_STATE_HPP__
