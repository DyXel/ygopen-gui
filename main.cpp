#include <memory>
#include "game_instance.hpp"
#include "drawing/api.hpp"

static std::unique_ptr<YGOpen::GameInstance> gi;
[[noreturn]] static void quit(int rc);

#ifdef __ANDROID__
static const Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_ES;
#else
static const Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_CORE;
#endif

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to initialize SDL: %s\n", SDL_GetError());
		quit(1);
	}
	// TODO: select backend from commandline and fallback
	Drawing::API::PreloadBackend(DEFAULT_BACKEND);
	gi = std::make_unique<YGOpen::GameInstance>();
	if(gi->Init() != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to initialize main game instance");
		quit(1);
	}
	SDL_Event e;
	while(!gi->IsExiting())
	{
		while(SDL_PollEvent(&e))
			gi->PropagateEvent(e);
		gi->TickOnce();
		gi->DrawOnce();
	}
	quit(0);
}

static void quit(int rc)
{
	gi.reset();
	SDL_Quit();
	exit(rc);
}
