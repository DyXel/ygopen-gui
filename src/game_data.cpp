#include "game_data.hpp"
#include "game_instance.hpp"

#include <SDL_mutex.h>

namespace YGOpen
{

GameData::GameData(GameInstance& gi) : instance(gi)
{}

GameData::~GameData()
{
	if(guiFontFile)
		SDL_RWclose(guiFontFile);
}

int GameData::Init()
{
	cfgMutex = SDL_CreateMutex();
	if(cfgMutex == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateMutex: %s\n",
		                SDL_GetError());
		return 1;
	}
	return 0;
}

} // YGOpen
