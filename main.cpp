#include <memory>
#include "game_instance.hpp"

static std::unique_ptr<YGOpen::GameInstance> gi;
[[noreturn]] static void quit(int rc);

int main(int argc, char *argv[])
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
		                "Unable to initialize SDL: %s\n", SDL_GetError());
		quit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
// 	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	gi = std::make_unique<YGOpen::GameInstance>();
	if(gi->Init() != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
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
