#include "game_data.hpp"
#include "game_instance.hpp"

#include <SDL_mutex.h>

namespace YGOpen
{

GameData::GameData(GameInstance& gi) : instance(gi)
{}

GameData::~GameData()
{
	if(guiFontFile)
		SDL_RWclose(guiFontFile);
}

bool GameData::LoadConfigs()
{
	std::string path = std::string(CONFIG_PATH) + GLOBAL_CONFIG_FILENAME;
	cfgs = std::make_unique<Configs>();
	cfgs->global = DefaultGlobalConfig();
	SDL_Log("Loading global config file: %s", path.c_str());
	try
	{
		// Read file onto buffer.
		// NOTE: probably move onto its own function.
		SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
		if (rw == nullptr)
			throw std::invalid_argument(SDL_GetError());
		std::size_t fileSize = static_cast<std::size_t>(SDL_RWsize(rw));
		std::unique_ptr<char[]> fileData(new char[fileSize + 1]);
		std::size_t readTotal = 0, read = 1;
		char* buf = fileData.get();
		while(readTotal < fileSize && read != 0)
		{
			read = SDL_RWread(rw, buf, 1, (fileSize - readTotal));
			readTotal += read;
			buf += read;
		}
		SDL_RWclose(rw);
		if(readTotal != fileSize)
			throw std::length_error("Error when reading file. Size mismatch");
		fileData[readTotal] = '\0';
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

} // YGOpen
