#include "card_texture_manager.hpp"
#include <SDL_image.h>
#include <fmt/format.h>

#include "../sdl_utility.hpp"
#include "../drawing/renderer.hpp"
#include "../drawing/texture.hpp"

namespace YGOpen
{

Drawing::Texture CardTextureManager::GetTextureFromCode(uint32_t code)
{
	auto texture = renderer->NewTexture();
	std::string filePath = fmt::format("TEMP/pics/{}.jpg", code);
	SDL_Surface* image = IMG_Load(filePath.c_str());
	if((image != nullptr) &&
	   ((image = SDLU_SurfaceToRGBA32(image)) != nullptr))
	{
		texture->SetImage(image->w, image->h, image->pixels);
		return texture;
	}
	texture.reset();
	return texture;
}

CardTextureManager::CardTextureManager(Drawing::Renderer renderer) :
	renderer(renderer)
{
	errorTexture = renderer->NewTexture();
	// Load error texture
	SDL_Surface* image = IMG_Load("TEMP/unknown.jpg");
	if((image != nullptr) &&
	   ((image = SDLU_SurfaceToRGBA32(image)) != nullptr))
	{
		errorTexture->SetImage(image->w, image->h, image->pixels);
	}
}

CardTextureManager::~CardTextureManager()
{}

Drawing::Texture CardTextureManager::GetCardTextureByCode(uint32_t code)
{
	// Code is invalid or texture was attempted to load but failed before
	if(code == 0u || error_cards.find(code) != error_cards.end())
		return errorTexture;

	auto search = textures.find(code);
	// Texture is already loaded
	if(search != textures.end())
		return search->second;

	// Attempt to load file and create texture
	Drawing::Texture tex = GetTextureFromCode(code);
	if(tex)
	{
		textures.emplace(code, tex);
		return tex;
	}

	// Otherwise add to the failed list
	error_cards.insert(code);
	return errorTexture;
}

} // namespace YGOpen
