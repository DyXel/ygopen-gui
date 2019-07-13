#include "menu.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{
namespace State
{

Menu::Menu()
{
	triangle = Drawing::API::NewPrimitive();
	const Drawing::Vertices vertices =
	{
		{ 0.0f,  0.5f,  0.0f},
		{-0.5f, -0.5f,  0.0f},
		{ 0.5f, -0.5f,  0.0f},
	};
	const Drawing::Colors colors =
	{
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
	};
	
	triangle->SetVertices(vertices);
	triangle->SetColors(colors);
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
	Drawing::API::Clear();
	triangle->Draw();
}

} // State
} // YGOpen
