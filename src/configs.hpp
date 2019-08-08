#ifndef CONFIGS_HPP
#define CONFIGS_HPP
#include <nlohmann/json.hpp>

namespace YGOpen
{

extern const char* CONFIG_PATH;
extern const char* CARD_PICTURES_PATH;
extern const char* CARD_DATABASES_PATH;
extern const char* GAME_IMAGES_PATH;
extern const char* GAME_SOUNDS_PATH;
extern const char* GAME_FONTS_PATH;

extern const char* GLOBAL_CONFIG_FILENAME;

extern const char* DEFAULT_WINDOW_TITLE;
extern const int DEFAULT_WINDOW_WIDTH;
extern const int DEFAULT_WINDOW_HEIGHT;
extern const float DEFAULT_DPI;

struct Configs
{
	nlohmann::json global;
};

nlohmann::json DefaultGlobalConfig();

} // YGOpen

#endif // CONFIGS_HPP
