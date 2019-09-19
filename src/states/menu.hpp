#ifndef MENU_HPP
#define MENU_HPP
#include <memory>

#include "../state.hpp"
#include "../drawing/types.hpp"
#include "../gui/environment.hpp"
#include "../gui/button.hpp"

namespace YGOpen
{

class GameInstance;
struct GameData;

namespace State
{

class Menu : public IState
{
public:
	Menu(GameInstance& gi, GameData& data, const Drawing::Renderer& renderer);
	virtual ~Menu() = default;
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
private:
	GameInstance& gi;
	GameData& data;
	Drawing::Renderer renderer;
	Drawing::Primitive bkg;
	Drawing::Vertices bkgVertices;
	GUI::Environment env;
	GUI::Button duelBtn;
	GUI::Button exitBtn;
	
	void OnResize();
};

} // State

} // YGOpen

#endif // MENU_HPP
