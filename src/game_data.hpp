#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP
#include <memory>
#include "configs.hpp"

struct SDL_mutex;

namespace YGOpen
{

class GameInstance;

// Data which is shared across the same GameInstance and its states,
// Which should include: live configuration, textures, synchronization
// primitives.. Basically everything that should be shared
// between states and the main GameInstance.
struct GameData
{
	GameData(GameInstance& gi);
	int Init();
	GameInstance& instance; // Owner of this data
	
	// Synchronization data
	SDL_mutex* cfgMutex;
	
	// Video data
	int canvasWidth{DEFAULT_WINDOW_WIDTH}; // Canvas width in pixels
	int canvasHeight{DEFAULT_WINDOW_HEIGHT}; // Canvas height in pixels
	float dpi; // DPI of the screen, may be overriden by user settings
	
	// Time data
	float elapsed;
	
	// Miscellaneous data
	std::unique_ptr<Configs> cfgs;
	bool powerSaving{false};
};

} // YGOpen

#endif // GAME_DATA_HPP
