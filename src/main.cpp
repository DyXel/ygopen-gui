#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "game_instance.hpp"
#include "drawing/api.hpp"

static std::unique_ptr<YGOpen::GameInstance> gi;

#if defined(__ANDROID__)
static constexpr Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_ES;
#else
static constexpr Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_CORE;
#endif

extern "C" int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	static int exitCode = 0;
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to initialize SDL: %s\n", SDL_GetError());
		exitCode = 1;
		goto quit;
	}
	{
		static constexpr auto IMG_INIT_FLAGS = IMG_INIT_PNG;
		if((IMG_Init(IMG_INIT_FLAGS) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			                "Unable to initialize SDL_image: %s",
			                IMG_GetError());
			exitCode = 1;
			goto quit;
		}
	}
	// TODO: select backend from commandline and fallback
	Drawing::API::PreloadBackend(DEFAULT_BACKEND);
	gi = std::make_unique<YGOpen::GameInstance>();
	if(gi->Init(DEFAULT_BACKEND) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to initialize main game instance");
		exitCode = 1;
		goto quit;
	}
	SDL_Event e;
	while(!gi->IsExiting())
	{
		while(SDL_PollEvent(&e))
			gi->PropagateEvent(e);
		gi->TickOnce();
		gi->DrawOnce();
	}
quit:
	gi.reset();
	IMG_Quit();
	SDL_Quit();
	return exitCode;
}
