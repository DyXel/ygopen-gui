#include "sdl_utility.hpp"

SDL_Surface* SDLU_EmptySurface()
{
	return SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_RGBA32);
}

SDL_Surface* SDLU_SurfaceToRGBA32(SDL_Surface* surf)
{
	if(!surf)
	{
		SDL_SetError("Surface is NULL");
		return nullptr;
	}
	if(SDL_Surface* newSurf; surf->format->format != SDL_PIXELFORMAT_RGBA32)
	{
		newSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
		SDL_FreeSurface(surf);
		if(!newSurf)
			newSurf = SDLU_EmptySurface();
		return newSurf;
	}
	return surf;
}
