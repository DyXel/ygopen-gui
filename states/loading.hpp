#ifndef __LOADING_HPP__
#define __LOADING_HPP__
#include <memory>
#include <queue>
#include <future>
#include "../state.hpp"

namespace YGOpen
{
class GameInstance;
class Configs;
namespace State
{

using LoadJob = std::packaged_task<bool()>;

class Loading : public IState
{
public:
	Loading(GameInstance& gi, std::shared_ptr<Configs> cfgs);
	virtual ~Loading();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
private:
	std::queue<LoadJob> pendingJobs;
	std::queue<LoadJob>::size_type totalJobs;
	std::future<bool> lastJob;
	GameInstance& gi;
};

} // State
} // YGOpen

#endif // __LOADING_HPP__
