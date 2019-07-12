#ifndef __CONFIGS_HPP__
#define __CONFIGS_HPP__
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

#endif // __CONFIGS_HPP__
