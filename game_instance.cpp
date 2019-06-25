#include "game_instance.hpp"
#include "game_state.hpp"

namespace YGOpen
{
#define LOG_GL_STRING(n) LogGLString(#n, n)

static const char* DEFAULT_WINDOW_TITLE = "YGOpen";
static const int DEFAULT_WINDOW_WIDTH = 1280;
static const int DEFAULT_WINDOW_HEIGHT = 720;

GameInstance::GameInstance()
{
	SDL_Log("GameInstance constructor");
}

GameInstance::~GameInstance()
{
	SDL_Log("GameInstance destructor");
	if(window != nullptr)
		SDL_DestroyWindow(window);
	if(glCtx != nullptr)
		SDL_GL_DeleteContext(glCtx);
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
}


} // YGOpen
