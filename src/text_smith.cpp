#include "text_smith.hpp"
#include <SDL.h>

#include "sdl_utility.hpp"

namespace YGOpen
{

static constexpr int SHADOW_DISTANCE_X = 1;
static constexpr int SHADOW_DISTANCE_Y = 2;
static SDL_Rect SHADOW_RECT = {SHADOW_DISTANCE_X, SHADOW_DISTANCE_Y, 0, 0};

TextSmith::~TextSmith()
{
	if(f != nullptr)
		TTF_CloseFont(f);
}

bool TextSmith::LoadFont(SDL_RWops* fontFile, int size)
{
	if(f != nullptr)
		TTF_CloseFont(f);
	if((f = TTF_OpenFontRW(fontFile, 0, size)) == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load font: %s",
		             TTF_GetError());
		return f != nullptr;
	}
	return f != nullptr;
}

void TextSmith::SetTextColor(const SDL_Color& c)
{
	tc = c;
}

void TextSmith::SetShadowColor(const SDL_Color& c)
{
	sc = c;
}

SDL_Surface* TextSmith::Text(std::string_view text) const
{
	if(f == nullptr)
		return SDLU_EmptySurface();
	SDL_Surface* fgText = TTF_RenderUTF8_Blended(f, text.data(), tc);
	return SDLU_SurfaceToRGBA32(fgText);
}

SDL_Surface* TextSmith::ShadowedText(std::string_view text) const
{
	if(f == nullptr)
		return SDLU_EmptySurface();
	SDL_Surface* fgText;
	SDL_Surface* bgText;
	if((fgText = TTF_RenderUTF8_Blended(f, text.data(), tc)) == nullptr)
		return SDLU_EmptySurface();
	if((bgText = TTF_RenderUTF8_Blended(f, text.data(), sc)) == nullptr)
	{
		SDL_FreeSurface(fgText);
		return SDLU_EmptySurface();
	}
	// Create a new surface from both surfaces blitted
	SDL_Surface* eText = SDL_CreateRGBSurfaceWithFormat(
	0, fgText->w + SHADOW_DISTANCE_X, fgText->h + SHADOW_DISTANCE_Y,
	32, SDL_PIXELFORMAT_RGBA32
	);
	SDL_BlitSurface(bgText, nullptr, eText, &SHADOW_RECT);
	SDL_BlitSurface(fgText, nullptr, eText, nullptr);
	SDL_FreeSurface(fgText);
	SDL_FreeSurface(bgText);
	return eText;
}

}  // namespace YGOpen
