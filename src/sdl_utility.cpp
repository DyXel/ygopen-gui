#include "sdl_utility.hpp"

SDL_Surface* SDLU_EmptySurface()
{
	return SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_RGBA32);
}

SDL_Surface* SDLU_SurfaceToRGBA32(SDL_Surface* surf)
{
	if(surf == nullptr)
	{
		SDL_SetError("Surface is NULL");
		return nullptr;
	}
	if(SDL_Surface* newSurf; surf->format->format != SDL_PIXELFORMAT_RGBA32)
	{
		newSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
		SDL_FreeSurface(surf);
		if(newSurf == nullptr)
			newSurf = SDLU_EmptySurface();
		return newSurf;
	}
	return surf;
}

std::unique_ptr<char[]> SDLU_TextFromFile(std::string_view path)
{
	SDL_RWops* rw = SDL_RWFromFile(path.data(), "rb");
	if (rw == nullptr)
		return std::unique_ptr<char[]>();
	const auto fileSize = static_cast<std::size_t>(SDL_RWsize(rw));
	std::unique_ptr<char[]> fileData(new char[fileSize + 1]);
	std::size_t readTotal = 0, read = 1;
	char* buf = fileData.get();
	while(readTotal < fileSize && read != 0)
	{
		read = SDL_RWread(rw, buf, 1, (fileSize - readTotal));
		readTotal += read;
		buf += read;
	}
	SDL_RWclose(rw);
	if(readTotal != fileSize)
		return std::unique_ptr<char[]>();
	fileData[readTotal] = '\0';
	return fileData;
}
