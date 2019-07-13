#include "menu.hpp"
#include "../drawing/gl_mesh.hpp"
#include "../drawing/gl_shader_and_program.hpp"

namespace YGOpen
{
namespace State
{

Menu::Menu()
{
	
	// Create circle
	Drawing::Vertices vertices;
	Drawing::Colors colors;
	const float full = 2.0f * M_PI;
	for(float i = 0.0f; i < full; i += full / 100.0f)
	{
		vertices.push_back({std::sin(i), std::cos(i), 0.0f});
		colors.push_back({1.0f, 0.0f, 0.0f, 1.0f});
	}
}

Menu::~Menu()
{

}

void Menu::OnEvent([[maybe_unused]] const SDL_Event& e)
{

}

void Menu::Tick()
{

}

void Menu::Draw()
{
	
	
}

} // State
} // YGOpen
