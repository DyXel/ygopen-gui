#ifndef __GAME_INSTANCE_HPP__
#define __GAME_INSTANCE_HPP__
#include <memory>
#include <SDL.h>

namespace YGOpen
{
struct CommonData;
namespace State
{
class IState;
}

class GameInstance
{
public:
	GameInstance();
	~GameInstance();
	
	GameInstance(const GameInstance&) = delete;
	GameInstance(GameInstance&&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;
	GameInstance& operator=(GameInstance&&) = delete;
	
	int Init(/*int argc, char argv**/);
	bool IsExiting() const;
	void PropagateEvent(const SDL_Event& e);
	void TickOnce();
	void DrawOnce();
	void SetState(std::shared_ptr<State::IState> newState);
private:
	bool exiting{false};
	SDL_Window* window{nullptr};

	std::shared_ptr<CommonData> data;
	std::shared_ptr<State::IState> state;
};

} // YGOpen

#endif // __GAME_INSTANCE_HPP__
