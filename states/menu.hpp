#ifndef MENU_HPP
#define MENU_HPP
#include <memory>
#include "../state.hpp"

namespace OGL
{
class Program;
class Mesh;
}

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
private:
};

} // State
} // YGOpen

#endif // MENU_HPP
