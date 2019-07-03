#include <static_configs.hpp>
#include "game_instance.hpp"
#include "state.hpp"
#include "states/loading.hpp"

namespace YGOpen
{
#define LOG_GL_STRING(n) LogGLString(#n, n)

GameInstance::GameInstance()
{
	SDL_Log("GameInstance constructor");
}

GameInstance::~GameInstance()
{
	state.reset();
	if(window != nullptr)
		SDL_DestroyWindow(window);
	if(glCtx != nullptr)
		SDL_GL_DeleteContext(glCtx);
	SDL_Log("GameInstance destructor");
}

int GameInstance::Init(/*int argc, char argv**/)
{
	window = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH,
	                          DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if(window == NULL)
	{
		window = nullptr;
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to create SDL Window: %s",
		             SDL_GetError());
		return -1;
	}
	glCtx = SDL_GL_CreateContext(window);
	if(glCtx == NULL)
	{
		glCtx = nullptr;
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,
		             "Unable to create OpenGL context: %s", SDL_GetError());
		return -1;
	}
	LOG_GL_STRING(GL_RENDERER);
	LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
	LOG_GL_STRING(GL_VERSION);
// 	LOG_GL_STRING(GL_EXTENSIONS); // Too spammy
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
	cfgs = std::make_shared<Configs>();
	state = std::make_shared<State::Loading>(*this, cfgs);
	return 0;
}

void GameInstance::LogGLString(const char* nameStr, const GLenum name)
{
	const GLubyte* ret = glGetString(name);
	if (ret == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,
		             "Unable to get OpenGL string: %s (0x%X)", nameStr, name);
	}
	else
	{
		SDL_Log("OpenGL: %s (0x%X): %s", nameStr, name, ret);
	}
}

bool GameInstance::IsExiting() const
{
	return exiting;
}

void GameInstance::PropagateEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
		exiting = true;
	state->OnEvent(e);
}

void GameInstance::TickOnce()
{
	state->Tick();
	SDL_GL_SwapWindow(window);
}

void GameInstance::SetState(std::shared_ptr<State::IState> newState)
{
	state = newState;
}


} // YGOpen
