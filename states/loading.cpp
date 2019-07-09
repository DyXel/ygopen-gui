#include "loading.hpp"
#include "menu.hpp"
#include "../configs.hpp"
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

Loading::Loading(GameInstance& gi, std::shared_ptr<Configs> cfgs) : gi(gi)
{
	SDL_Log("State::Loading constructor");
	pendingJobs.emplace([cfgs]() -> bool
	{
		std::string path = fmt::format("{}{}", CONFIG_PATH,
		                               GLOBAL_CONFIG_FILENAME);
		cfgs->global = DefaultGlobalConfig();
		try
		{
			SDL_Log("Loading global config file: %s", path.c_str());
			std::ifstream f(path);
			nlohmann::json j;
			f >> j;
			cfgs->global.merge_patch(j);
		}
		catch(std::exception& e)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR,
			             "Could not apply user settings: %s", e.what());
		}
		std::ofstream f(path);
		f << cfgs->global.dump(1, '\t', false,
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
		gi.SetState(std::make_shared<State::Menu>());
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

}

} // State
} // YGOpen
