#ifndef CONFIGS_HPP
#define CONFIGS_HPP
#include <static_configs.hpp>
#include <nlohmann/json.hpp>

namespace YGOpen
{

struct Configs
{
	nlohmann::json global;
};

nlohmann::json DefaultGlobalConfig();

} // YGOpen

#endif // CONFIGS_HPP
