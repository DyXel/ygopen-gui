#include "configs.hpp"
#include "SDL_platform.h"

namespace YGOpen
{

const char* CONFIG_PATH = u8"configs/";
const char* CARD_PICTURES_PATH = u8"pics/";
const char* CARD_DATABASES_PATH = u8"dbs/";
const char* GAME_IMAGES_PATH = u8"assets/textures/";
const char* GAME_SOUNDS_PATH = u8"assets/sounds/";
const char* GAME_FONTS_PATH = u8"assets/fonts/";

const char* GLOBAL_CONFIG_FILENAME = u8"global.json";

const char* DEFAULT_WINDOW_TITLE = u8"YGOpen";
#ifndef __ANDROID__
const int DEFAULT_WINDOW_WIDTH = 1280;
const int DEFAULT_WINDOW_HEIGHT = 720;
#else // let android figure it out
const int DEFAULT_WINDOW_WIDTH = 0;
const int DEFAULT_WINDOW_HEIGHT = 0;
#endif
const float DEFAULT_DPI = 72.0f;

static const nlohmann::json DEFAULT_GLOBAL_CONFIG_JSON =
{
	{"lang", "en"},
	{"online", true},
	{"fullscreen", false},
	{"enableMusic", true},
	{"enableSfx", true},
	{"guiFontFile", "NotoSansJP-Regular.otf"},
};

nlohmann::json DefaultGlobalConfig()
{
	return DEFAULT_GLOBAL_CONFIG_JSON;
}

} // YGOpen
