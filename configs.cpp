#include "configs.hpp"

namespace YGOpen
{

static const nlohmann::json DEFAULT_GLOBAL_CONFIG_JSON =
{
	{"lang", "en"},
	{"online", true},
	{"fullscreen", false},
	{"enableMusic", true},
	{"enableSfx", true}
};

nlohmann::json DefaultGlobalConfig()
{
	return DEFAULT_GLOBAL_CONFIG_JSON;
}

}
