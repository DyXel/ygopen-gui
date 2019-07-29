#ifndef MENU_HPP
#define MENU_HPP
#include <memory>
#include "../drawing/api.hpp"
#include "../gui/environment.hpp"
#include "../gui/button.hpp"
#include "../state.hpp"

namespace YGOpen
{

struct GameData;

namespace State
{

class Menu : public IState
{
public:
	Menu(GameData* ptrData);
	virtual ~Menu() = default;
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
private:
	GameData* data;
	Drawing::Matrix proj;
	Drawing::Primitive bkg;
	GUI::Environment env;
	GUI::Button duelBtn;
	GUI::Button exitBtn;
	
	void OnResize();
};

} // State

} // YGOpen

#endif // MENU_HPP
