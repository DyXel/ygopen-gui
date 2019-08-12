#ifndef LOADING_HPP
#define LOADING_HPP
#include <memory>
#include <queue>
#include <SDL_thread.h>
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
	
	SDL_mutex* taskMtx;
	GameData* data; // Accessed by SDL_ThreadFunction
private:
	bool cancelled{false};
	std::queue<SDL_ThreadFunction> pendingJobs;
	std::queue<SDL_ThreadFunction>::size_type totalJobs;
};

} // State

} // YGOpen

#endif // LOADING_HPP
