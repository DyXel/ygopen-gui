#ifndef LOADING_HPP
#define LOADING_HPP
#include <memory>
#include <queue>
#include <future>
#include "../state.hpp"

namespace YGOpen
{
struct CommonData;
namespace State
{

class Loading : public IState
{
public:
	Loading(const std::shared_ptr<CommonData>& ptrData);
	virtual ~Loading();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
private:
	using LoadJob = std::packaged_task<bool()>;
	std::queue<LoadJob> pendingJobs;
	std::queue<LoadJob>::size_type totalJobs;
	std::future<bool> lastJob;
	std::shared_ptr<CommonData> data;
};

} // State
} // YGOpen

#endif // LOADING_HPP
