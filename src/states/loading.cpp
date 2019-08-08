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

// Tasks
int LoadGlobalConfig(void* voidData)
{
	auto data = static_cast<GameData*>(voidData);
	SDL_LockMutex(data->cfgMutex);

	std::string path = fmt::format("{}{}", CONFIG_PATH, GLOBAL_CONFIG_FILENAME);
	data->cfgs = std::make_unique<Configs>();
	data->cfgs->global = DefaultGlobalConfig();
	SDL_Log("Loading global config file: %s", path.c_str());
	try
	{
		// Read file onto buffer.
		// NOTE: probably move onto its own function.
		SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
		if (rw == nullptr)
			throw std::invalid_argument(SDL_GetError());
		std::size_t fileSize = static_cast<std::size_t>(SDL_RWsize(rw));
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
			throw std::length_error("Error when reading file. Size mismatch");
		fileData[readTotal] = '\0';
		// Parse JSON and merge onto current configuration
		nlohmann::json j = nlohmann::json::parse(fileData.get());
		data->cfgs->global.merge_patch(j);
	}
	catch(std::exception& e)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_ERROR,
		            "Could not apply user settings: %s", e.what());
	}
// 	data->cfgs->global.dump(1, '\t', false,
// 	nlohmann::json::error_handler_t::replace);
	
	SDL_UnlockMutex(data->cfgMutex);
	return 1;
}

int LoadFont(void* voidData)
{
	auto data = static_cast<GameData*>(voidData);
	SDL_LockMutex(data->cfgMutex);
	
	const auto fp = data->cfgs->global["guiFontFile"].get<std::string>();
	std::string path = fmt::format("{}{}", GAME_FONTS_PATH, fp);
	const int guiFontSz = static_cast<int>(data->dpi / DEFAULT_DPI * 22.0f);
	SDL_Log("Loading gui font: %s", path.c_str());
	try
	{
		data->guiFontFile = SDL_RWFromFile(path.c_str(), "rb");
		if(data->guiFontFile == nullptr)
			throw std::invalid_argument(SDL_GetError());
		data->guiFont.LoadFont(data->guiFontFile, guiFontSz);
	}
	catch(std::exception& e)
	{
		SDL_Log("Could not load font: %s", e.what());
	}
	SDL_UnlockMutex(data->cfgMutex);
	return 1;
}

// int LoadJob(void* voidData)
// {
// 	auto data = static_cast<GameData*>(voidData);
// 	SDL_LockMutex(data->cfgMutex);
// 	// Code here
// 	SDL_UnlockMutex(data->cfgMutex);
// 	return 1;
// }

Loading::Loading(GameData* ptrData) : data(ptrData)
{
	pendingJobs.emplace(&LoadGlobalConfig);
	pendingJobs.emplace(&LoadFont);
	totalJobs = pendingJobs.size();
}

Loading::~Loading()
{}

void Loading::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
	{
		pendingJobs = std::queue<SDL_ThreadFunction>();
		cancelled = true;
		SDL_LockMutex(data->cfgMutex); // Wait for pending task
	}
}

void Loading::Tick()
{
	int status = SDL_TryLockMutex(data->cfgMutex);
	if(status == 0) // Lock adquired
	{
		SDL_UnlockMutex(data->cfgMutex);
		if(pendingJobs.empty()) // We are out of tasks
		{
			if(!cancelled) // If tasks were not cancelled proceed normally
				data->instance.SetState(std::make_shared<State::Menu>(data));
		}
		else // Start next task, thread will unlock mutex when done
		{
			auto task = std::move(pendingJobs.front());
			pendingJobs.pop();
			SDL_Thread* t = SDL_CreateThread(task, "LOADTASK", data);
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
