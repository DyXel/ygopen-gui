#ifndef LOADING_HPP
#define LOADING_HPP
#include <memory>
#include <queue>
#include <future>
#include "../state.hpp"

namespace YGOpen
{

struct GameData;

namespace State
{

class Loading : public IState
{
public:
	Loading(GameData* ptrData);
	virtual ~Loading();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
	
	GameData* data; // Accessed by SDL_ThreadFunction
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
