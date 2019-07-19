#include "loading.hpp"
#include "menu.hpp"
#include "../game_data.hpp"
#include "../game_instance.hpp"

#include <string>
#include <fstream>
#include <exception>
#include <thread>

#include <fmt/format.h>

namespace YGOpen
{
namespace State
{

Loading::Loading(GameData* ptrData) : data(ptrData)
{
	SDL_Log("State::Loading constructor");
	pendingJobs.emplace([this]() -> bool
	{
		data->cfgs = std::make_unique<Configs>();
		std::string path = fmt::format("{}{}", CONFIG_PATH,
		                               GLOBAL_CONFIG_FILENAME);
		data->cfgs->global = DefaultGlobalConfig();
		try
		{
			SDL_Log("Loading global config file: %s", path.c_str());
			std::ifstream f(path);
			nlohmann::json j;
			f >> j;
			data->cfgs->global.merge_patch(j);
		}
		catch(std::exception& e)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,
			             "Could not apply user settings: %s", e.what());
		}
		std::ofstream f(path);
		f << data->cfgs->global.dump(1, '\t', false,
		                             nlohmann::json::error_handler_t::replace);
		return true;
	});
	
	totalJobs = pendingJobs.size();
}

Loading::~Loading()
{
	SDL_Log("State::Loading destructor");
}

void Loading::OnEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
	{
		pendingJobs = std::queue<LoadJob>();
		if(lastJob.valid())
			lastJob.wait();
	}
}

void Loading::Tick()
{
	using namespace std::chrono_literals;
	if(pendingJobs.empty() && lastJob.valid() &&
	   lastJob.wait_for(0ms) == std::future_status::ready)
	{
		data->gi.SetState(std::make_shared<State::Menu>(data));
	}
	else if(!pendingJobs.empty())
	{
		if(!lastJob.valid() || (lastJob.valid() &&
		   lastJob.wait_for(0ms) == std::future_status::ready))
		{
			LoadJob job(std::move(pendingJobs.front()));
			pendingJobs.pop();
			lastJob = job.get_future();
			std::thread t(std::move(job));
			t.detach();
		}
	}
}

void Loading::Draw()
{
	Drawing::API::Clear();
	Drawing::API::Present();
}

} // State
} // YGOpen
