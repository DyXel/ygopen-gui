#include "game_data.hpp"

#include <string_view>
#include <SDL_mutex.h>
#include <SDL_image.h>

#include "sdl_utility.hpp"
#include "drawing/renderer.hpp"
#include "drawing/texture.hpp"

namespace YGOpen
{

GameData::~GameData()
{
	if(guiFontFile)
		SDL_RWclose(guiFontFile);
}

void GameData::InitLoad(Drawing::Renderer renderer)
{
	renderer = renderer;
	menuBkg = renderer->NewTexture();
}

void GameData::FinishLoad()
{
	while(!gpuPushQueue.empty())
	{
		auto p = std::move(gpuPushQueue.front());
		gpuPushQueue.pop();
		p.first->SetImage(p.second->w, p.second->h, p.second->pixels);
		SDL_FreeSurface(p.second);
	}
}

bool GameData::LoadConfigs()
{
	std::string path = std::string(CONFIG_PATH) + GLOBAL_CONFIG_FILENAME;
	cfgs = std::make_unique<Configs>();
	cfgs->global = DefaultGlobalConfig();
	SDL_Log("Loading global config file: %s", path.c_str());
	try
	{
		auto fileData = SDLU_TextFromFile(path);
		if(!fileData)
			throw std::invalid_argument(SDL_GetError());
		// Parse JSON and merge onto current configuration
		nlohmann::json j = nlohmann::json::parse(fileData.get());
		cfgs->global.merge_patch(j);
	}
	catch(const std::exception& e)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Could not apply user settings: %s", e.what());
		return false;
	}
// 	cfgs->global.dump(1, '\t', false,
// 	nlohmann::json::error_handler_t::replace);
	return true;
}

bool GameData::LoadGUIFont()
{
	const auto path = GAME_FONTS_PATH +
	                  cfgs->global["guiFontFile"].get<std::string>();
	const auto size = static_cast<int>(dpi / DEFAULT_DPI * 22.0f);
	SDL_Log("Loading gui font: %s", path.c_str());
	try
	{
		guiFontFile = SDL_RWFromFile(path.c_str(), "rb");
		if(guiFontFile == nullptr)
			throw std::invalid_argument(SDL_GetError());
		guiFont.LoadFont(guiFontFile, size);
	}
	catch(const std::exception& e)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Could not load font: %s", e.what());
		return false;
	}
	return true;
}

bool GameData::LoadBkgs()
{
	auto TryPushOne = [&](Drawing::Texture tex, std::string_view path)
	{
		if(!tex)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			             "Invalid texture when loading '%s'", path.data());
			return;
		}
		SDL_Surface* image = IMG_Load(path.data());
		if(image && (image = SDLU_SurfaceToRGBA32(image)))
		{
			gpuPushQueue.emplace(tex, image);
		}
		else
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			             "Unable to load '%s': %s\n", path.data(),
			             SDL_GetError());
		}
	};
	std::string basePath = GAME_IMAGES_PATH;
	TryPushOne(menuBkg, basePath + "menu_bkg.png");
	return true;
}

} // YGOpen
