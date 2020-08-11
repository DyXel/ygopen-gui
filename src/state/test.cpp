#include "test.hpp"

#include "menu.hpp"

namespace YGOpen::State
{

Test::Test(Service& svc) :
	svc(svc)
{}

void Test::OnEvent(const SDL_Event& e)
{}

void Test::Tick()
{}

} // namespace YGOpen::State
