#ifndef YGOPEN_STATE_MENU_HPP
#define YGOPEN_STATE_MENU_HPP
#include "../state.hpp"

namespace YGOpen::State
{

class Menu final
{
public:
	Menu(Service& svc);
	
	void OnEvent(const SDL_Event& e);
	void Tick();
private:
	Service& svc;
};

} // namespace YGOpen::State

#endif // YGOPEN_STATE_MENU_HPP
