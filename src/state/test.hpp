#ifndef YGOPEN_STATE_TEST_HPP
#define YGOPEN_STATE_TEST_HPP
#include "../state.hpp"

namespace YGOpen::State
{

class Test final
{
public:
	Test(Service& svc);
	
	void OnEvent(const SDL_Event& e);
	void Tick();
private:
	Service& svc;
};

} // namespace YGOpen::State

#endif // YGOPEN_STATE_TEST_HPP
