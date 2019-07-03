#ifndef MENU_HPP
#define MENU_HPP
#include "../state.hpp"

namespace YGOpen
{
namespace State
{

class Menu : public IState
{
public:
	Menu();
	virtual ~Menu();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
};

} // State
} // YGOpen

#endif // MENU_HPP
