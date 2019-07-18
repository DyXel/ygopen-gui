#include "menu.hpp"

#include <SDL_image.h>

#include "../common_data.hpp"

#include "../drawing/primitive.hpp"
#include "../drawing/texture.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{
namespace State
{

Menu::Menu(const std::shared_ptr<CommonData>& ptrData) : data(ptrData)
{
	auto texture = Drawing::API::NewTexture();
	SDL_Surface* image = IMG_Load("texture.png");
	if(!image)
	{
		SDL_Log("IMG_Load: %s\n", IMG_GetError());
	}
	if(image->format->format != SDL_PIXELFORMAT_RGBA32)
	{
		SDL_Log("Changing image format");
		SDL_Surface* temp = image;
		image = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_RGBA32, 0);
		if(!image)
		{
			SDL_Log("SDL_ConvertSurfaceFormat: %s\n", SDL_GetError());
		}
		SDL_FreeSurface(temp);
	}
	
	texture->SetImage(image->w, image->h, image->pixels);
	
	SDL_FreeSurface(image);
	
	triangle = Drawing::API::NewPrimitive();
	const Drawing::Vertices vertices =
	{
		{ -1.0f,  1.0f, 0.0f},
		{  1.0f,  1.0f, 0.0f},
		{ -1.0f, -1.0f, 0.0f},
		{  1.0f, -1.0f, 0.0f},
	};
	const Drawing::Colors colors =
	{
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
	};
	const Drawing::TexCoords texCoords =
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},
	};
	
	triangle->SetDrawMode(Drawing::PDM_TRIANGLE_STRIP);
	triangle->SetVertices(vertices);
	triangle->SetTexCoords(texCoords);
	triangle->SetTexture(texture);
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
	Drawing::API::Present();
}

} // State
} // YGOpen
