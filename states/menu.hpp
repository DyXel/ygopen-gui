#ifndef MENU_HPP
#define MENU_HPP
#include <memory>
#include "../state.hpp"
#include "../drawing/api.hpp"
#include "../drawing/primitive.hpp"

namespace OGL
{
class Program;
class Mesh;
}

namespace YGOpen
{
struct CommonData;
namespace State
{

class Menu : public IState
{
public:
	Menu(const std::shared_ptr<CommonData>& ptrData);
	virtual ~Menu();
	void OnEvent(const SDL_Event& e) override;
	void Tick() override;
	void Draw() override;
private:
	std::shared_ptr<CommonData> data;
	Drawing::Primitive triangle;
};

} // State
} // YGOpen

#endif // MENU_HPP
