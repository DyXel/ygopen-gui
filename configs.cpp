#include "configs.hpp"

namespace YGOpen
{

Configs DefaultConfigs()
{
	Configs c{};
	c.global = nlohmann::json{};
	return c;
}

}
