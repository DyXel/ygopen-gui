#ifndef MENU_HPP
#define MENU_HPP
#include <memory>
#include "../state.hpp"
#include "../drawing/api.hpp"

namespace YGOpen
{

struct GameData;

namespace State
{

class Menu : public IState
{
public:
	Menu(GameData* ptrData);
	virtual ~Menu();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
private:
	GameData* data;
	Drawing::Matrix proj;
	Drawing::Primitive triangle;
	
	void OnResize();
};

} // State

} // YGOpen

#endif // MENU_HPP
