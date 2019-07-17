#include "menu.hpp"
#include "../drawing/types.hpp"
#include "../common_data.hpp"

namespace YGOpen
{
namespace State
{

Menu::Menu()
Menu::Menu(const std::shared_ptr<CommonData>& ptrData) : data(ptrData)
{
	triangle = Drawing::API::NewPrimitive();
	const Drawing::Vertices vertices =
	{
		{-1.0f,  1.0f,  0.0f},
		{ 1.0f,  1.0f,  0.0f},
		{-1.0f, -1.0f,  0.0f},
		{ 1.0f, -1.0f,  0.0f},
		{ 0.0f,  0.0f,  0.0f},
	};
	const Drawing::Colors colors =
	{
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
	};
	const Drawing::Indices indices =
	{
		0, 4, 2, 1, 3, 4
	};
	
	triangle->SetDrawMode(Drawing::PDM_TRIANGLES);
	triangle->SetVertices(vertices);
	triangle->SetColors(colors);
	triangle->SetIndices(indices);
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
	Drawing::API::Present();
}

} // State
} // YGOpen
