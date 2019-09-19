#ifndef LOADING_HPP
#define LOADING_HPP
#include <memory>
#include <queue>
#include <SDL_thread.h>

#include "../state.hpp"
#include "../drawing/types.hpp"

namespace YGOpen
{

class GameInstance;
struct GameData;

namespace State
{

class Loading : public IState
{
public:
	Loading(GameInstance& gi, GameData& data, const Drawing::Renderer& renderer);
	virtual ~Loading();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
	
	SDL_mutex* taskMtx;
private:
	GameInstance& gi;
public:
	GameData& data; // Accessed by SDL_ThreadFunction
private:
	Drawing::Renderer renderer;
	bool cancelled{false};
	std::queue<SDL_ThreadFunction> pendingJobs;
	std::queue<SDL_ThreadFunction>::size_type totalJobs;
};

} // State

} // YGOpen

#endif // LOADING_HPP
