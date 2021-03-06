#include "loading.hpp"

#include <string>

#include <SDL_mutex.h>
#include <SDL_rwops.h>
#include <fmt/format.h>

#include "../drawing/renderer.hpp"
#include "../game_data.hpp"
#include "../game_instance.hpp"
#include "menu.hpp"

namespace YGOpen
{
namespace State
{

#define TASKS() \
	X(LoadConfigs) \
	X(LoadGUIFont) \
	X(LoadBkgs)

#define X(func) \
	static int TASK_##func(void* voidData) \
	{ \
		auto loading = static_cast<Loading*>(voidData); \
		SDL_LockMutex(loading->taskMtx); \
		loading->data.func(); \
		SDL_UnlockMutex(loading->taskMtx); \
		return 1; \
	}
TASKS()
#undef X

Loading::Loading(GameInstance& gi, GameData& data, Drawing::Renderer renderer) :
	gi(gi), data(data), renderer(renderer)
{
	taskMtx = SDL_CreateMutex();
	if(taskMtx == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateMutex: %s\n",
		                SDL_GetError());
		gi.Exit();
		cancelled = true;
		return;
	}
	data.InitLoad(renderer);
#define X(func) pendingJobs.emplace(&TASK_##func);
TASKS()
#undef X
	totalJobs = pendingJobs.size();
}

Loading::~Loading()
{
	if(taskMtx != nullptr)
		SDL_DestroyMutex(taskMtx);
}

void Loading::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
	{
		pendingJobs = std::queue<SDL_ThreadFunction>();
		cancelled = true;
		SDL_LockMutex(taskMtx); // Wait for pending task
	}
}

void Loading::Tick()
{
	int status = SDL_TryLockMutex(taskMtx);
	if(status == 0) // Lock adquired
	{
		SDL_UnlockMutex(taskMtx);
		if(pendingJobs.empty()) // We are out of tasks
		{
			if(!cancelled) // If tasks were not cancelled proceed normally
			{
				data.FinishLoad();
				gi.SetState(std::make_shared<State::Menu>(gi, data, renderer));
			}
		}
		else // Start next task, thread will unlock mutex when done
		{
			auto task = pendingJobs.front();
			pendingJobs.pop();
			SDL_Thread* t = SDL_CreateThread(task, "LOADTASK", this);
			if(t == nullptr)
			{
				SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				                "SDL_CreateThread: %s\n",
				                SDL_GetError());
				gi.Exit();
				return;
			}
			SDL_DetachThread(t);
		}
	}
	else // status == SDL_MUTEX_TIMEDOUT // Waiting...
	{
		
	}
}

void Loading::Draw()
{}

}  // namespace State
}  // namespace YGOpen
