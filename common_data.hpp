#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP
#include <memory>
#include "game_instance.hpp"
#include "configs.hpp"

namespace YGOpen
{
// Data which is shared across the same GameInstance and its states,
// Which should include: live configuration, textures, synchronization
// primitives.. Basically everything that should be shared
// between states and the main GameInstance.
struct CommonData
{
	CommonData(GameInstance& gi) : gi(gi) {};
	GameInstance& gi; // Owner of this data
	// Synchronization data
	
	// Video data
	int canvasWidth{DEFAULT_WINDOW_WIDTH}; // Canvas width in pixels
	int canvasHeight{DEFAULT_WINDOW_HEIGHT}; // Canvas height in pixels
	
	// Miscellaneous data
	std::unique_ptr<Configs> cfgs;
	bool isRecording{false};
	bool isPowerSaving{false};
};

} // YGOpen

#endif // COMMON_DATA_HPP
