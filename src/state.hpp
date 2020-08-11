#ifndef YGOPEN_INSTANCE_STATE_HPP
#define YGOPEN_INSTANCE_STATE_HPP
#include <SDL_events.h>

namespace YGOpen
{

struct Service;

class StateConcept
{
	void OnEvent(const SDL_Event& e);
	void Tick();
};

} // YGOpen

#endif // YGOPEN_INSTANCE_STATE_HPP
