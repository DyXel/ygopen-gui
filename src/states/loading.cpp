#include "loading.hpp"
#include "menu.hpp"
#include "../game_data.hpp"
#include "../game_instance.hpp"

#include <string>

#include <SDL_mutex.h>
#include <SDL_rwops.h>
#include <fmt/format.h>

namespace YGOpen
{
namespace State
{

#define TASKS() \
	X(LoadConfigs) \
	X(LoadGUIFont)

#define X(func) \
	int TASK_##func(void* voidData) \
	{ \
		auto loading = static_cast<Loading*>(voidData); \
		SDL_LockMutex(loading->taskMtx); \
		loading->data->func(); \
		SDL_UnlockMutex(loading->taskMtx); \
		return 1; \
	}
TASKS()
#undef X

Loading::Loading(GameData* ptrData) : data(ptrData)
{
	taskMtx = SDL_CreateMutex();
	if(taskMtx == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateMutex: %s\n",
		                SDL_GetError());
		data->instance.Exit();
		cancelled = true;
		return;
	}
#define X(func) pendingJobs.emplace(&TASK_##func);
TASKS()
#undef X
	totalJobs = pendingJobs.size();
}

Loading::~Loading()
{
	if(taskMtx)
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
				data->instance.SetState(std::make_shared<State::Menu>(data));
		}
		else // Start next task, thread will unlock mutex when done
		{
			auto task = std::move(pendingJobs.front());
			pendingJobs.pop();
			SDL_Thread* t = SDL_CreateThread(task, "LOADTASK", this);
			if(t == nullptr)
			{
				SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
				                "SDL_CreateThread: %s\n",
				                SDL_GetError());
				data->instance.Exit();
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
{
	Drawing::API::Clear();
	Drawing::API::Present();
}

} // State
} // YGOpen
