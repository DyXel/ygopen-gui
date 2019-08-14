#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP
#include <memory>
#include <queue>
#include "configs.hpp"
#include "text_smith.hpp"
#include "drawing/types.hpp"

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
	~GameData();
	GameInstance& instance; // Owner of this data
	
	// Values updated by GameInstance
	float elapsed; // Time elapsed since last draw call.
	int canvasWidth{DEFAULT_WINDOW_WIDTH}; // Canvas width in pixels
	int canvasHeight{DEFAULT_WINDOW_HEIGHT}; // Canvas height in pixels
	float dpi{}; // DPI of the screen, may be overriden by user settings
	bool powerSaving{false};
	
	void InitLoad();
	void FinishLoad();
	
	// The following functions are called on a different thread they represent
	// the loadable content, each one loads the members that follows them
	bool LoadConfigs();
	std::unique_ptr<Configs> cfgs;
	
	bool LoadGUIFont();
	SDL_RWops* guiFontFile{nullptr};
	TextSmith guiFont;
	
	bool LoadBkgs();
	Drawing::Texture menuBkg;
private:
	// Queue used to push image data down the GPU memory
	// NOTE: needed because OpenGL is not multithreaded
	std::queue<std::pair<Drawing::Texture, SDL_Surface*>> gpuPushQueue;
};

} // YGOpen

#endif // GAME_DATA_HPP
