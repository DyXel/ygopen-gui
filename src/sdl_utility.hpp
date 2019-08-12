#ifndef SDL_UTILITY_HPP
#define SDL_UTILITY_HPP
#include <memory>
#include <SDL_video.h>

SDL_Surface* SDLU_EmptySurface();
SDL_Surface* SDLU_SurfaceToRGBA32(SDL_Surface* surf);
std::unique_ptr<char[]> SDLU_TextFromFile(std::string_view path);

#endif // SDL_UTILITY_HPP
