#ifndef __GAME_STATE_HPP__
#define __GAME_STATE_HPP__
#include <SDL.h>

namespace YGOpen
{

class GameState
{
public:
	virtual void OnEvent(const SDL_Event& e) = 0;
	virtual void Tick() = 0;
};

}

#endif // __GAME_STATE_HPP__	
