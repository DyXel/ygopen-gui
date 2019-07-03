#include "menu.hpp"
#include <SDL_opengles2.h>

namespace YGOpen
{
namespace State
{

Menu::Menu()
{

}

Menu::~Menu()
{

}

void Menu::OnEvent([[maybe_unused]] const SDL_Event& e)
{

}

void Menu::Tick()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

} // State
} // YGOpen
